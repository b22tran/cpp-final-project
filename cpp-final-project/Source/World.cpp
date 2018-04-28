#include "World.hpp"
#include "Foreach.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>


World::World(sf::RenderWindow& window)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mTextures() 
, mSceneGraph()
, mSceneLayers()
, mWorldBounds(0.f, 0.f, 960.0f, 460.f)
, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y / 2.f)
, mScrollSpeed(0.f)
, mPlayerCharacter(nullptr)
, mEnemyCharacter(nullptr)
{
	loadTextures();
	buildScene();

	// Prepare the view
	mPlayerCharacter->setPosition(600.f, mWorldBounds.height - mWorldView.getSize().y / 2);
	mEnemyCharacter->setPosition(300.f, mWorldBounds.height - mWorldView.getSize().y / 2);
}

void World::update(sf::Time dt)
{

	mPlayerCharacter->setVelocity(0.f, 150.f);
	mEnemyCharacter->setVelocity(0.f, 150.f);

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty()) {
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
		adaptPlayerVelocity();
		adaptEnemyVelocity();
	}

	//handle collisions
	handleCollisions();
	destroyEntitiesOutsideView();
	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
	adaptEnemyPosition();
}

void World::draw()
{
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::charDead() const {
	int p1 = mPlayerCharacter->getHP();
	int p2 = mEnemyCharacter->getHP();
	if (p1<=0)
		return true;
	else if (p2<=0)
		return true;
	else
		return false;
}

void World::loadTextures()
{
	mTextures.load(Textures::Player, "Media/Textures/Char1.png");
	mTextures.load(Textures::Enemy, "Media/Textures/Char2.png");
	mTextures.load(Textures::Background, "Media/Textures/Background-dl.jpg");
	mTextures.load(Textures::Ball, "Media/Textures/BulletD.png");
}



void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 30.0f;

	sf::Vector2f position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance - 30.f);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top);
	if (position.x > 150.0f && position.x < 755) {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - 100.0f - (borderDistance * 2));
	}
	else {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - (borderDistance * 1.5f));
	}
	mPlayerCharacter->setPosition(position);
}

void World::adaptEnemyPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 30.0f;

	sf::Vector2f position = mEnemyCharacter->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance - 30.f);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top);
	if (position.x > 150.0f && position.x < 755) {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - 100.0f - (borderDistance * 2));
	}
	else {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - (borderDistance * 1.5f));
	}
	mEnemyCharacter->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayerCharacter->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerCharacter->setVelocity(velocity / std::sqrt(2.f));
}

void World::adaptEnemyVelocity()
{
	sf::Vector2f velocity = mEnemyCharacter->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mEnemyCharacter->setVelocity(velocity / std::sqrt(2.f));
}

void World::buildScene(){
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)	{
		Category::Type category = (i == Air) ? Category::Scene : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();
		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& texture = mTextures.get(Textures::Background);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

	// Add player's character
	std::unique_ptr<Character> player(new Character(Character::Player, mTextures));
	mPlayerCharacter = player.get();
	mPlayerCharacter->setPosition(mSpawnPosition);
	mSceneLayers[Air]->attachChild(std::move(player));

	// Add second player's character
	std::unique_ptr<Character> enemy(new Character(Character::Enemy, mTextures));
	mEnemyCharacter = enemy.get();
	mEnemyCharacter->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Air]->attachChild(std::move(enemy));

}


float World::randNum(float min, float max){
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

sf::FloatRect World::getViewBounds() const{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}


void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	FOREACH(SceneNode::Pair pair, collisionPairs) {
		if (matchesCategories(pair, Category::PlayerCharacter, Category::EnemyCharacter)) {
			//set first to play, second to enemy
			auto& player = static_cast<Character&>(*pair.first);
			auto& enemy = static_cast<Character&>(*pair.second);
			//std::cout << "You bumped into someone!" << std::endl << "Current HP: " << player.getHP() << std::endl;
		}

		//else if someone gets hit by a projectile
		else if (matchesCategories(pair, Category::EnemyCharacter, Category::PlayerBullet)
			|| matchesCategories(pair, Category::PlayerCharacter, Category::EnemyBullet)) {
			//set types
			auto& enemy = static_cast<Character&>(*pair.first);
			auto& projectile = static_cast<Weapon&>(*pair.second);

			// Apply projectile damage to character, destroy projectile
			std::cout << "Projectile hit someone!" << std::endl << "Enemy HP: " << enemy.getHP() << std::endl;;
			enemy.dmg(projectile.getDamage());
			projectile.destroy();
		}
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::Character;
	command.action = derivedAction<Actor>([this](Actor& e, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
			e.remove();
	});

	mCommandQueue.push(command);
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}
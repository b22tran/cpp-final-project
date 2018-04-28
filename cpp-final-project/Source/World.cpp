#include "World.hpp"
#include "Foreach.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

//creating view
//sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(400.0f, 400.0f));

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
	//mWorldView.setCenter(mSpawnPosition);
	mPlayerCharacter->setPosition(600.f, mWorldBounds.height - mWorldView.getSize().y / 2);
	mEnemyCharacter->setPosition(160.f, mWorldBounds.height - mWorldView.getSize().y / 2);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	//mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
	//mWorldView.setCenter(mWorldView.getSize().x / 2.f, mWorldView.getSize().y / 2.f);
	mPlayerCharacter->setVelocity(0.f, 150.f);
	mEnemyCharacter->setVelocity(0.f, 150.f);

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty()) {
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
		adaptPlayerVelocity();
		adaptEnemyVelocity();
	}


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

void World::loadTextures()
{
	mTextures.load(Textures::Player, "Media/Textures/Char1.png");
	mTextures.load(Textures::Enemy, "Media/Textures/Char2.png");
	mTextures.load(Textures::Background, "Media/Textures/Background-dl.jpg");
}



void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 50.0f;

	sf::Vector2f position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance - 30.f);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top);
	if (position.x > 145.0f && position.x < 755) {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - 100.0f - (borderDistance * 2));
	}
	else {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - (borderDistance * 2));
	}
	mPlayerCharacter->setPosition(position);
}

void World::adaptEnemyPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 50.0f;

	sf::Vector2f position = mEnemyCharacter->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance - 30.f);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	if (position.x > 145.0f && position.x < 755) {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - 100.0f - (borderDistance * 2));
	}
	else {
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - (borderDistance * 2));
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



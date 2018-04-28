#include "World2.hpp"
#include "Foreach.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

//creating view
//sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(400.0f, 400.0f));

World2::World2(sf::RenderWindow& window)
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
	mEnemyCharacter->setPosition(100.f, mWorldBounds.height - mWorldView.getSize().y / 2);
}

void World2::update(sf::Time dt)
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

void World2::draw()
{
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World2::getCommandQueue()
{
	return mCommandQueue;
}

void World2::loadTextures()
{
	mTextures.load(Textures::Player, "Media/Textures/Char1.png");
	mTextures.load(Textures::Enemy, "Media/Textures/Char2.png");
	mTextures.load(Textures::Background, "Media/Textures/Background-2.png");
}



void World2::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 30.0f;

	sf::Vector2f position = mPlayerCharacter->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance - 30.f);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height -90.0f- (borderDistance * 1.5f));
	mPlayerCharacter->setPosition(position);
}

void World2::adaptEnemyPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 30.0f;

	sf::Vector2f position = mEnemyCharacter->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance - 30.f);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - 90.0f - (borderDistance * 1.5f));
	mEnemyCharacter->setPosition(position);
}

void World2::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayerCharacter->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerCharacter->setVelocity(velocity / std::sqrt(2.f));
}

void World2::adaptEnemyVelocity()
{
	sf::Vector2f velocity = mEnemyCharacter->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mEnemyCharacter->setVelocity(velocity / std::sqrt(2.f));
}

void World2::buildScene() {
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i) {
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


float World2::randNum(float min, float max) {
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

sf::FloatRect World2::getViewBounds() const {
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}




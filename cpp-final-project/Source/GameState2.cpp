#include "GameState2.hpp"

GameState2::GameState2(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(*context.window)
	, mPlayer(*context.player)
	, mEnemy(*context.enemy)
{
}

void GameState2::draw()
{
	mWorld.draw();
}

bool GameState2::update(sf::Time dt)
{
	mWorld.update(dt);

	if (mWorld.charDead()) {
		mPlayer.setGameStatus(Player::GameOver);
		requestStackPush(States::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);
	mEnemy.handleRealtimeInput(commands);
	return true;
}

bool GameState2::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);
	mEnemy.handleEvent(event, commands);
	
	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);

	return true;
}
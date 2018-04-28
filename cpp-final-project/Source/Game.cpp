#include "Game.hpp"
#include "Utility.hpp"
#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "TitleState.hpp"
#include "GameState.hpp"
#include "GameState2.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "SettingState.hpp"
#include "StringHelpers.hpp"


const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);
//creating view
//sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(400.0f, 400.0f));

Game::Game() : mWindow(sf::VideoMode(960, 460), "Super Slash Bros!", sf::Style::Close)
, mWorld(mWindow)
, mPlayer()
, mEnemy()
, mFonts()
, mStateStack(State::Context(mWindow, mTextures, mFonts, mPlayer, mEnemy))
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);
	mFonts.load(Fonts::Main, "Media/kenny.ttf");
	mTextures.load(Textures::TitleScreen, "Media/Textures/Title.jpg");
	mTextures.load(Textures::ButtonNormal, "Media/Textures/ButtonNormal.png");
	mTextures.load(Textures::ButtonSelected, "Media/Textures/ButtonSelected.png");
	mTextures.load(Textures::ButtonPressed, "Media/Textures/ButtonPressed.png");

	mStatisticsText.setFont(mFonts.get(Fonts::Main));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(States::Title);
}

void Game::run(){
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen()){
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;

		while (timeSinceLastUpdate > TimePerFrame){

			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			// Check inside this loop, because stack might be empty before update() call
			if (mStateStack.isEmpty())
				
				mWindow.close();
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processInput(){

	CommandQueue& commands = mWorld.getCommandQueue();

	sf::Event event;
	while (mWindow.pollEvent(event)){
		mStateStack.handleEvent(event);
		mPlayer.handleEvent(event, commands);
		mEnemy.handleEvent(event, commands);
		if (event.type == sf::Event::Closed)
			mWindow.close();
	}

	mPlayer.handleRealtimeInput(commands);
	mEnemy.handleRealtimeInput(commands);
}

void Game::update(sf::Time elapsedTime){
	mStateStack.update(elapsedTime);
	//mWorld.update(elapsedTime);
}

void Game::render(){
	mWindow.clear();

	mStateStack.draw();
	//mWorld.draw();

	mWindow.setView(mWindow.getDefaultView());
//	mWindow.setView(view);
	mWindow.draw(mStatisticsText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime){
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f)){
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");
							 
		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}


void Game::registerStates()
{
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<GameState>(States::Game);
	mStateStack.registerState<GameState2>(States::Game2);
	mStateStack.registerState<PauseState>(States::Pause);
	mStateStack.registerState<SettingsState>(States::Settings);
}

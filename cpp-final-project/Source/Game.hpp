#pragma once
#include "World.hpp"
#include "Player.hpp"
#include "Enemy.h"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "StateStack.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


class Game : private sf::NonCopyable{
	public:
		Game();
		void run();

private:
		void processInput();
		void update(sf::Time elapsedTime);
		void render();
		void updateStatistics(sf::Time elapsedTime);
		void registerStates();
		static const sf::Time TimePerFrame;

		sf::RenderWindow mWindow;
		World mWorld;
		Player mPlayer;
		Enemy mEnemy;
		StateStack mStateStack;
		TextureHolder			mTextures;
		FontHolder				mFonts;
		sf::Text mStatisticsText;
		sf::Time mStatisticsUpdateTime;
		std::size_t mStatisticsNumFrames;


};

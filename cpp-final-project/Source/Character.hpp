#pragma once
#include "Actor.hpp"
#include "ResourceIdentifiers.hpp"
#include "Weapon.h"
#include "Command.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>


class Character : public Actor
{
	public:
		enum Type {
			Player,
			Enemy,
			TypeCount,
		};


	public:
		Character(Type type, const TextureHolder& textures);

		void shoot();
		virtual unsigned int getCategory() const;
		float getMaxSpeed() const;
				float toRadian(float degree);
		bool isAllied() const;
		void jumpInterval();

		virtual sf::FloatRect getBoundingRect() const;



	private:
		virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		void checkBulletLaunch(sf::Time dt, CommandQueue& commands);
		void createBullets(SceneNode& node, const TextureHolder& textures) const;
		void createProjectile(SceneNode& node, Weapon::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
		void updateMovementPattern(sf::Time dt);
		void updateCurrent(sf::Time dt, CommandQueue& commands);
		void checkJumpRate(sf::Time dt, CommandQueue& commands);

		bool mJumpRate;
		int mJumpRateLevel;
		int mFireRateLevel;
		float mTravelledDistance;
		bool mIsFiring;

		Type mType;
		Command mFireCommand;
		sf::Time mFireCountdown;
		std::size_t mDirectionIndex;
		sf::Sprite mSprite;
		sf::Time mJumpCountdown;
		Command mJumpCommand;
};


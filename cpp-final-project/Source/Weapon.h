#pragma once
#include "Actor.hpp"
#include "ResourceIdentifiers.hpp"
#include <SFML/Graphics/Sprite.hpp>

class Weapon : public Actor
{
public:
	enum Type	{
		PlayerBullet,
		EnemyBullet,
		TypeCount,
	};

	Weapon(Type type, const TextureHolder& textures);

	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	float getMaxSpeed() const;
	int getDamage() const;
	void centerOrigin(sf::Sprite& sprite);


private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	Type mType;
	sf::Sprite mSprite;
	sf::Vector2f mTargetDirection;
};
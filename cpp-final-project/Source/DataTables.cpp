#include "DataTables.h"
#include "Character.hpp"
#include "Weapon.h"

// For std::bind() placeholders
using namespace std::placeholders;

float randNum(float min, float max) {
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}
std::vector<CharacterData> initializeCharacterData(){
	//typecount is is the last in the enum..
	std::vector<CharacterData> data(Character::TypeCount);

	data[Character::Player].hp = 10;
	data[Character::Player].speed = 400.f;
	data[Character::Player].jumpInterval = sf::seconds(2);
	data[Character::Player].fireInterval = sf::seconds(2);
	data[Character::Player].texture = Textures::Player;

	data[Character::Enemy].hp = 10;
	data[Character::Enemy].speed = 400.f;
	data[Character::Enemy].jumpInterval = sf::seconds(2);
	data[Character::Enemy].fireInterval = sf::seconds(2);
	data[Character::Enemy].texture = Textures::Enemy;

	return data;
}


std::vector<WeaponData> initializeWeaponData()
{
	std::vector<WeaponData> data(Weapon::TypeCount);

	data[Weapon::PlayerBullet].dmg = 10;
	data[Weapon::PlayerBullet].speed = 350.f;
	data[Weapon::PlayerBullet].texture = Textures::Ball;

	data[Weapon::EnemyBullet].dmg = 10;
	data[Weapon::EnemyBullet].speed = 350.f;
	data[Weapon::EnemyBullet].texture = Textures::Ball;

	return data;
}


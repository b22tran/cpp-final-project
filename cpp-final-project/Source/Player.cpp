#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Character.hpp"
#include "Foreach.hpp"
#include <iostream>
#include <map>
#include <string>
#include <algorithm>


struct CharacterMover{
	CharacterMover(float vx, float vy)
	: velocity(vx, vy){
	}

	void operator() (Character& character, sf::Time) const{
		character.accelerate(velocity);
	}

	sf::Vector2f velocity;
};
struct CharacterJumper {
	CharacterJumper(float vx, float vy)
		: velocity(vx, vy) {
	}

	void operator() (Character& character, sf::Time) const {
		if (character.mJumpRate && character.mJumpCountdown <= sf::Time::Zero) {
			character.accelerate(velocity);
			character.mJumpRate = false;
		}
	}

	sf::Vector2f velocity;
};
Player::Player(Player::PlayerType type){
	mType = type;
	// Set initial key bindings
	if (mType == PlayerType::P1) {
		mKeyBinding[sf::Keyboard::Up] = MoveUp;
		mKeyBinding[sf::Keyboard::Down] = MoveDown;
		mKeyBinding[sf::Keyboard::Left] = MoveLeft;
		mKeyBinding[sf::Keyboard::Right] = MoveRight;
		mKeyBinding[sf::Keyboard::RShift] = Attack;
		// Set initial action bindings
		initializeActions();

		// Assign all categories to player's body
		FOREACH(auto& pair, mActionBinding)
			pair.second.category = Category::PlayerCharacter;
	}
	if (mType == PlayerType::P2) {
		mKeyBinding2[sf::Keyboard::W] = MoveUp2;
		mKeyBinding2[sf::Keyboard::S] = MoveDown2;
		mKeyBinding2[sf::Keyboard::A] = MoveLeft2;
		mKeyBinding2[sf::Keyboard::D] = MoveRight2;
		mKeyBinding2[sf::Keyboard::F] = Attack2;// Set initial action bindings
		initializeActions();

		// Assign all categories to player's body
		FOREACH(auto& pair, mActionBinding)
			pair.second.category = Category::EnemyCharacter;
	}
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands) {
	if (event.type == sf::Event::KeyPressed){
		if (mType == PlayerType::P1) {
			// Check if pressed key appears in key binding, trigger command if so
			auto found = mKeyBinding.find(event.key.code);
			if (found != mKeyBinding.end() && !isRealtimeAction(found->second)) {
				std::cout << "p1 cmd push" << std::endl;
				commands.push(mActionBinding[found->second]);
			}
		} else if(mType == PlayerType::P2) {
			// Check if pressed key appears in key binding, trigger command if so
			auto found = mKeyBinding2.find(event.key.code);
			if (found != mKeyBinding2.end() && !isRealtimeAction(found->second)) {
				std::cout << "p2 cmd push" << std::endl;
				commands.push(mActionBinding[found->second]);
			}
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands){
	// Traverse all assigned keys and check if they are pressed
	if (mType == PlayerType::P1) {
		FOREACH(auto pair, mKeyBinding) {
			// If key is pressed, lookup action and trigger corresponding command
			if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second)) {
				std::cout << "p1 cmd push rt" << std::endl;
				commands.push(mActionBinding[pair.second]);
			}
		}
	}
	if (mType == PlayerType::P2) {
		FOREACH(auto pair, mKeyBinding2) {
			// If key is pressed, lookup action and trigger corresponding command
			if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second)) {
				std::cout << "p2 cmd push rt" << std::endl;
				commands.push(mActionBinding[pair.second]);
			}
		}
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key){
	// Remove all keys that already map to action
	if (mType == PlayerType::P1) {

		for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); ) {
			if (itr->second == action)
				mKeyBinding.erase(itr++);
			else
				++itr;
		}

		// Insert new binding
		mKeyBinding[key] = action;
	}
	else {
		for (auto itr = mKeyBinding2.begin(); itr != mKeyBinding2.end(); ) {
			if (itr->second == action)
				mKeyBinding2.erase(itr++);
			else
				++itr;
		}

		// Insert new binding
		mKeyBinding2[key] = action;
	}
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const{
	if (mType == PlayerType::P1) {
		FOREACH(auto pair, mKeyBinding) {
			if (pair.second == action)
				return pair.first;
		}
	}
	else if (mType == PlayerType::P2) {
		FOREACH(auto pair, mKeyBinding2) {
			if (pair.second == action)
				return pair.first;
		}
	}

	return sf::Keyboard::Unknown;
}

void Player::initializeActions(){
	const float playerSpeed = 250.f;
	if (mType == PlayerType::P1) {

		mActionBinding[MoveUp].action = derivedAction<Character>(CharacterJumper(0.f, -playerSpeed * 5));
		//mActionBinding[MoveDown].action = derivedAction<Character>(CharacterMover(0.f, +playerSpeed));
		mActionBinding[MoveLeft].action = derivedAction<Character>(CharacterMover(-playerSpeed * 2, 0.f));
		mActionBinding[MoveRight].action = derivedAction<Character>(CharacterMover(+playerSpeed * 2, 0.f));
		mActionBinding[Attack].action = derivedAction<Character>([](Character& c, sf::Time) {
			c.shoot();
		});
	}
	if (mType == PlayerType::P2){
		mActionBinding[MoveUp2].action = derivedAction<Character>(CharacterJumper(0.f, -playerSpeed * 5));
		//mActionBinding[MoveDown].action = derivedAction<Character>(CharacterMover(0.f, +playerSpeed));
		mActionBinding[MoveLeft2].action = derivedAction<Character>(CharacterMover(-playerSpeed * 2, 0.f));
		mActionBinding[MoveRight2].action = derivedAction<Character>(CharacterMover(+playerSpeed * 2, 0.f));
		mActionBinding[Attack2].action = derivedAction<Character>([](Character& c, sf::Time) {
			c.shoot();
		});
	}
}

bool Player::isRealtimeAction(Action action){
	switch (action)	{
		case MoveLeft:
		case MoveRight:
		case MoveDown:
		case MoveUp:
		case Attack:
		case MoveLeft2:
		case MoveRight2:
		case MoveDown2:
		case MoveUp2:
		case Attack2:
			return true;
		default:
			return false;
	}
}

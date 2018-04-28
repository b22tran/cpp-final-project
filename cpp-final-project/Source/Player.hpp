#pragma once
#include "Command.hpp"
#include <SFML/Window/Event.hpp>
#include <map>


class CommandQueue;

class Player{
	public:
		enum Action{
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			Attack,
			ActionCount
		};
		enum PlayerType {
			P1,
			P2
		};

	public:
		Player(PlayerType);
		void handleEvent(const sf::Event& event, CommandQueue& commands);
		void handleRealtimeInput(CommandQueue& commands);
		void assignKey(Action action, sf::Keyboard::Key key);

		sf::Keyboard::Key getAssignedKey(Action action) const;
		std::map<Action, Command>				mActionBinding;


	private:
		void initializeActions();
		static bool isRealtimeAction(Action action);

		bool canJump;
		PlayerType mType;
		std::map<sf::Keyboard::Key, Action>		mKeyBinding;
		std::map<sf::Keyboard::Key, Action>		mKeyBinding2;

};

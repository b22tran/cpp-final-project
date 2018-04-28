#ifndef BOOK_GAMESTATE2_HPP
#define BOOK_GAMESTATE2_HPP

#include "State.hpp"
#include "World2.hpp"
#include "Player.hpp"
#include "Enemy.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class GameState2 : public State
{
public:
						GameState2(StateStack& stack, Context context);

	virtual void		draw();
	virtual bool		update(sf::Time dt);
	virtual bool		handleEvent(const sf::Event& event);


private:
	World2				mWorld;
	Player&				mPlayer;
	Enemy&			mEnemy;
};

#endif // BOOK_GAMESTATE2_HPP
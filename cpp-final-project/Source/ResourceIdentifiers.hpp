#pragma once

namespace sf
{
	class Texture;
	class Font;
}

//all textures for scenenodes
namespace Textures{
	enum ID{
		PlayerUp,
		Player,
		PlayerLeft,
		PlayerRight,
		Enemy,
		Background,
		Ball,
		Bomb,
		TextureCount,
		ButtonNormal,
		ButtonSelected,
		ButtonPressed,
		TitleScreen

	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}

template <typename Resource, typename Identifier>
class ResourceHolder;
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>			FontHolder;

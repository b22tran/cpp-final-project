#pragma once

namespace sf
{
	class Texture;
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
		BulletZ,
		BulletD,
		BulletV,
		BulletV2,
		TextureCount

	};
}

template <typename Resource, typename Identifier>
class ResourceHolder;
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;


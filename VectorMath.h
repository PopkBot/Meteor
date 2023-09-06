#pragma once

#include "SFML/Graphics.hpp"

class VectorMath
{
public:

	static float lenght(sf::Vector3f vec) {
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	static sf::Vector3f normalize(sf::Vector3f vec) {
		return vec * lenght(vec);
	}
};


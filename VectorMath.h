#pragma once

#include "SFML/Graphics.hpp"

class VectorMath
{
public:

	static float lenght(sf::Vector3f vec) {
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	static sf::Vector3f normalize(sf::Vector3f vec) {
		if (lenght(vec) == 0) {
			return vec;
		}
		return vec / lenght(vec);
	}

	static float dot(sf::Vector3f vec1, sf::Vector3f vec2) {
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	static sf::Vector3f cross(sf::Vector3f vec1, sf::Vector3f vec2) {
		sf::Vector3f newVec;
		newVec.x = vec1.y * vec2.z - vec1.z * vec2.y;
		newVec.y = vec1.z * vec2.x - vec1.x * vec2.z;
		newVec.z = vec1.x * vec2.y - vec1.y * vec2.x;
		return newVec;
	}
};


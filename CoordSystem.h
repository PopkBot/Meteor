#pragma once
#include "SFML/Graphics.hpp"

class CoordSystem
{
public:
	sf::Vector3f vecX;
	sf::Vector3f vecY;
	sf::Vector3f vecZ;

	CoordSystem(sf::Vector3f vecX, sf::Vector3f vecY, sf::Vector3f vecZ) {
		this->vecX = vecX;
		this->vecY = vecY;
		this->vecZ = vecZ;
	}
};


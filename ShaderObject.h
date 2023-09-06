
#pragma once

#include "SFML/System.hpp"

class ShaderObject
{
public:
	sf::Vector3f position;
	sf::Vector3f colour;

	ShaderObject() {}

	ShaderObject(sf::Vector3f position, sf::Vector3f colour) {
		this->position = position;
		this->colour = colour;
	}

	
};


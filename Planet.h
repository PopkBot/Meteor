
#pragma once

#include "ShaderObject.h"

class Planet: public ShaderObject
{
public:
	float radius;
	float mu;

	Planet() {}

	Planet(sf::Vector3f position, sf::Vector3f colour, float radius, float mu)
		: ShaderObject(position,colour){
		this->radius = radius;
		this->mu = mu;
	}
};


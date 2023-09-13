#pragma once

#include "SFML/Graphics.hpp"

class FlyWheel
{
public:

	static constexpr float radSToRPM = 9.5493;

	float maxSpeed = 327;// rad/s
	float maxMoment = 0.25; // N*m
	float rotorInertia = 0.14;// kg m2
	float maxAcceleration = maxMoment / rotorInertia;
	float speed=0;
	sf::Vector3f direction;

	FlyWheel(sf::Vector3f direction) {
		this->direction = direction;
	}

	FlyWheel(){}

	float makeTorque(float moment, float dt) {
		
		if (abs(moment) > maxMoment) {
			moment = maxMoment*moment/abs(moment);
		}
		speed += moment / rotorInertia * dt * radSToRPM;
		return moment;

	}




};


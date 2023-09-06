#pragma once

#include "ShaderObject.h"
#include "Planet.h"
#include "VectorMath.h"


class Satellite: public ShaderObject
{
public:

	float dt;
	float mass;
	float Jx, Jy, Jz;
	sf::Vector3f velocity;
	sf::Vector3f vecX, vecY, vecZ;
	sf::Vector3f w;
	
	Satellite(){}

	Satellite(sf::Vector3f position, sf::Vector3f colour, sf::Vector3f velocity, sf::Vector3f w, float Jx, float Jy, float Jz, float mass)
		:ShaderObject(position, colour) {

		this->Jx = Jx;
		this->Jy = Jy;
		this->Jz = Jz;
		this->velocity = velocity;
		this->w = w;
		this->mass = mass;
		dt = 0.01;
		vecX = sf::Vector3f(1, 0, 0);
		vecY = sf::Vector3f(0, 1, 0);
		vecZ = sf::Vector3f(0, 0, 1);

	}

	void dynamic(Planet planet) {

		sf::Vector3f vecSatToPlanet = VectorMath::normalize(position - planet.position);
		float satRadius = VectorMath::lenght(position - planet.position);
		sf::Vector3f acceleration = (-vecSatToPlanet * planet.mu * mass / (satRadius * satRadius)) / mass;
		velocity += acceleration * dt;
		position += velocity * dt;

		sf::Vector3f eps = sf::Vector3f(0, 0, 0);
		w += eps * dt;
		float pitch = w.y * dt;
		float yaw = w.z * dt;
		float roll = w.x * dt;

		rotate(pitch, yaw, roll);

	}


	void rotate(float pitch, float yaw, float roll) {

		double x1 = vecX.x;
		double x2 = vecX.y;
		double x3 = vecX.z;

		double y1 = vecY.x;
		double y2 = vecY.y;
		double y3 = vecY.z;

		double z1 = vecZ.x;
		double z2 = vecZ.y;
		double z3 = vecZ.z;


		double su = sin(yaw / 2);
		double sv = sin(pitch / 2);
		double sw = sin(roll / 2);

		double cu = cos(yaw / 2);
		double cv = cos(pitch / 2);
		double cw = cos(roll / 2);

		double q0 = -cw * (y1 * sv * z1 * su - cu * cv + y2 * sv * z2 * su + y3 * sv * z3 * su) - x1 * sw * (cu * y1 * sv + cv * z1 * su - y2 * sv * z3 * su + y3 * sv * z2 * su);
		double q1 = cw * (cu * y1 * sv + cv * z1 * su - y2 * sv * z3 * su + y3 * sv * z2 * su) - x1 * sw * (y1 * sv * z1 * su - cu * cv + y2 * sv * z2 * su + y3 * sv * z3 * su) - x2 * sw * (cu * y3 * sv + cv * z3 * su - y1 * sv * z2 * su + y2 * sv * z1 * su) + x3 * sw * (cu * y2 * sv + cv * z2 * su + y1 * sv * z3 * su - y3 * sv * z1 * su);
		double q2 = cw * (cu * y2 * sv + cv * z2 * su + y1 * sv * z3 * su - y3 * sv * z1 * su) + x1 * sw * (cu * y3 * sv + cv * z3 * su - y1 * sv * z2 * su + y2 * sv * z1 * su) - x2 * sw * (y1 * sv * z1 * su - cu * cv + y2 * sv * z2 * su + y3 * sv * z3 * su) - x3 * sw * (cu * y1 * sv + cv * z1 * su - y2 * sv * z3 * su + y3 * sv * z2 * su);
		double q3 = cw * (cu * y3 * sv + cv * z3 * su - y1 * sv * z2 * su + y2 * sv * z1 * su) - x1 * sw * (cu * y2 * sv + cv * z2 * su + y1 * sv * z3 * su - y3 * sv * z1 * su) + x2 * sw * (cu * y1 * sv + cv * z1 * su - y2 * sv * z3 * su + y3 * sv * z2 * su) - x3 * sw * (y1 * sv * z1 * su - cu * cv + y2 * sv * z2 * su + y3 * sv * z3 * su);

		vecX = rotateVec(q0, q1, q2, q3, vecX);
		vecY = rotateVec(q0, q1, q2, q3, vecY);
		vecZ = rotateVec(q0, q1, q2, q3, vecZ);

	}

	sf::Vector3f rotateVec(float q0, float q1, float q2, float q3, sf::Vector3f vec) {
		sf::Vector3f v = vec;
		sf::Vector3f t;
		t.x = 2 * (q2 * v.z - q3 * v.y);
		t.y = 2 * (q3 * v.x - q1 * v.z);
		t.z = 2 * (q1 * v.y - q2 * v.x);

		sf::Vector3f qt;
		qt.x = q2 * t.z - q3 * t.y;
		qt.y = q3 * t.x - q1 * t.z;
		qt.z = q1 * t.y - q2 * t.x;

		return v + (float)q0 * t + qt;
	}




};

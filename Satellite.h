#pragma once

#include "ShaderObject.h"
#include "Planet.h"
#include "VectorMath.h"
#include "Trajectory.h"


class Satellite: public ShaderObject
{
public:

	float dt;
	float t = 0;
	float mass;
	float Jx, Jy, Jz;
	float inertiaK;
	sf::Vector3f velocity;
	sf::Vector3f vecX, vecY, vecZ;
	sf::Vector3f w;
	Trajectory trajectory;
	
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
		inertiaK = 1;

	}

	void setOrbit(Planet planet, float a,float e, float i, float Omega,float w, float nu) {

		float rad = a * (1 - e * e) / (1 + e * cos(nu));
		float rp = a * (1 - e);
		float ra = a * (1 + e);
		float vp = sqrt(2 * planet.mu * ra / ((ra + rp) * rp));
		sf::Vector3f r;
		r.x = rad * (cos(w + nu) * cos(Omega) - sin(w + nu) * cos(i) * sin(Omega));
		r.y = rad * (cos(w + nu) * sin(Omega) + sin(w + nu) * cos(i) * cos(Omega));
		r.z = -rad * sin(w + nu) * sin(i);

		float vel = sqrt(2 * planet.mu * (1 / rad - 1 / rp) + vp * vp);
		float phi = asin(rp * vp / (rad * vel));

		sf::Vector3f inOrbitVec = sf::Vector3f(cos(Omega), sin(Omega), 0);
		sf::Vector3f perVec;
		perVec.x = rp * (cos(w) * cos(Omega) - sin(w) * cos(i) * sin(Omega));
		perVec.y = rp * (cos(w) * sin(Omega) + sin(w) * cos(i) * cos(Omega));
		perVec.z = -rp * sin(w) * sin(i);

		sf::Vector3f inOrbitVec2;
		if (w == 0) {
			float rad2 = a * (1 - e * e) / (1 + e * cos(0.5));
			inOrbitVec2.x = rad * (cos(0.5) * cos(Omega) - sin(0.5) * cos(i) * sin(Omega));
			inOrbitVec2.y = rad * (cos(0.5) * sin(Omega) + sin(0.5) * cos(i) * cos(Omega));
			inOrbitVec2.z = -rad * sin(0.5) * sin(i);
		}
		else
		{
			inOrbitVec2 = perVec;
		}
		sf::Vector3f centrVec = -VectorMath::normalize(perVec) * a * e;
		sf::Vector3f orbitN = VectorMath::normalize(VectorMath::cross(inOrbitVec, inOrbitVec2));
		sf::Vector3f ellipseSideVec = VectorMath::normalize(VectorMath::cross(orbitN, perVec)) * a * sqrt(1 - e * e);


		sf::Vector3f v = rotateAroundVec(orbitN, -VectorMath::normalize(r), phi) * vel;

		this->velocity = v;
		this->position = r + planet.position;

		trajectory = Trajectory(perVec-centrVec,ellipseSideVec,centrVec+planet.position);

	}

	void dynamic(Planet planet ,sf::Vector3f force, sf::Vector3f moment) {

		sf::Vector3f vecSatToPlanet = VectorMath::normalize(position - planet.position);
		float satRadius = VectorMath::lenght(position - planet.position);
		sf::Vector3f acceleration = (-vecSatToPlanet * planet.mu * mass / (satRadius * satRadius))+force / mass;
		velocity += acceleration * dt;
		position += velocity * dt;

		sf::Vector3f eps = sf::Vector3f(moment.x/Jx, moment.y / Jy, moment.z / Jz)/inertiaK;
		w += eps * dt;

		sf::Vector3f localW = toLocaleCS(w);
		float pitch = localW.y * dt;
		float yaw = localW.z * dt;
		float roll = localW.x * dt;

		rotate(pitch, yaw, roll);

		t += dt;
	}

	void dynamic(Planet planet) {
		dynamic(planet, sf::Vector3f(0, 0, 0), sf::Vector3f(0, 0, 0));
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

	sf::Vector3f rotateAroundVec(sf::Vector3f axis, sf::Vector3f vec, float angle) {

		double x1 = axis.x;
		double x2 = axis.y;
		double x3 = axis.z;

		double sw = sin(angle / 2);
		double cw = cos(angle / 2);

		double q0 = cw;
		double q1 = x1 * sw;
		double q2 = x2 * sw;
		double q3 = x3 * sw;

		return rotateVec(q0, q1, q2, q3, vec);
	}

	sf::Vector3f toLocaleCS(sf::Vector3f vec) {
		sf::Vector3f localV;
		localV.x = VectorMath::dot(vec, vecX);
		localV.y = VectorMath::dot(vec, vecY);
		localV.z = VectorMath::dot(vec, vecZ);
		return localV;
	}

	sf::Vector3f toGlobalCS(sf::Vector3f vec) {

		float det = vecX.x * vecY.y * vecZ.z - vecX.x * vecY.z * vecZ.y - vecX.y * vecY.x * vecZ.z + vecX.y * vecY.z * vecZ.x + vecX.z * vecY.x * vecZ.y - vecX.z * vecY.y * vecZ.x;

		sf::Vector3f localV;
		localV.x = vec.x * (vecY.y * vecZ.z - vecY.z * vecZ.y) - vec.y * (vecX.y * vecZ.z - vecX.z * vecZ.y) + vec.z * (vecX.y * vecY.z - vecX.z * vecY.y);
		localV.y = -vec.x * (vecY.x * vecZ.z - vecY.z * vecZ.x) + vec.y * (vecX.x * vecZ.z - vecX.z * vecZ.x) - vec.z * (vecX.x * vecY.z - vecX.z * vecY.x);
		localV.z = vec.x * (vecY.x * vecZ.y - vecY.y * vecZ.x) - vec.y * (vecX.x * vecZ.y - vecX.y * vecZ.x) + vec.z * (vecX.x * vecY.y - vecX.y * vecY.x);
		return localV/det;
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

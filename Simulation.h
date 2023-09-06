#pragma once

#include "Planet.h"
#include "Satellite.h"

class Simulation
{
public:
	
	Satellite satellite;
	Planet planet;

	Simulation() {
		this->planet = Planet(sf::Vector3f(0, 0, 0), sf::Vector3f(0,5,5), 100, 1);
		this->satellite = Satellite(sf::Vector3f(planet.radius+10, 0, 0) + planet.position, sf::Vector3f(255, 0, 0), sf::Vector3f(0, 0, 10.488),
			sf::Vector3f(0,1,1), 1, 1, 1, 1);

	}

	void draw(sf::Shader& shader) {

		shader.setUniform("u_planetPos", planet.position);
		shader.setUniform("u_planetCol", planet.colour);
		shader.setUniform("u_planetRad", planet.radius);

		shader.setUniform("u_satPos", satellite.position);
		shader.setUniform("u_satCol", satellite.colour);
		shader.setUniform("u_satVecX", satellite.vecX);
		shader.setUniform("u_satVecY", satellite.vecY);
		shader.setUniform("u_satVecZ", satellite.vecZ);

	}

	void runSimulation() {

		satellite.dynamic(planet);

	}




};


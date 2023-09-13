#pragma once
#include <SFML/Graphics.hpp>

class Trajectory
{
public:
	sf::Vector3f rp, rb, rc;

	Trajectory() {}

	Trajectory(sf::Vector3f rp, sf::Vector3f rb, sf::Vector3f rc) {
		this->rb = rb;
		this->rp = rp;
		this->rc = rc;
	}

	void draw(sf::Shader &shader){
		shader.setUniform("u_elU", rp);
		shader.setUniform("u_elV", rb);
		shader.setUniform("u_elC", rc);
	}
};


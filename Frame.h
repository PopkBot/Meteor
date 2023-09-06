#pragma once
#include <SFML/Graphics.hpp>


class Frame
{
public:
	int w, h;
	int mouseX;
	int mouseY;
	float mouseSensitivity;
	float speed;
	bool mouseHidden;
	bool povTower;
	bool keyFlag[6] = { false, false, false, false, false, false };
	double r = 10;
	float dirMoveX;
	float dirMoveY;
	float dirMoveZ;

	sf::Vector3f pos;
	sf::Clock clock;
	int framesStill;

	sf::RenderWindow window;
	sf::RenderTexture empityTexture;
	sf::Sprite empitySprite;
	sf::Shader shader;

	Frame() {
		initialize();
	}

	void initialize() {

		//r = 10;
		//w = 1080;
		//h = 1080;
		//mouseX = (int)(w / 2);
		//mouseY = (int)(h / 2);
		//mouseSensitivity = 3.0f;
		//speed = 0.1f;
		//mouseHidden = false;
		//povTower = true;

		//dirMoveX = 0;
		//dirMoveY = 0;
		//dirMoveZ = 0;

		//pos = sf::Vector3f(-5.0f, 0.0f, 0.0f);
		//framesStill = 1;

		//sf::RenderWindow window(sf::VideoMode(h, w), "Tracing", sf::Style::Titlebar | sf::Style::Close); //Titlebar | sf::Style::Close
		//window.setFramerateLimit(60);
		//window.setPosition(sf::Vector2i(830, 0));

		//empityTexture.create(h, w);
		//empitySprite = sf::Sprite(empityTexture.getTexture());
		//
		//shader.loadFromFile("Shader.frag", sf::Shader::Fragment);
		//shader.setUniform("u_resolution", sf::Vector2f(h, w));
	}



};


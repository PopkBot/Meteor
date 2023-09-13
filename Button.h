#pragma once
#include <SFML/Graphics.hpp>

class Button
{
public:
	sf::Text text;
	sf::Font font;
	sf::RectangleShape backGround;
	float sizeX;
	float sizeY;

	Button(){
	}

	void initialize(sf::Vector2f position,std::wstring str) {


		font.loadFromFile("cour.ttf");
		text.setFont(font);
		text.setFillColor(sf::Color::Black);
		text.setString(str);
		text.setCharacterSize(20);
		text.setPosition(position);
		text.setStyle(sf::Text::Bold);

		this->sizeX = str.length() * 15;
		this->sizeY = 30;

		backGround.setFillColor(sf::Color::Color(255, 165, 0, 255));
		backGround.setPosition(text.getPosition() - sf::Vector2f(5, 0));
		backGround.setSize(sf::Vector2f(sizeX, sizeY));

	}

	void draw(sf::RenderWindow& window) {
		window.draw(backGround);
		window.draw(text);
	}

	bool isPressed(sf::Vector2f mousePos) {
		sf::Vector2f relPos = mousePos - text.getPosition();
		if (relPos.x >= 0 && relPos.x <= sizeX && relPos.y >= 0 && relPos.y <= sizeY) {
			return true;
		}
		return false;
	}

};


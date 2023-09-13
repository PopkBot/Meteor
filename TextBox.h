#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

class TextBox
{
public:

	const int DELETE_KEY = 8;
	const int ENTRY_KEY = 13;
	const int ESCAPE_KEY = 27;

	sf::Text text;
	std::wstring inputText;
	std::wstring defaultInput;
	std::wstring staticText;
	sf::RectangleShape backGround;
	sf::Font font;
	float sizeX;
	float sizeY;
	int limit;


	bool isSelected = false;
	
	TextBox(){}

	void initialize(sf::Vector2f pos, std::wstring initText, int limit) {
		initialize(pos, initText, limit, L"");
	}

	void initialize(sf::Vector2f pos, std::wstring initText, int limit, std::wstring inputString) {
		staticText = initText;
		inputText = inputString;
		defaultInput = inputString;

		font.loadFromFile("cour.ttf");
		text.setFont(font);
		if (limit > 0) {
			text.setFillColor(sf::Color::Black);
		}
		else {
			text.setFillColor(sf::Color::Color(255, 165, 0, 255));
		}
		text.setString(staticText + inputText);
		text.setCharacterSize(20);
		text.setPosition(pos);
		text.setStyle(sf::Text::Bold);

		this->limit = limit;
		this->sizeX = staticText.length() *15 + limit *15;
		this->sizeY = 30;

		backGround.setFillColor(sf::Color::Color(255, 165, 0, 255));
		backGround.setPosition(text.getPosition() - sf::Vector2f(5, 0));
		backGround.setSize(sf::Vector2f(sizeX, sizeY));
	}

	void draw(sf::RenderWindow& window) {


		if (isSelected) {
			text.setString(staticText + L" " + inputText+L"_");
		}
		else {
			text.setString(staticText + L" " + inputText);
		}
		if (limit > 0) {
			window.draw(backGround);
		}
		window.draw(text);
	}

	void inputLogic(int charInput) {
		if (charInput != DELETE_KEY && charInput != ENTRY_KEY 
			&& charInput != ESCAPE_KEY && inputText.length()<limit) {
			inputText += static_cast<wchar_t>(charInput);
		}
		else if (charInput == DELETE_KEY && inputText.size()>0) {
			inputText = inputText.substr(0, inputText.size()-1);
		}
		else if (charInput == DELETE_KEY && inputText.size() == 0) {
			inputText += defaultInput;
		}
	}

	void typedOn(sf::Event input) {
		if (isSelected) {
			int charInput = input.text.unicode;
			inputLogic(charInput);
		}
	}

	bool setSelect(sf::Vector2f mousePos) {
		sf::Vector2f relPos = mousePos - text.getPosition();
		if (relPos.x >= 0 && relPos.x <= sizeX && relPos.y >= 0 && relPos.y <= sizeY) {
			isSelected = true;
			return true;
		}
		isSelected = false;
		return false;
	}


};


#pragma once

#include "SFML/Graphics.hpp"
#include "FlyWheel.h"
#include <string>

class StatusScreen
{
public:

	static constexpr float RAD_TO_GRAD = 57.2958;

	sf::Font font;
	sf::Text text;
	sf::Vector2f ancerPoint;

	std::vector<sf::Text> fwStatus;
	std::vector<sf::Text> wStatus;
	std::vector<sf::Text> axisStatus;
	sf::Text stateCode;
	sf::Text dt;
	sf::Text instructions;

	StatusScreen() {

		ancerPoint = sf::Vector2f(15, 30);

		font.loadFromFile("cour.ttf");
		text.setFont(font);
		text.setFillColor(sf::Color::Yellow);
		text.setString("TEST");
		text.setCharacterSize(20);
		text.setPosition(ancerPoint);
		text.setStyle(sf::Text::Bold);

		setUpFwStatus();
		setUpWStatus();
		setUpStateCode();
		setUpAxisStatus();
		setUpDt();
		setUpInstructions();

	}

	void draw(sf::RenderWindow& window, sf::Shader &shader) {


		shader.setUniform("screen1_dim", sf::Vector2f(400, 250));
		drawFwStatus(window);
		drawWStatus(window);
		drawStateCode(window);
		drawAxisStatus(window);
		drawDt(window);
		drawInstructions(window);

	}

	void setUpInstructions() {
			instructions.setFont(font);
			instructions.setFillColor(sf::Color::Color(255, 165, 0, 255));
			instructions.setString(L"Управлять камерой \"M\"; переключить камеру на Землю/аппарат \"N\"; ускорить, замедлить \"[\",\"]\"; выход \"Esc\"");
			instructions.setCharacterSize(15);
			instructions.setPosition(ancerPoint + sf::Vector2f(10, 950));
			instructions.setStyle(sf::Text::Bold);
	}

	void setUpFwStatus() {
		sf::Text text;
		for (int i = 0; i < 4; i++) {
			
			text.setFont(font);
			text.setFillColor(sf::Color::Color(255, 165, 0, 255));
			text.setString(L"ДМ " + std::to_wstring(i));
			text.setCharacterSize(20);
			text.setPosition(ancerPoint + sf::Vector2f(10, 10 + i * 25));
			text.setStyle(sf::Text::Bold);
			fwStatus.push_back(text);

		}
	}

	void setUpAxisStatus() {
		sf::Text text;
		for (int i = 0; i < 3; i++) {

			text.setFont(font);
			text.setFillColor(sf::Color::Color(255, 165, 0, 255));
			
			text.setCharacterSize(20);
			text.setPosition(ancerPoint + sf::Vector2f(320, 10 + (i) * 25));
			text.setStyle(sf::Text::Bold);
			axisStatus.push_back(text);

		}

		axisStatus[0].setString("X");
		axisStatus[1].setString("Y");
		axisStatus[2].setString("Z");
	}

	void setUpStateCode() {
		stateCode.setFont(font);
		stateCode.setFillColor(sf::Color::Color(255, 165, 0, 255));
		stateCode.setString(L"Режим: ");
		stateCode.setCharacterSize(20);
		stateCode.setPosition(ancerPoint + sf::Vector2f(10, 10 + 7 * 25));
		stateCode.setStyle(sf::Text::Bold);
	}

	void setUpDt() {
		dt.setFont(font);
		dt.setFillColor(sf::Color::Color(255, 165, 0, 255));
		dt.setString(L"Время х");
		dt.setCharacterSize(20);
		dt.setPosition(ancerPoint + sf::Vector2f(600, 10));
		dt.setStyle(sf::Text::Bold);
	}

	void setUpWStatus() {
		sf::Text text;
		for (int i = 0; i < 3; i++) {

			text.setFont(font);
			text.setFillColor(sf::Color::Color(255, 165, 0, 255));
			text.setCharacterSize(20);
			text.setPosition(ancerPoint + sf::Vector2f(10, 10 + (i+4) * 25));
			text.setStyle(sf::Text::Bold);
			wStatus.push_back(text);

		}

		wStatus[0].setString("Wx");
		wStatus[1].setString("Wy");
		wStatus[2].setString("Wz");
	}

	void drawInstructions(sf::RenderWindow& window) {
		window.draw(instructions);
	}

	void drawFwStatus(sf::RenderWindow& window) {
		for (int i = 0; i < fwStatus.size(); i++) {
			window.draw(fwStatus[i]);
		}
	}

	void drawWStatus(sf::RenderWindow& window) {
		for (int i = 0; i < wStatus.size(); i++) {
			window.draw(wStatus[i]);
		}
	}

	void drawAxisStatus(sf::RenderWindow& window) {
		for (int i = 0; i < axisStatus.size(); i++) {
			window.draw(axisStatus[i]);
		}
	}

	void drawStateCode(sf::RenderWindow& window) {
		window.draw(stateCode);
	}

	void drawDt(sf::RenderWindow& window) {
		window.draw(dt);
	}

	void setFwStatus(FlyWheel fw[]) {
		for (int i = 0; i < fwStatus.size(); i++) {
			wchar_t buff[100];
			swprintf(buff, sizeof(buff), L"ДМ %1d %8.2f об/мин",i+1, fw[i].speed);
			fwStatus[i].setString(buff);
		}
	}

	void setDt(float dtf) {
		wchar_t buff[100];
		swprintf(buff, sizeof(buff), L"Время х%8.2f", dtf/0.01);
	  	dt.setString(buff);
	}

	void setWStatus(sf::Vector3f w) {
		
		wchar_t buff[100];
		swprintf(buff, sizeof(buff), L"Wx   %8.4f град/с", w.x * RAD_TO_GRAD);
		wStatus[0].setString(buff);

		swprintf(buff, sizeof(buff), L"Wy   %8.4f град/с", w.y * RAD_TO_GRAD);
		wStatus[1].setString(buff);

		swprintf(buff, sizeof(buff), L"Wz   %8.4f град/с", w.z * RAD_TO_GRAD);
		wStatus[2].setString(buff);
	}

	void setAxisStatus(sf::Vector3f angles) {

		wchar_t buff[100];
		if (abs(angles.x) > 1) {
			angles.x = angles.x / abs(angles.x);
		}
		if (abs(angles.y) > 1) {
			angles.y = angles.y / abs(angles.y);
		}
		if (abs(angles.z) > 1) {
			angles.z = angles.z / abs(angles.z);
		}
		swprintf(buff, sizeof(buff), L"X %8.4f град", acos(angles.x*0.99999999) * RAD_TO_GRAD);
		axisStatus[0].setString(buff);

		swprintf(buff, sizeof(buff), L"Y %8.4f град", acos(angles.y * 0.99999999) * RAD_TO_GRAD);
		axisStatus[1].setString(buff);

		swprintf(buff, sizeof(buff), L"Z %8.4f град", acos(angles.z * 0.99999999) * RAD_TO_GRAD);
		axisStatus[2].setString(buff);
	}

	void setStateCode(std::wstring code) {
		stateCode.setString(L"Режим: " + code);
	}
};


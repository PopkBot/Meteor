
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Frame.h"
#include "Simulation.h"

#include "TextBox.h"




void runFrame(Simulation & simulation) {

	
	sf::Vector2f mousePos;

	bool isTyping = false;

	float r = 10;
	float rk = 2;
	float trajectoryScale = 0.99999;
	int w = 1000, h = 1800;
	int mouseX = (int)(w / 4);
	int mouseY = (int)(h / 4);
	float mouseSensitivity = 2.0f;
	float speed = 0.1f;
	bool mouseHidden = false;
	bool povTower = true;
	bool keyFlag[6] = { false, false, false, false, false, false };

	float dirMoveX = 0;
	float dirMoveY = 0;
	float dirMoveZ = 0;

	sf::Vector3f pos = sf::Vector3f(-5.0f, 0.0f, 0.0f);
	sf::Clock clock;
	int framesStill = 1;


	sf::RenderWindow window(sf::VideoMode(h,w), "Meteor",sf::Style::Titlebar| sf::Style::Close | sf::Style::Resize); //Titlebar | sf::Style::Close
	
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(0, 0));

	sf::RenderTexture empityTexture;
	empityTexture.create(h, w);
	sf::Sprite empitySprite = sf::Sprite(empityTexture.getTexture());


	sf::Shader shader;
	shader.loadFromFile("shader\\MeteorShader.frag", sf::Shader::Fragment);
	shader.setUniform("u_resolution", sf::Vector2f(h, w));
	
	
	while (window.isOpen()) {

		sf::Event event;



		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Resized) {
				window.setSize(sf::Vector2u(window.getSize().y*1.8, window.getSize().y));
			}

			if (event.type == sf::Event::MouseButtonReleased && !mouseHidden) {
				isTyping = simulation.checkTextBoxesSelection(mousePos);
				int pressedButtonId = simulation.buttonPressed(mousePos);
				simulation.procedeButtonCommand(pressedButtonId);
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				r += (double)event.mouseWheelScroll.delta*rk;
			}
			

			if (event.type == sf::Event::Closed)
			{
				window.close();
				return;
			}
			if (event.type == sf::Event::MouseMoved)
			{

				mousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);

				if (mouseHidden)
				{
					int mx = event.mouseMove.x - w / 4;
					int my = event.mouseMove.y - h / 4;
					mouseX += mx;
					mouseY += my;
					sf::Mouse::setPosition(sf::Vector2i(w / 4, h / 4), window);
					if (mx != 0 || my != 0) framesStill = 1;
				}
			}
			if (event.type == sf::Event::KeyPressed && !isTyping) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				if (event.key.code == sf::Keyboard::M) {
					if (!mouseHidden) {
						framesStill = 1;
						window.setMouseCursorVisible(false);
						mouseHidden = true;
					}
					else {
						window.setMouseCursorVisible(true);
						mouseHidden = false;
					}
				}
				if (event.key.code == sf::Keyboard::N) {
					
					if (povTower) {
						r = simulation.planet.radius * 2;
						rk = 500;
						trajectoryScale = 0.99;
					}
					else
					{
						r = 10;
						rk = 2;
						trajectoryScale = 0.99999;
					}
					povTower = !povTower;
				}




				if (event.key.code == sf::Keyboard::LBracket) { 
					simulation.satellite.dt-=0.01;
					if (simulation.satellite.dt < 0) {
						simulation.satellite.dt = 0;
					}
				}
				if (event.key.code == sf::Keyboard::RBracket) { simulation.satellite.dt += 0.01; }

			}
			if (event.type == sf::Event::KeyReleased && !isTyping)
			{

			}
			else if (event.type == sf::Event::TextEntered && isTyping) {
				simulation.typedToTextBoses(event);
			}

		}
		float mx = ((float)mouseX / w - 0.5f) * mouseSensitivity;
		float my = ((float)mouseY / h - 0.5f) * mouseSensitivity;
		if (mouseHidden) {


			sf::Vector3f dir = sf::Vector3f(0.0f, 0.0f, 0.0f);
			sf::Vector3f dirTemp;


			dirTemp.z += -(r + dirMoveZ) * sin(my);
			dirTemp.x += -(r + dirMoveX) * cos(my) * cos(mx);
			dirTemp.y += -(r + dirMoveY) * cos(my) * sin(mx);
			if (!keyFlag[0]) {
				dir.x = dirTemp.x;
				dir.y = dirTemp.y;
				dir.z = dirTemp.z;
			}
			else {
				dir.x = dirTemp.x * cos(mx) - dirTemp.y * sin(mx);
				dir.y = dirTemp.x * sin(mx) + dirTemp.y * cos(mx);
				dir.z = dirTemp.z;

			}
			pos = dir;
		}

		simulation.runSimulation();
		

		if (povTower) shader.setUniform("u_pos", pos + simulation.satellite.position);
		else shader.setUniform("u_pos", pos);

		shader.setUniform("u_trajectory_scale", trajectoryScale);
		shader.setUniform("u_mouse", sf::Vector2f(mx, my));




		window.draw(empitySprite, &shader);
		simulation.draw(shader, window);

		window.display();

		
		
	}
}

int main()
{
	
	Simulation simulation;
	runFrame(simulation);

}



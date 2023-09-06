
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Frame.h"
#include "Simulation.h"



void runFrame(Simulation & simulation) {
	
	float r = 10;
	int w = 1080, h = 1080;
	int mouseX = (int)(w / 2);
	int mouseY = (int)(h / 2);
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


	sf::RenderWindow window(sf::VideoMode(h, w), "Tracing", sf::Style::Titlebar | sf::Style::Close); //Titlebar | sf::Style::Close
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(830, 0));

	sf::RenderTexture empityTexture;
	empityTexture.create(h, w);
	sf::Sprite empitySprite = sf::Sprite(empityTexture.getTexture());


	sf::Shader shader;
	shader.loadFromFile("Shader.frag", sf::Shader::Fragment);
	shader.setUniform("u_resolution", sf::Vector2f(h, w));
	
	
	while (window.isOpen()) {
		//std::chrono::steady_clock::time_point begin;
		//std::chrono::steady_clock::time_point end;

		sf::Event event;



		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::MouseWheelScrolled) {
				r += (double)event.mouseWheelScroll.delta;
			}

			if (event.type == sf::Event::Closed)
			{


				window.close();
			}
			if (event.type == sf::Event::MouseMoved)
			{

				if (mouseHidden)
				{
					int mx = event.mouseMove.x - w / 2;
					int my = event.mouseMove.y - h / 2;
					mouseX += mx;
					mouseY += my;
					sf::Mouse::setPosition(sf::Vector2i(w / 2, h / 2), window);
					if (mx != 0 || my != 0) framesStill = 1;
				}
			}
			if (event.type == sf::Event::KeyPressed) {
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
					povTower = !povTower;
				}




				//if (event.key.code == sf::Keyboard::C) { dirMoveZ += -1; }// rocket.betta = 0;rocket.alpha = 0;rocket.gamma = 0; }
				//if (event.key.code == sf::Keyboard::W) { dirMoveX += 1; } //rocket.betta += 0.1f; }
			//	if (event.key.code == sf::Keyboard::S) { dirMoveX += -1; } //rocket.betta -= 0.1f; }
			//	if (event.key.code == sf::Keyboard::A) { dirMoveY += -1; } //rocket.gamma += 0.1f; }
			//	if (event.key.code == sf::Keyboard::D) { dirMoveY += 1; } //rocket.gamma -= 0.1f;}
				//if (event.key.code == sf::Keyboard::Q) { rocket.bTurnX = 1; } //rocket.alpha += 0.1f; }
				//if (event.key.code == sf::Keyboard::E) { rocket.bTurnX = -1; } //rocket.alpha -= 0.1f; }
				//printf("bthrust =%d\n", rocket.bThrust);

			}
			else if (event.type == sf::Event::KeyReleased)
			{

				//if (event.key.code == sf::Keyboard::Space)dirMoveZ = 0;
				//if (event.key.code == sf::Keyboard::C)dirMoveZ = 0; 
				//if (event.key.code == sf::Keyboard::W)dirMoveX = 0;
				//if (event.key.code == sf::Keyboard::S)dirMoveX = 0;
				//if (event.key.code == sf::Keyboard::A)dirMoveY = 0;
				//if (event.key.code == sf::Keyboard::D)dirMoveY = 0;
				//if (event.key.code == sf::Keyboard::Q)rocket.bTurnX = 0;
				//if (event.key.code == sf::Keyboard::E)rocket.bTurnX = 0;


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
		simulation.draw(shader);

		if (povTower) shader.setUniform("u_pos", pos + simulation.satellite.position);
		else shader.setUniform("u_pos", pos);
		shader.setUniform("u_mouse", sf::Vector2f(mx, my));
		window.draw(empitySprite, &shader);
		window.display();

	}
}

int main()
{

	
	Simulation simulation;
	runFrame(simulation);


}



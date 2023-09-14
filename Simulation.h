#pragma once

#include "Planet.h"
#include "MeteorSat.h"
#include "StatusScreen.h"
#include "CoordSystem.h"
#include "TextBox.h"
#include "Button.h"


class Simulation
{
public:
	
	static const int INPUT_BOXES_COUNT = 18;
	static const int STATIC_BOXES_COUNT = 5;
	static const int BUTTONS_COUNT = 8;

	static const int BUTTON_STATE_START = 0;
	static const int BUTTON_TURN_X_START = 1;
	static const int BUTTON_TURN_Y_START = 2;
	static const int BUTTON_TURN_Z_START = 3;
	static const int BUTTON_ORBIT_SET = 4;
	static const int BUTTON_ATTITUDE_SET = 5;
	static const int BUTTON_W_SET = 6;
	static const int BUTTON_INERTIA_SET = 7;

	StatusScreen statusScreen;
	MeteorSat satellite;
	Planet planet;
	ShaderObject sun;
	TextBox inputTextBoxes[INPUT_BOXES_COUNT];
	TextBox staticTextBoxes[STATIC_BOXES_COUNT];
	Button buttons[BUTTONS_COUNT];

	Simulation() {
		this->planet = Planet(sf::Vector3f(0, 0, 0), sf::Vector3f(0, 0.7, 0.7), 6400, 398600);
		this->satellite = MeteorSat(planet.position, sf::Vector3f(0.5,0.5,0.5), sf::Vector3f(0, 0, 0),
			sf::Vector3f(0, 0, 0), 6770,13139,8452, 1);
		this->sun = ShaderObject(sf::Vector3f(0,-1,0),sf::Vector3f(0,0,0));

		float xc = 1300;

		staticTextBoxes[0].initialize(sf::Vector2f(xc, 10), L"Режим", 0);
		inputTextBoxes[0].initialize(sf::Vector2f(xc, 45), L"C",14);
		inputTextBoxes[1].initialize(sf::Vector2f(xc+210, 45), L"-> До",14);
		
		staticTextBoxes[1].initialize(sf::Vector2f(xc, 105), L"Поворот вокруг", 0);
		inputTextBoxes[2].initialize(sf::Vector2f(xc, 140), L"X [град]", 8,L"0");
		inputTextBoxes[3].initialize(sf::Vector2f(xc, 175), L"Y [град]", 8, L"0");
		inputTextBoxes[4].initialize(sf::Vector2f(xc, 210), L"Z [град]", 8, L"0");

		staticTextBoxes[2].initialize(sf::Vector2f(xc, 280), L"Орбита", 0);
		inputTextBoxes[5].initialize(sf::Vector2f(xc, 315), L"Большая полуось      [км]", 10, L"7215");
		inputTextBoxes[6].initialize(sf::Vector2f(xc, 350), L"Эксцентриситет           ", 10, L"0.00124");
		inputTextBoxes[7].initialize(sf::Vector2f(xc, 385), L"Наклонение         [град]", 10, L"98.76");
		inputTextBoxes[8].initialize(sf::Vector2f(xc, 420), L"Долгота восх. угла [град]", 10, L"0");
		inputTextBoxes[9].initialize(sf::Vector2f(xc, 455), L"Аргумент перицентра[град]", 10, L"0");
		inputTextBoxes[10].initialize(sf::Vector2f(xc, 490),L"Истинная аномалия  [град]", 10, L"0");

		staticTextBoxes[3].initialize(sf::Vector2f(xc, 560), L"Ориентация", 0);
		inputTextBoxes[11].initialize(sf::Vector2f(xc, 595), L"Рысканье [град]", 8, L"0");
		inputTextBoxes[12].initialize(sf::Vector2f(xc, 630), L"Тангаж   [град]", 8, L"0");
		inputTextBoxes[13].initialize(sf::Vector2f(xc, 665), L"Крен     [град]", 8, L"0");

		staticTextBoxes[4].initialize(sf::Vector2f(xc, 735), L"Wo", 0);
		inputTextBoxes[14].initialize(sf::Vector2f(xc, 770), L"Wox [град/с]", 8, L"0");
		inputTextBoxes[15].initialize(sf::Vector2f(xc, 805), L"Woy [град/с]", 8, L"0");
		inputTextBoxes[16].initialize(sf::Vector2f(xc, 840), L"Woz [град/с]", 8, L"0");

		inputTextBoxes[17].initialize(sf::Vector2f(xc, 910), L"Множитель инерции", 6, L"1");


		buttons[0].initialize(sf::Vector2f(xc + 210, 10), L"Начать");
		buttons[1].initialize(sf::Vector2f(xc + 310, 140), L"Начать");
		buttons[2].initialize(sf::Vector2f(xc + 310, 175), L"Начать");
		buttons[3].initialize(sf::Vector2f(xc + 310, 210), L"Начать");
		buttons[4].initialize(sf::Vector2f(xc + 210, 280), L"Задать");
		buttons[5].initialize(sf::Vector2f(xc + 210, 560), L"Задать");
		buttons[6].initialize(sf::Vector2f(xc + 210, 735), L"Задать");
		buttons[7].initialize(sf::Vector2f(xc + 350, 910), L"Задать");

		commenceAttitudeSet();
		commenceWSet();
		commenceOrbitSet();

	}

	void draw(sf::Shader& shader, sf::RenderWindow& window) {


		shader.setUniform("u_planetPos", planet.position);
		shader.setUniform("u_planetCol", planet.colour);
		shader.setUniform("u_planetRad", planet.radius);

		shader.setUniform("u_satPos", satellite.position);
		shader.setUniform("u_satCol", satellite.colour);
		shader.setUniform("u_satVecX", satellite.vecX);
		shader.setUniform("u_satVecY", satellite.vecY);
		shader.setUniform("u_satVecZ", satellite.vecZ);

		shader.setUniform("u_sunVec", sun.position);

		shader.setUniform("u_targetW", VectorMath::normalize( satellite.state.targetW));
		shader.setUniform("u_W", VectorMath::normalize(satellite.w));
		shader.setUniform("u_moment", VectorMath::normalize(satellite.state.moment));

		shader.setUniform("isDrawOrbitalCS", 1.0f);

		CoordSystem orbitalCS = getOrbitalCS();
		shader.setUniform("orbX", orbitalCS.vecX);
		shader.setUniform("orbY", orbitalCS.vecY);
		shader.setUniform("orbZ", orbitalCS.vecZ);

		satellite.trajectory.draw(shader);

		statusScreen.draw(window, shader);
		
		for (TextBox textBox : staticTextBoxes) {
			textBox.draw(window);
		}
		for (TextBox textBox : inputTextBoxes) {
			textBox.draw(window);
		}
		for (Button button : buttons) {
			button.draw(window);
		}

		
		
	}

	void runSimulation() {

		satellite.proceedState(planet, sun);
		satellite.dynamic(planet, satellite.state.force, satellite.state.moment);

		statusScreen.setFwStatus(satellite.fw);
		statusScreen.setWStatus(satellite.toLocaleCS(satellite.w));
		statusScreen.setStateCode(satellite.getStateName());
		statusScreen.setAxisStatus(getAnglesOrbitalToLocal());
		statusScreen.setDt(satellite.dt);
		
		printf("state %d\t|velocity| = %-10.4f\t|r| = %-10.4f\tmoment = %-10.4f \n",
			satellite.state.procedureCode,
			VectorMath::lenght(satellite.velocity),
			VectorMath::lenght(satellite.position - planet.position),
			VectorMath::lenght(satellite.state.moment));

	}

	sf::Vector3f getAnglesOrbitalToLocal() {

		CoordSystem orbitalCS = getOrbitalCS();
		sf::Vector3f angles;
		angles.x = VectorMath::dot(satellite.vecX, orbitalCS.vecX);
		angles.y = VectorMath::dot(satellite.vecY, orbitalCS.vecY);
		angles.z = VectorMath::dot(satellite.vecZ, orbitalCS.vecZ);



		return angles;
	}

	CoordSystem getOrbitalCS() {
		sf::Vector3f orbZ = VectorMath::normalize(satellite.position - planet.position);

		sf::Vector3f orbX = VectorMath::normalize(VectorMath::cross(VectorMath::cross(orbZ, satellite.velocity), orbZ));
		if (VectorMath::dot(VectorMath::cross(orbZ, satellite.velocity), orbZ) == 0) {
			orbX = VectorMath::normalize(satellite.velocity);
		}
		sf::Vector3f orbY = VectorMath::normalize(VectorMath::cross(orbZ, orbX));
		return CoordSystem(orbX, orbY, orbZ);
	}

	bool checkTextBoxesSelection(sf::Vector2f mousePos) {
		
		bool isSelected = false;
		for (int i = 0; i < INPUT_BOXES_COUNT; i++) {
			if (inputTextBoxes[i].setSelect(mousePos) && !isSelected) {
				isSelected = true;
			}
		}
		return isSelected;
	}

	void typedToTextBoses(sf::Event input) {

		for (int i = 0; i < INPUT_BOXES_COUNT; i++) {
			inputTextBoxes[i].typedOn(input);
		}
	}

	int buttonPressed(sf::Vector2f mousePos) {
		for (int i = 0; i < BUTTONS_COUNT; i++) {
			if (buttons[i].isPressed(mousePos)) {
				return i;
			}
		}
		return -1;
	}

	void procedeButtonCommand(int command) {
		try {
			switch (command)
			{
			case BUTTON_STATE_START: commenceStateProcedure(); break;
			case BUTTON_TURN_X_START: commenceAxisTurn(0); break;
			case BUTTON_TURN_Y_START: commenceAxisTurn(1); break;
			case BUTTON_TURN_Z_START: commenceAxisTurn(2); break;
			case BUTTON_ORBIT_SET: commenceOrbitSet(); break;
			case BUTTON_ATTITUDE_SET: commenceAttitudeSet(); break;
			case BUTTON_W_SET: commenceWSet(); break;
			case BUTTON_INERTIA_SET: commenceInertiaSet(); break;
			default:
				break;
			}
		}
		catch (const std::exception& ex) {

		}
	}

	void commenceStateProcedure() {
		int procedure1 = satellite.getStateCode(inputTextBoxes[0].inputText);
		int procedure2 = satellite.getStateCode(inputTextBoxes[1].inputText);
		if (procedure1 > procedure2) {
			return;
		}
		satellite.setStateSecuence(procedure1, procedure2);
		satellite.setState(1);
	}

	void commenceAxisTurn(int axis) { //0-x 1-y 2-z

		float angle = std::stof(inputTextBoxes[2 + axis].inputText);
		sf::Vector3f turn = sf::Vector3f(0,0,0);
		switch (axis)
		{
		case 0: turn.x = angle; break;
		case 1: turn.y = angle; break;
		case 2: turn.z = angle; break;
		default:
			break;
		}
		satellite.setStateTurn(turn);
	}

	void commenceOrbitSet() {

		float a = std::stof(inputTextBoxes[5].inputText);
		float e = std::stof(inputTextBoxes[6].inputText);
		float i = std::stof(inputTextBoxes[7].inputText) * satellite.GRAD_TO_RAD;
		float Omega = std::stof(inputTextBoxes[8].inputText) * satellite.GRAD_TO_RAD;
		float w = std::stof(inputTextBoxes[9].inputText) * satellite.GRAD_TO_RAD;
		float nu = std::stof(inputTextBoxes[10].inputText) * satellite.GRAD_TO_RAD;
		satellite.setOrbit(planet, a, e, i, Omega, w, nu);
	}

	void commenceAttitudeSet() {
		satellite.vecX = sf::Vector3f(1, 0, 0);
		satellite.vecY = sf::Vector3f(0, 1, 0);
		satellite.vecZ = sf::Vector3f(0, 0, 1);

		float yaw = std::stof(inputTextBoxes[11].inputText) * satellite.GRAD_TO_RAD;
		float pitch = std::stof(inputTextBoxes[12].inputText) * satellite.GRAD_TO_RAD;
		float roll = std::stof(inputTextBoxes[13].inputText) * satellite.GRAD_TO_RAD;

		satellite.rotate(pitch, yaw, roll);
	}

	void commenceWSet() {

		float wx = std::stof(inputTextBoxes[14].inputText) * satellite.GRAD_TO_RAD;
		float wy = std::stof(inputTextBoxes[15].inputText) * satellite.GRAD_TO_RAD;
		float wz = std::stof(inputTextBoxes[16].inputText) * satellite.GRAD_TO_RAD;

		sf::Vector3f w = sf::Vector3f(wx, wy, wz);
		satellite.w = w;
	}

	void commenceInertiaSet() {
		satellite.inertiaK = std::stof(inputTextBoxes[17].inputText);
	}
};


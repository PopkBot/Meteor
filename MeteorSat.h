#pragma once

#include "Satellite.h"
#include "FlyWheel.h"
#include "VectorMath.h"





class MeteorSat: public Satellite 
{
public:

	static constexpr float GRAD_TO_RAD = 0.0174533;

	static const int STABILIZE = 1;
	static const int TO_EARTH = 2;
	static const int TO_SUN = 3;
	static const int TWO_Z_S = 4;
	static const int TWO_Z_K = 5;
	static const int TURN = 6;


	std::wstring CODE_STABILIZE = L"Успокоение";
	std::wstring CODE_TO_EARTH = L"Поиск Земли";
	std::wstring CODE_TO_SUN = L"Поиск Солнца";
	std::wstring CODE_TWO_Z_S = L"2ЗС";
	std::wstring CODE_TWO_Z_K = L"2ЗК";
	std::wstring CODE_TURN = L"Поворот по осям";

	static constexpr float STABILITY_W = 0.001f; // rad/s
	static constexpr float TO_EARTH_W = 0.001f; // rad/s
	static constexpr float TO_SUN_W = 0.001f; // rad/s
	static constexpr float TURN_W = 0.001f; // rad/s


	struct State
	{
		sf::Vector3f targetW;
		sf::Vector3f force;
		sf::Vector3f moment;
		sf::Vector3f angle;
		sf::Vector3f targetVec;

		int procedureCode;

		int startState;
		int endState;

		State() {
			targetW = sf::Vector3f(0, 0, 0);
			force = sf::Vector3f(0, 0, 0);
			moment = sf::Vector3f(0, 0, 0);
			angle = sf::Vector3f(0, 0, 0);
			targetVec = sf::Vector3f(0, 0, 0);
			procedureCode = 0;
		}

		void zero() {
			targetW = sf::Vector3f(0, 0, 0);
			force = sf::Vector3f(0, 0, 0);
			moment = sf::Vector3f(0, 0, 0);
			angle = sf::Vector3f(0, 0, 0);
			targetVec = sf::Vector3f(0, 0, 0);
			procedureCode = 0;
		}
	};

	State state;
	FlyWheel fw[4];

	MeteorSat(){}

	MeteorSat(sf::Vector3f position, sf::Vector3f colour, sf::Vector3f velocity, sf::Vector3f w, float Jx, float Jy, float Jz, float mass) :
		Satellite(position, colour, velocity, w, Jx, Jy, Jz, mass) {

		fw[0] = FlyWheel(sf::Vector3f(-0.5792, 0.5792, -0.5736));
		fw[1] = FlyWheel(sf::Vector3f(-0.5792, -0.5792, -0.5736));
		fw[2] = FlyWheel(sf::Vector3f(-0.5792, -0.5792, 0.5736));
		fw[3] = FlyWheel(sf::Vector3f(-0.5792, 0.5792, 0.5736));


	}

	std::wstring getStateName(int stateCode) {
		switch (stateCode)
		{
		case STABILIZE: return CODE_STABILIZE;
		case TO_EARTH: return CODE_TO_EARTH;
		case TO_SUN: return CODE_TO_SUN;
		case TWO_Z_S: return CODE_TWO_Z_S;
		case TWO_Z_K: return CODE_TWO_Z_K;
		case TURN: return CODE_TURN;
		default:
			return L"Ожидание";
		}
	}

	int getStateCode(std::wstring stateCode) {
		if (stateCode == CODE_STABILIZE) {
			return STABILIZE;
		}
		if (stateCode == CODE_TO_EARTH) {
			return TO_EARTH;
		}
		if (stateCode == CODE_TO_SUN) {
			return TO_SUN;
		}
		if (stateCode == CODE_TWO_Z_S) {
			return TWO_Z_S;
		}
		if (stateCode == CODE_TWO_Z_K) {
			return TWO_Z_K;
		}
		return -1;
	}

	std::wstring getStateName() {
		return getStateName(state.procedureCode);
	}

	void setState(int stateCode) {
		switch (stateCode)
		{
		case STABILIZE: setStateStabilize(); break;
		case TO_EARTH: setStateToEarth(); break;
		case TO_SUN: setStateToSun(); break;
		case TWO_Z_S: setStateTwoZS(); break;
		case TWO_Z_K: setStateTwoZK(); break;
		default:
			break;
		}
	}

	void setStateSecuence(int start, int end) {
		state.startState = start;
		state.endState = end;
	}

	void nextState() {
		if (state.endState > state.procedureCode) {
			state.procedureCode++;
			setState(state.procedureCode);
		}
	}


	void proceedState(Planet planet, ShaderObject sun) {
		switch (state.procedureCode)
		{
		case STABILIZE: stabilize(); break;
		case TO_EARTH: toEarth(planet); break;
		case TO_SUN: toSun(sun,planet); break;
		case TWO_Z_S: twoZS(planet, sun); break;
		case TWO_Z_K: twoZK(planet, sun); break;
		case TURN: turn(); break;
		default:
			break;
		}
	}

	void stabilize() {

		if (VectorMath::lenght(w - state.targetW) < STABILITY_W) {
			nextState();
			return;
		}

		state.targetW = sf::Vector3f(0, 0, 0);

		float k[4];
		calculateFlyWheelComposition(k);
		calculateMoment(k);
		printf("w = %f\tmom = %f\t dot(mom,targw) = %f\n",
			VectorMath::lenght(w), VectorMath::lenght(state.moment),
			VectorMath::dot(VectorMath::normalize(state.moment), VectorMath::normalize(state.targetW)));
	}

	void toEarth(Planet planet) {
		
		float dot = VectorMath::dot(vecZ, VectorMath::normalize(position - planet.position));
		float a1=-1;
		float a2 = -1;
		if (dot > 0.9999 && VectorMath::lenght(w) < TO_EARTH_W) {
			nextState();
			return;
		}

		if (dot < 0.5) {
			state.targetW = -(vecX)*GRAD_TO_RAD*0.4f;
			
		}
		else {
			sf::Vector3f planetToSat = VectorMath::normalize(position - planet.position);
			sf::Vector3f wToTargetPos = VectorMath::normalize(VectorMath::cross(vecZ, planetToSat)) * 0.2f;
			sf::Vector3f wBraking = -w;
			a1 = powf(2, -(abs(dot)+0.0) * 5);
			a2 = powf(2, (abs(dot) - 1) * 3);
			state.targetW = VectorMath::normalize(wToTargetPos * a1 + wBraking * a2);
			
		}
		if (dot > 0.99999) {
			state.targetW = sf::Vector3f(0, 0, 0);
		}

		float k[4];
		calculateFlyWheelComposition(k);
		calculateMoment(k);
		printf("dot = %f\ttargW = %f\tw = %f\tmom = %f\t dot(mom,targw) = %f\ta1 = %f\ta2 = %f\n",
			dot, VectorMath::lenght(state.targetW), VectorMath::lenght(w),VectorMath::lenght(state.moment),
			VectorMath::dot(VectorMath::normalize(state.moment),VectorMath::normalize(state.targetW)),
			a1,a2);

		

	}

	void toSun(ShaderObject sun, Planet planet) {

		sf::Vector3f solarPlaneVec = VectorMath::normalize(VectorMath::cross(VectorMath::normalize(sun.position), VectorMath::normalize(position - planet.position)));
		float dot = VectorMath::dot(vecY, solarPlaneVec);
		float dotToSun = VectorMath::dot(vecY, sun.position);
		float a1 = -1;
		float a2 = -1;


		if (abs(dot) < 0.001 && dotToSun > 0 && VectorMath::lenght(w) < TO_SUN_W) {
			nextState();
			return;
		}
		if (dotToSun < 0) {
			state.targetW = vecZ * GRAD_TO_RAD * 0.4f;
		}
		else {
			sf::Vector3f wToTargetPos = VectorMath::normalize(VectorMath::cross(vecY, sun.position)) * 0.2f;
			if (abs(dot) > 0.01) {
				wToTargetPos = VectorMath::dot(vecZ, VectorMath::normalize(VectorMath::cross(vecY, sun.position))) * vecZ * 0.2f;
			}
			sf::Vector3f wBraking = -w * powf(abs(VectorMath::lenght(w)), 2);
			a1 = powf(2, (abs(dot) - 1) * 4);
			a2 = powf(2, -abs(dot) * 10);
			state.targetW = (wToTargetPos * a1 + wBraking * a2);
		}

		if (abs(dot) < 0.001 && dotToSun>0) {
			state.targetW = sf::Vector3f(0, 0, 0);
		}

		float k[4];
		calculateFlyWheelComposition(k);
		calculateMoment(k);
		printf("dotSun = %f\ttargW = %f\tw = %f\tmom = %f\t dot(mom,targw) = %f\ta1 = %f\ta2 = %f\n",
			dot, VectorMath::lenght(state.targetW), VectorMath::lenght(w), VectorMath::lenght(state.moment),
			VectorMath::dot(VectorMath::normalize(state.moment), VectorMath::normalize(state.targetW)),
			a1, a2);


	}

	void twoZS(Planet planet, ShaderObject sun) {

		float dotEarth = VectorMath::dot(vecZ, VectorMath::normalize(position - planet.position));
		sf::Vector3f solarPlaneVec = VectorMath::normalize(VectorMath::cross(VectorMath::normalize(sun.position), VectorMath::normalize(position - planet.position)));
		float dotToSun1 = VectorMath::dot(vecY, solarPlaneVec);
		float dotToSun2= VectorMath::dot(vecY, sun.position);

		if (!(dotEarth > 0.9999 && VectorMath::lenght(w) < TO_EARTH_W*10)) {
			toEarth(planet);
			return;
		}

		if (!(abs(dotToSun1) < 0.001 && dotToSun2>0 && VectorMath::lenght(w) < TO_SUN_W*10)) {
			toSun(sun, planet);
			return;
		}
		nextState();
		
	}

	void twoZK(Planet planet, ShaderObject sun) {
		float dotEarth = VectorMath::dot(vecZ, VectorMath::normalize(position - planet.position));
		sf::Vector3f solarPlaneVec = VectorMath::normalize(VectorMath::cross(VectorMath::normalize(sun.position), VectorMath::normalize(position - planet.position)));
		float dotToSun1 = VectorMath::dot(vecY, solarPlaneVec);
		float dotToSun2 = VectorMath::dot(vecY, sun.position);

		float dotToVel = VectorMath::dot(vecX, VectorMath::normalize(velocity));

		if (!(dotEarth > 0.99999)) {
			toEarth(planet);
			return;
		}

		if (dotToVel < 0.99999) {
			sf::Vector3f wToTargetPos = VectorMath::normalize(VectorMath::cross(vecX, velocity )) * 0.2f;
			sf::Vector3f wBraking = -w * powf(abs(VectorMath::lenght(w)), 2);
			float a1 = powf(2, -(abs(dotToVel) + 0.0) * 5);
			float a2 = powf(2, (abs(dotToVel) - 1) * 3);
			state.targetW = (wToTargetPos * a1 + wBraking * a2);
		}
		else {
			float targetAngSpeed = 1 / sqrt(pow(VectorMath::lenght(position - planet.position), 3) / planet.mu);
			state.targetW = VectorMath::normalize(VectorMath::cross(position - planet.position, velocity)) * targetAngSpeed;
		}

		
		float k[4];
		calculateFlyWheelComposition(k);
		calculateMoment(k);
		printf("dotVel = %f\ttargW = %f\tw = %f\n",
			dotToVel, VectorMath::lenght(state.targetW), VectorMath::lenght(w));
		
	}

	void turn() {

		float dot=0;

		if (state.angle.x != 0) {
			dot = VectorMath::dot(vecZ, state.targetVec);
			state.targetW = VectorMath::normalize(VectorMath::cross(vecZ, state.targetVec)) * 0.4f * GRAD_TO_RAD;
		}
		else if (state.angle.y != 0) {
			dot = VectorMath::dot(vecZ, state.targetVec);
			state.targetW = VectorMath::normalize(VectorMath::cross(vecZ, state.targetVec)) * 0.4f * GRAD_TO_RAD;
		}
		else if (state.angle.z != 0) {
			dot = VectorMath::dot(vecX, state.targetVec);
			state.targetW = VectorMath::normalize(VectorMath::cross(vecX, state.targetVec)) * 0.4f * GRAD_TO_RAD;
		}

		if (abs(dot) >0.9999 && VectorMath::lenght(w) < TURN_W) {
			state.zero();
			return;
		}

		state.targetW -= w * powf(2, (abs(dot) - 1) * 50);

		if (abs(dot) >0.9999) {
			state.targetW = sf::Vector3f(0, 0, 0);
		}
		float k[4];
		calculateFlyWheelComposition(k);
		calculateMoment(k);
		printf("dot = %f\ttargW = %f\tw = %f\n",
			dot, VectorMath::lenght(state.targetW), VectorMath::lenght(w));

	}

	void calculateFlyWheelComposition(float k[]) {
		for (int i = 0; i < 4; i++) {
			k[i] = 0;
		}
		sf::Vector3f w1 = toLocaleCS(VectorMath::normalize((w - state.targetW)));
		float eps = VectorMath::lenght(w1);
		while (eps > 0.0001) {

			float maxDot = 0;
			int maxI = -1;
			for (int i = 0; i < 4; i++) {
				float dot = VectorMath::dot(fw[i].direction, w1);
				if (abs(maxDot) < abs(dot) ) {
					maxI = i;
					maxDot = dot;
				}
			}

			k[maxI] += maxDot;
			w1 -= fw[maxI].direction * maxDot;
			eps = VectorMath::lenght(w1);

		}
	}

	void calculateMoment(float k[]) {
		float maxK = k[0];
		for (int i = 1; i < 4; i++) {
			if (abs(k[i]) > abs(maxK)) {
				maxK = k[i];
			}
		}
		if (maxK == 0) {
			return;
		}
		float momentMult = fw[0].maxMoment / abs(maxK);
		sf::Vector3f moment = sf::Vector3f(0, 0, 0);
		for (int i = 0; i < 4; i++) {
			moment -= toGlobalCS(fw[i].makeTorque(momentMult * k[i],dt) * fw[i].direction);
		}
		state.moment = moment;
	}

	void setStateStabilize(sf::Vector3f targetW) {
		state.zero();
		state.procedureCode = STABILIZE;
		state.targetW = targetW;
	}

	void setStateStabilize() {
		setStateStabilize(sf::Vector3f(0, 0, 0));
	}

	void setStateToEarth() {
		state.zero();
		state.procedureCode = TO_EARTH;
	}

	void setStateToSun() {
		state.zero();
		state.procedureCode = TO_SUN;
	}

	void setStateTwoZS() {
		state.zero();
		state.procedureCode = TWO_Z_S;
	}

	void setStateTwoZK() {
		state.zero();
		state.procedureCode = TWO_Z_K;
	}
	
	void setStateTurn(sf::Vector3f angles) {
		state.zero();
		state.procedureCode = TURN;
		state.angle = angles;
		float phi;
		if (state.angle.x != 0) {
			phi = state.angle.x * GRAD_TO_RAD;
			state.targetVec = toGlobalCS(sf::Vector3f(0,-sin(phi),cos(phi)));
		}
		else if (state.angle.y != 0) {
			phi = state.angle.y * GRAD_TO_RAD;
			state.targetVec = toGlobalCS(sf::Vector3f(sin(phi), 0, cos(phi)));
		}
		else if (state.angle.z != 0) {
			phi = state.angle.z * GRAD_TO_RAD;
			state.targetVec = toGlobalCS(sf::Vector3f(cos(phi), sin(phi), 0));

		}
	}
};



#pragma once

#include "SFML/Graphics.hpp"
#include "VectorMath.h"
#include <math.h>


class VerticalSensor
{
public:

	static sf::Vector3f getData(sf::Vector3f planetPos, sf::Vector3f satPos, sf::Vector3f vecX,sf::Vector3f vecY,sf::Vector3f vecZ) {

		sf::Vector3f data;
		data.x = VectorMath::dot(VectorMath::normalize(satPos-planetPos),vecX); 
		data.y = VectorMath::dot(VectorMath::normalize(satPos - planetPos), vecY);
		data.z = -1;
		if (VectorMath::dot(satPos - planetPos, vecZ) > 1) {
			data.z = 1;
		}
		return data;
			
	}

};


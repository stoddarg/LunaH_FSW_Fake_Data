#pragma once
#include <string>
#include <iostream>

class Ball
{
private:
	std::string m_color;
	double m_radius;
	int m_array[5];

public:
	Ball();
	Ball(std::string color);
	Ball(double rad);
	Ball(std::string color, double rad);
	~Ball();

	//member functions
	void print()
	{
		std::cout << "color: " << m_color << ", radius: " << m_radius << "\n";
	}
};


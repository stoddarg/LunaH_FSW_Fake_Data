#include "stdafx.h"
#include "Ball.h"

Ball::Ball() : m_color("black"), m_radius(10.0), m_array{ 1,2,3,4,5 }
{
}

//set just color
Ball::Ball(std::string color) : m_color(color), m_radius(10.0)
{
}

//set just radius
Ball::Ball(double rad): m_color("black"), m_radius(rad)
{
}

//set both
Ball::Ball(std::string color = "black", double rad = 10.0) : m_color(color), m_radius(rad)
{
}

//de-constructor
Ball::~Ball()
{
}
#pragma once

#include <iostream>

class Point3d
{
private:
	double m_x;
	double m_y;
	double m_z;
	bool equality;

public:
	Point3d();
	Point3d(double x, double y, double z);
	~Point3d();

	void setValues(double x, double y, double z)
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	void print()
	{
		std::cout << "< " << m_x << ", " << m_y << ", " << m_z << " >\n";
	}

	bool isEqual(const Point3d &p1)
	{
		if (m_x == p1.m_x)
		{
			if (m_y == p1.m_y)
			{
				if (m_z == p1.m_z)
					equality = true;
				else
					equality = false;
			}
			else
				equality = false;
		}
		else
			equality = false;

		return equality;
	}
};


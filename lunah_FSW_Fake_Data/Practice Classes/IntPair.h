#pragma once

#include <iostream>

class IntPair
{
private:
	int m_value1;
	int m_value2;

public:
	IntPair();
	IntPair(int x, int y);
	~IntPair();

	void set(int x, int y)
	{
		m_value1 = x;
		m_value2 = y;
	}

	void print()
	{
		std::cout << "Pair(" << m_value1 << ", " << m_value2 << ")\n";
	}
};


#pragma once
#include <cassert>
#include <iostream>

class DateClass
{
private:
	int m_year;
	int m_month;
	int m_day;

public:
	//The constructor will use the defaults if no input is given, otherwise will use what the user supplies
	DateClass(int year=2000, int month=1, int day=1);
	~DateClass();

	//define simple inline member functions in the header
	void setDate(int year, int month, int day)
	{
		assert(month <= 12 && day <= 31);

		m_year = year;
		m_month = month;
		m_day = day;
	}

	void print() //define member function
	{
		std::cout << m_year << "/" << m_month << "/" << m_day << '\n';
	}

	//define more complex member functions in the cpp file
	
};


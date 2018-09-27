#pragma once
#include <iostream>
#include <cassert>

class Stack
{
private:
	int m_stack_array[10];
	int m_stack_length;
	bool equality;

public:
	Stack();
	~Stack();

	void reset()
	{
		memset(&m_stack_array, '0', 10);
		m_stack_length = 0;
	}

	bool push(int value)
	{
		if (m_stack_length <= sizeof(m_stack_array))
		{
			m_stack_array[m_stack_length] = value;
			m_stack_length++;
			equality = true;
		}
		else
			equality = false;

		return equality;
	}

	int pop()
	{
		assert(m_stack_length >= 0);

		int holder = m_stack_array[m_stack_length];	//read in the value
		m_stack_array[m_stack_length] = 0;			//erase the value
		m_stack_length--;							//decrement the length of the stack

		return holder;	//return the popped value
	}

	void print()
	{
		std::cout << "( ";
		for (int i = 0; i < m_stack_length; i++)
		{
			std::cout << m_stack_array[i] << " ";
		}
		std::cout << " )\n";
	}
};


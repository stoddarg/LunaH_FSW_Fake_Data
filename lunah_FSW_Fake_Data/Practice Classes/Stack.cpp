#include "stdafx.h"
#include "Stack.h"


Stack::Stack()
{
	memset(&m_stack_array, '0', 10);
	m_stack_length = 0;
}


Stack::~Stack()
{
}

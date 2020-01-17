#include <stdlib.h>
#include <vector>
#include <tuple>
#include "Scanner.h"

using namespace std;

unsigned int Scanner::GetInputCode()
{
	if (m_InputFile.eof())
	{
		return 1;
	}
	for (int k = 0; k < m_CharSet.size(); ++k)
	{
		if (m_CurrentChar >= get<0>(m_CharSet[k]) && m_CurrentChar <= get<1>(m_CharSet[k]))
		{
			return get<2>(m_CharSet[k]);
		}
	}

	return 0;
}

int Scanner::NextLexeme()
{
	
	unsigned int z_Automaton = 0;
	unsigned int z_CurrentState = 0;
	unsigned int z_InputCode = 0;
	while (true)
	{
		z_InputCode = GetInputCode();

		z_CurrentState = m_Automata[z_Automaton][z_CurrentState][z_InputCode];

		switch (z_CurrentState)
		{

		default:
			continue;
		}
	}
}
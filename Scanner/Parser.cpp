#include "Parser.h"

int Parser::ParseStatement()
{
	int z_ReturnCode = 0;
	int z_CurrentState = 0;
	int z_InputCode = m_Scanner.NextLexeme();
	if (z_InputCode == -3)
	{
		return -3;
	}
	if (z_InputCode == -4)
	{
		return -4;
	}
	unsigned int z_Automaton = m_Automata[0][0][z_InputCode];
	while (true)
	{
		z_InputCode = GetInputCode();
		z_CurrentState = m_Automata[z_Automaton][z_CurrentState][z_InputCode];
		if (z_CurrentState < 0 && z_CurrentState != -1)
		{
			z_ReturnCode = -z_CurrentState;
			z_CurrentState = -2;
		}
		switch (z_CurrentState)
		{
		case -4: //Invalid character.
			//return -something
		case -3: //Invalid token.
			//return -something
		case -2: //Literal.
			return z_ReturnCode;
		case -1: //Keyword/identifier/operator.
			return TokenCode();
		default:
			continue;
		}
	}
}
#include "Parser.h"

int Parser::ParseStatement()
{
	int z_ReturnCode = 0;
	int z_CurrentState = 0;
	int z_InputCode = m_Scanner.NextLexeme();
	if (z_InputCode == -3)
	{//Invalid token.
		return -3;
	}
	if (z_InputCode == -4)
	{//Invalid character.
		return -4;
	}
	int z_Automaton = m_StateTables[0][0][z_InputCode];
	if (z_Automaton < 0)
	{//Single token statement.
		return -z_Automaton;
	}
	while (true)
	{
		z_InputCode = m_Scanner.NextLexeme();
		if (z_InputCode == -3)
		{//Invalid token.
			return -2;
		}
		if (z_InputCode == -4)
		{//Invalid character.
			return -3;
		}
		z_CurrentState = m_StateTables[z_Automaton][z_CurrentState][z_InputCode];
		if (z_CurrentState == -1)
		{//Invalid statement.
			return -1;
		}
		if (z_CurrentState < -1)
		{//Valid statement.
			return -z_CurrentState;
		}
	}
}
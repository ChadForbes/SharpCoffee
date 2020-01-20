#include <stdlib.h>
#include <vector>
#include <tuple>
#include <map>
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

int Scanner::TokenCode()
{
	std::map<std::string, int>::const_iterator z_TokenValue = m_KeywordTable.find(m_CurrentLexeme);
	if (z_TokenValue == m_KeywordTable.end())
	{
		return m_KeywordTable["Identifier"];
	}
	return z_TokenValue->second;
}

int Scanner::NextLexeme()
{
	int z_ReturnCode = 0;
	int z_CurrentState = 0;
	unsigned int z_InputCode = GetInputCode();
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
		case -2: //Literal.
			return z_ReturnCode;
		case -1: //Keyword/identifier.
			return TokenCode();
		default:
			continue;
		}
	}
}
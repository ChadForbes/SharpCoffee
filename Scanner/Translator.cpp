#include "Translator.h"

using namespace std;

void Translator::Translate(std::string a_SourceFile, std::string a_OutputFile)
{


	int z_ReturnCode = 0;
	int z_CurrentState = 0;
	unsigned int z_InputCode = m_Scanner.NextLexeme();
	unsigned int z_Automaton = m_Automata[0][0][z_InputCode];
	while (true)
	{

	}
}
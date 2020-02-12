#pragma once
#include "Scanner.h"

class Parser
{
public:
	Parser();
	Parser(std::string a_InputFile);
	~Parser()
	{
		m_Scanner.~Scanner();
	}
	int ParseStatement();

private:
	std::vector<std::vector<std::vector<int>>> m_StateTables;
	Scanner m_Scanner;
	//Disallow copying or assignment.
	Parser(Parser& a_Source) {};
	Parser operator =(Parser& a_Value) {};
};

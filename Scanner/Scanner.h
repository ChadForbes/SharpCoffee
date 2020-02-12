#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>

class Scanner
{
public:
	Scanner();
	Scanner(std::string a_FilePath);
	~Scanner()
	{
		m_InputFile.close();
	}

	std::ifstream m_InputFile;
	std::string m_CurrentLexeme = "";

	int NextLexeme();
	int TokenCode();

private:
	char m_CurrentChar;
	std::vector<std::vector<std::vector<int>>> m_StateTables;
	std::vector<std::string> m_InputClasses;
	std::map<std::string, int> m_KeywordTable;

	unsigned int GetInputCode();

	//Disallow copying or assignment.
	Scanner(Scanner& a_Source) {};
	Scanner operator =(Scanner& a_Value) {};
};


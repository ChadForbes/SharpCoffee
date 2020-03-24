#include <stdlib.h>
#include <vector>
#include <tuple>
#include <map>
#include <iostream>
#include "json/json.h"
#include "Scanner.h"

using namespace std;

Scanner::Scanner(string a_FilePath)
{
	m_InputFile.open(a_FilePath);
	if (m_InputFile)
	{
		m_CurrentChar = m_InputFile.get();
		if (m_CurrentChar == static_cast<char>(239))
		{
			char z_SecondByteOrderMark = m_InputFile.get();
			if (z_SecondByteOrderMark == static_cast<char>(187))
			{
				char z_ThirdByteOrderMark = m_InputFile.get();
				if (z_ThirdByteOrderMark == static_cast<char>(191))
				{
					m_CurrentChar = m_InputFile.get();
				}
				else
				{
					m_InputFile.putback(z_ThirdByteOrderMark);
					m_InputFile.putback(z_SecondByteOrderMark);
				}
			}
			else
			{
				m_InputFile.putback(z_SecondByteOrderMark);
			}
		}
	}
	else
	{
		//Throw error of some sort.
		cout << "Could not open file. Filepath: " << a_FilePath;
	}

	Json::Reader z_Reader;
	Json::Value z_Root;
	ifstream z_KeywordsFile("C:\\Users\\Glenden\\source\\repos\\SharpCoffee\\KeywordsOperators_CSharp.json");
	z_Reader.parse(z_KeywordsFile, z_Root, false);
	Json::Value::Members popnam = z_Root.getMemberNames();
	int i = 0;
	for (string x : popnam) 
	{
		m_KeywordTable.insert(make_pair(x, z_Root[x].asInt()));
		++i;
	}

	ifstream z_InputClassesFile("C:\\Users\\Glenden\\source\\repos\\SharpCoffee\\InputClasses_CSharp.json");
	z_Reader.parse(z_InputClassesFile, z_Root, false);
	string z_Result;
	Json::Value& z_Inputs = z_Root["CSharpInputClasses"];
	for (int i = 0; i < z_Inputs.size(); i++) 
	{
		z_Result = z_Inputs[i].asString();
		m_InputClasses.push_back(z_Result);
	}

	ifstream ifs("C:\\Users\\Glenden\\source\\repos\\SharpCoffee\\ScannerStateTables_CSharp.json");
	z_Reader.parse(ifs, z_Root, false);
	Json::Value& tables = z_Root["ScannerStateTables_CSharp"];
	for (int i = 0; i < tables.size(); i++) 
	{
		vector<vector<int>> stat;
		m_StateTables.push_back(stat);
		for (int j = 0; j < tables[i].size(); j++) 
		{
			vector <int> v;
			m_StateTables[i].push_back(v);
			for (int k = 0; k < tables[i][0].size(); k++) 
			{
				m_StateTables[i][j].push_back(tables[i][j][k].asInt());
			}
		}
	}
}

unsigned int Scanner::GetInputCode()
{
	if (m_InputFile.eof())
	{
		return 1;
	}
	for (int k = 0; k < m_InputClasses.size(); ++k)
	{
		if (m_InputClasses[k].find(m_CurrentChar) != string::npos)
		{
			return k + 2;
		}
	}
	return 0;
}

int Scanner::TokenCode()
{
	map<string, int>::const_iterator z_TokenValue = m_KeywordTable.find(m_CurrentLexeme);
	if (z_TokenValue == m_KeywordTable.end())
	{
		return m_KeywordTable["identifier"];
	}
	return z_TokenValue->second;
}

int Scanner::NextLexeme()
{
	int z_ReturnCode = 0;
	int z_CurrentState = 0;
	unsigned int z_InputCode = -4;
	unsigned int z_StateTable = 0;
	while (true)
	{//Loop to allow for the removal of irrelavent characters.
		z_InputCode = GetInputCode();
		z_StateTable = m_StateTables[0][0][z_InputCode];
		if (z_StateTable == -4)
		{
			m_CurrentLexeme = "EOF";
			return -1;
		}
		m_CurrentLexeme = "";
		m_CurrentLexeme += m_CurrentChar;
		m_CurrentChar = m_InputFile.get();
		if (z_StateTable != 0)
		{//If the current character is not irrelevant continue to the next loop.
			break;
		}
	}
	while (true)
	{
		z_InputCode = GetInputCode();
		z_CurrentState = m_StateTables[z_StateTable][z_CurrentState][z_InputCode];
		if (z_CurrentState < -4)
		{
			z_ReturnCode = z_CurrentState;
			z_CurrentState = -2;
		}
		switch (z_CurrentState)
		{
		case -4: //Invalid character.
			return -4;
		case -3: //Invalid token.
			return -3;
		case -2: //Literal.
			return z_ReturnCode;
		case -1: //Keyword/identifier/operator.
			return TokenCode();
		default:
			m_CurrentLexeme += m_CurrentChar;
			m_CurrentChar = m_InputFile.get();
			continue;
		}
	}
}
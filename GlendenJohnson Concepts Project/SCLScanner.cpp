//Course: CS4308
//Instructor: Dr. Garrido
//Assignment: Course Project
//Written by Glenden Johnson
//Last modified: 2/25/2019

#include "SCLScanner.h"
#include <cstdlib>
#include <cctype>
#include <iostream>

namespace SCLInterpreter
{
	SCLScanner::SCLScanner(std::string a_FilePath)
	{
		m_InputFile.open(a_FilePath);
		if (m_InputFile)
		{
			m_InputFile.get(m_CurrentChar);
		}
		else
		{
			std::cout << "Could not open file. Filepath: " << a_FilePath;
		}
	}

	int SCLScanner::TokenCode()
	{
		std::map<std::string, int>::const_iterator z_TokenValue = m_KeywordTable.find(m_CurrentLexeme);
		//If Keyword table does not contain the current lexeme then it is an identifier.
		if (z_TokenValue == m_KeywordTable.end())
		{
			return 100; //Return the code for an identifier.
		}
		//Return the token code that is associated with the current lexeme.
		return z_TokenValue->second;
	}

	int SCLScanner::NextLexeme()
	{
		m_CurrentLexeme = "";
		while (true) //Loop to allow for removal of whitespace.
		{
			unsigned int z_Automata = 0;

			//Check for end of file.
			if (m_InputFile.eof())
			{
				return -1;
			}
			//Check for, and remove, whitespace.
			else if (std::isspace(m_CurrentChar))
			{
				m_InputFile.get(m_CurrentChar);
				continue;
			}
			//Keyword of identifier, use Automaton #1.
			else if (std::isalpha(m_CurrentChar) || m_CurrentChar == '_')
			{
				z_Automata = 1;
			}
			//Unsigned numeric literal, use Automaton #2.
			else if (std::isdigit(m_CurrentChar))
			{
				z_Automata = 2;
			}
			//Operator or signed numeric literal, use Automaton #3.
			else if (m_CurrentChar == '+' || m_CurrentChar == '-')
			{
				z_Automata = 3;
			}
			//Char literal, use Automaton #4.
			else if (m_CurrentChar == '\'')
			{
				z_Automata = 4;
			}
			//String literal, use Automaton #5.
			else if (m_CurrentChar == '"')
			{
				z_Automata = 5;
			}
			//Single character tokens, return immediately.
			else if (m_CurrentChar == '%' || m_CurrentChar == '*' || m_CurrentChar == '(' || m_CurrentChar == ')' || m_CurrentChar == '[' || m_CurrentChar == ']' || m_CurrentChar == ',' || m_CurrentChar == ':' || m_CurrentChar == '.' || m_CurrentChar == '=')
			{
				//Add last scanned character to current lexeme.
				m_CurrentLexeme += m_CurrentChar;
				//Get next character.
				m_InputFile.get(m_CurrentChar);
				//Return the token code.
				return TokenCode();
			}
			//Operator or comment, use Automaton #6.
			else if (m_CurrentChar == '/')
			{
				z_Automata = 6;
			}
			else
			{
				return -3; //Invalid character exception.
			}

			unsigned int z_CurrentState = 0;
			unsigned int z_FormattedInput = 9;
			for (int j = 255; j >= 0; --j) //Max identifier and string length of 256.
			{
				//Add the last scanned character to the current lexeme.
				m_CurrentLexeme += m_CurrentChar;
				//Get the next character.
				m_InputFile.get(m_CurrentChar);
				
				//Assign a numeric value to the relavent types of input characters
				//	so that the input characters can be fed to the automata in a 
				//	more compact manner.
				if (m_InputFile.eof())
				{
					z_FormattedInput = 0;
				}
				else if (m_CurrentChar == '\n')
				{
					z_FormattedInput = 1;
				}
				else if (std::isalpha(m_CurrentChar) || m_CurrentChar == '_')
				{
					z_FormattedInput = 2;
				}
				else if (std::isdigit(m_CurrentChar))
				{
					z_FormattedInput = 3;
				}
				else if (m_CurrentChar == '/')
				{
					z_FormattedInput = 4;
				}
				else if (m_CurrentChar == '.')
				{
					z_FormattedInput = 5;
				}
				else if (m_CurrentChar == '\'')
				{
					z_FormattedInput = 6;
				}
				else if (m_CurrentChar == '\\')
				{
					z_FormattedInput = 7;
				}
				else if (m_CurrentChar == '"')
				{
					z_FormattedInput = 8;
				}
				else
				{
					z_FormattedInput = 9;
				}
				//Determine next state.
				z_CurrentState = m_StateTable[z_Automata][z_CurrentState][z_FormattedInput];
				switch (z_CurrentState)
				{
				case 10:
					//Valid word or operator.
					return TokenCode();
				case 11:
					//Valid signed integer.
					return 1;
				case 12:
					//Valid unsigned integer.
					return 2;
				case 13:
					//Valid signed real.
					return 3;
				case 14:
					//Valid unsigned real.
					return 4;
				case 15:
					//Valid char.
					m_CurrentLexeme.erase(0, 1);
					m_InputFile.get(m_CurrentChar);
					return 5;
				case 16:
					//Valid string.
					m_CurrentLexeme.erase(0, 1);
					m_InputFile.get(m_CurrentChar);
					return 6;
				case 17:
					//Invalid token.
					return -2;
				case 18:
					//Invalid character read.
					return -3;
				case 19:
					//Valid comment.
					return 7;
				case 20:
					//Automata state error.
					return -4;
				default:
					continue;
				}//end switch
			}//end for
			return -5; //Char, string, or comment exceeds allowable size.

		}//end while

	}//end function
}
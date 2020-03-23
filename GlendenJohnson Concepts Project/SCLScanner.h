//Course: CS4308
//Instructor: Dr. Garrido
//Assignment: Course Project
//Written by Glenden Johnson
//Last modified: 2/25/2019
//m_Keyword table updated 4/3/2019 by Glenden Johnson

#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace SCLInterpreter
{
#ifndef KEYWORD_TABLE
#define KEYWORD_TABLE
	static const std::map<std::string, int> m_KeywordTable = {
		{ "=", 101 },			{ "+", 102 },			{ "-", 103 },				{ "*", 104 },
		{ "/", 105 },			{ "%", 106 },			{ "(", 108 },				{ ")", 109 },
		{ "[", 110 },			{ "]", 111 },			{ ",", 112 },				{ "band", 114 },
		{ "bor", 115 },			{ "bxor", 116 },		{ "mod", 117 },				{ "lshift", 118 },
		{ "rshift", 119 },		{ "negate", 120 },		{ "true", 121 },			{ "false", 122 },
		{ "define", 123 },		{ "of", 124 },			{ "type", 125 },			{ "char", 126 },
		{ "integer", 127 },		{ "float", 128 },		{ "string", 129 },			{ "bool", 130 },
		{ "byte", 131 },		{ "array", 132 },		{ "declarations", 133 },	{ "constants", 134 },
		{ "variables", 135 },	{ "function", 138 },	{ "is", 139 },				{ "begin", 140 },
		{ "endfun", 141 },		{ "parameters", 142 },	{ "set", 143 },				{ "display", 144 },
		{ "increment", 145 },	{ "decrement", 146 },	{ "return", 147 },			{ "call", 148 },
		{ "or", 149 },			{ "and", 150 },			{ "not", 151 },				{ "equals", 152 },
		{ "greater", 153 },		{ "less", 154 },		{ "than", 155 },			{ "equal", 156 },
		{ "if", 157 },			{ "then", 158 },		{ "elseif", 159 },			{ "else", 160 },
		{ "endif", 161 },		{ "do", 165 },			{ "while", 170 },			{ "endwhile", 171 },
		{ "main", 177 },		{ "void", 178 },		{ "pointer", 179 },			{ "using", 180 },
		{ "global", 181 },		{ "implementations", 182 },

	};
#endif


	class SCLScanner
	{
	public:
		//Constructor(s)
		SCLScanner(std::string a_FilePath);
		SCLScanner() {};

		//Destructor
		~SCLScanner()
		{
			m_InputFile.close();
		}

		//Variables
		std::ifstream m_InputFile;
		std::string m_CurrentLexeme = "";

		//Functions
		int NextLexeme();
		int TokenCode();

	private:
		//Most recently scanned character.
		char m_CurrentChar;
		//Next state table for finite state automata.
		const std::vector<std::vector<std::vector<unsigned int>>> m_StateTable = {
			{//Automaton #0
				{ 20,20,20,20,20,20,20,20,20,20 }
			},
			{//Automaton #1
				{ 10,10, 0, 0,10,10,10,10,10,10 }
			},
			{//Automaton #2
				{ 12,12,12, 0,12, 1,12,12,12,12 },
				{ 17,17,17, 2,17,17,17,17,17,17 },
				{ 14,14,14, 2,14,14,14,14,14,14 }
			},
			{//Automaton #3
				{ 10,10,10, 1,10,10,10,10,10,10 },
				{ 11,11,11, 1,11, 2,11,11,11,11 },
				{ 17,17,17, 3,17,17,17,17,17,17 },
				{ 13,13,13, 3,13,13,13,13,13,13 }
			},
			{//Automaton #4
				{ 17, 1, 1, 1, 1, 1,15, 2, 1, 1 },
				{ 17,17,17,17,17,17,15,17,17,17 },
				{ 17, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
			},
			{//Automaton #5
				{ 17, 0, 0, 0, 0, 0, 0, 1,16, 0 },
				{ 17, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{//Automaton #6
				{ 10,10,10,10, 1,10,10,10,10,10 },
				{ 19,19, 1, 1, 1, 1, 1, 1, 1, 1 }
			}
		};

		//Disallow copying or assignment
		SCLScanner(SCLScanner& a_Source) {};
		SCLScanner operator =(SCLScanner& a_S1) {};
	};//end class SCLScanner
}
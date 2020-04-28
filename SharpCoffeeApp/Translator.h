#pragma once
#ifndef Translator_H
#define Translator_H
#include <string>
#include <map>
#include <vector>
#include <utility>
#include "Scanner.h"

class Translator
{
private:
	/*
	1. Add member variable to Translator class: vector<pair<string, int>> (A vector of pairs; each a pair of
	(z_str, z_numCode). Vector in C++ like ArrayList/List in Java.
	2. Instead of translateStr(), add the (z_str, z_numCode) pair to the end of the vector.
	(z_str, z_numCode) is appended to the vector first instead of m_outputFileString.
	3. Make any fixes (insertions, deletions,...) to the vector at any set index easily; no dealing with string.
	4. Now, we call translateStr() in the for loop, appending each translated (z_str, z_numCode) to
	m_outputFileString.
	*/
	std::string m_inputFilePath;
	std::string m_inputLanguage;
	std::string m_outputLanguage;

	std::string m_mappingFilePath;

	std::vector<std::pair<std::string, int>> m_outputVector;
	std::string m_HSB; // HeaderStringBuilder
	std::string m_BSB; // BodyStringBuilder

	Scanner scanner;

	std::map<std::string, std::map<std::string, std::string>> m_Mapping;
public:
	Translator();
	Translator(	std::string a_inputFilePath,
				std::string a_inputLanguage = "CSharp",
				std::string a_outputLanguage = "output");

	void translate();
	void translateStr(std::string a_str, int a_numCode);
	bool found(std::string a_vectorStr, std::string a_str);
	bool inMapping(std::string a_str);
	std::string m_outputHeaderString(std::string a_str);
	std::string m_outputBodyString(std::string a_str);
	std::string m_outputFileString;
};

#endif // !Translator_H
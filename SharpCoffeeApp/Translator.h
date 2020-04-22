#pragma once
#ifndef Translator_H
#define Translator_H
#include <string>
#include <map>
#include "Scanner.h"

class Translator
{
private:
	std::string m_inputFilePath;
	std::string m_inputLanguage;
	std::string m_outputLanguage;

	std::string m_mappingFilePath;

	// std::string m_outputFileString;
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
	void translateStr(std::string str, int numCode);
	bool inMapping(std::string str);
	void getHeaderInfo(std::string str);
	std::string toOutputString(std::string str);
	std::string m_outputFileString;
};

#endif // !Translator_H
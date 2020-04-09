#pragma once
#ifndef Translator_H
#define Translator_H
#include <string>
#include <map>

class Translator
{
private:
	std::string outputFilePath;
	std::string mappingFilePath;
	std::string javaFileString;
	std::string HSB; // HeaderStringBuilder
	std::string BSB; // BodyStringBuilder
	//std::ifstream mapping;
	//			C# token	map	(	Java token,	Java header info	)
	std::map<std::string, std::map<std::string, std::string>> m_Mapping;
public:
	Translator();
	Translator(std::string outputFilePath = "JavaFiles\\HelloWorld.java", std::string mappingFilePath = "CSharp_to_Java_Mapping.json");
	void translate();
	void translateStr(std::string str, int numCode);
	bool inMapping(std::string str);
	// bool inMapping(int numCode);
	void getHeaderInfo(std::string str);
	std::string toJavaString(std::string str);
};

#endif // !Translator_H
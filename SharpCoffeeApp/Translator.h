#pragma once
#ifndef Translator_H
#define Translator_H
#include <string>

class Translator
{
private:
	//std::string inputFilePath;
	std::string javaString;
	std::string HSB; // HeaderStringBuilder
	std::string BSB; // BodyStringBuilder
public:
	Translator();
	Translator(std::string inputFilePath);
	void translate(std::string str, int numCode);
	bool inMapping(std::string str);
	bool inMapping(int numCode);
	std::string toJavaString(std::string str);
};

#endif // !Translator_H
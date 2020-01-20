#pragma once
#include "Scanner.h"

class Translator
{
public:
	Translator();
	~Translator();

	void Translate(std::string a_SourceFile, std::string a_OutputFile);

private:
	Scanner m_Scanner;

	//Disallow copying or assignment.
	Translator(Translator& a_Source) {};
	Translator operator=(Translator& a_Source) {};
};


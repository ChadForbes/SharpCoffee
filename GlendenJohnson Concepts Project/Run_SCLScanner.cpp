//Course: CS4308
//Instructor: Dr. Garrido
//Assignment: Course Project
//Written by Glenden Johnson
//Last modified: 2/25/2019

#include "SCLScanner.h"
#include <fstream>

using namespace SCLInterpreter;

int main(int argc, char* argv[])
{
	SCLScanner z_scanner(argv[1]);
	std::ofstream z_OutputFile(".\\SCLScannerOutput.txt");

	z_OutputFile << "Scanning file " << argv[1] << "\n";
	int z_TokenCode = 0;
	while (z_scanner.m_InputFile.good() && !z_scanner.m_InputFile.eof())
	{
		z_TokenCode = z_scanner.NextLexeme();
		z_OutputFile << "Token Code: " << z_TokenCode << "\tToken: " << z_scanner.m_CurrentLexeme << "\n";
	}
	z_OutputFile << "Scanning complete.";
	z_OutputFile.close();

	return EXIT_SUCCESS;
}
#include <string>
#include <stdlib.h>
#include <iostream>
#include "Scanner.h"

using namespace std;

int main(int argc, char* argv[])
{
	string z_InputFilePath = "";
	string z_OutputFilePath = "";
	if (argc == 3)
	{
		z_InputFilePath = argv[1];
		z_OutputFilePath = argv[2];
	}
	else if (argc == 2)
	{
		z_InputFilePath = argv[1];
		z_OutputFilePath = "";
	}
	else
	{
		cerr << "Error: Invalid parameters.\nExpecting: SharpCoffeeApp.exe <Input File Path> [<Output File Path>]" << endl;
		return EXIT_FAILURE;
	}
	cout << "Initializing tokanizer..." << endl;
	Scanner z_Tokanizer(z_InputFilePath);
	for (int j = 0; j != 1; )
	{
		j = z_Tokanizer.NextLexeme();
		cout << "Token: " << z_Tokanizer.m_CurrentLexeme << "\tToken Code: " << j << endl;
	}
	return EXIT_SUCCESS;
}
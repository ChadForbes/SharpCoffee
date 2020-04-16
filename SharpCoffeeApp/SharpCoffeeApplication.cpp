#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include "Scanner.h"
#include "Translator.h"

using namespace std;

int main(int argc, char* argv[])
{
	string z_InputFilePath = "";
	string z_OutputFilePath = "";
	string z_InputLanguage = "CSharp";
	string z_OutputLanguage = "Java";
	if (argc == 5)
	{
		z_InputFilePath = argv[1];
		z_InputLanguage = argv[2];
		z_OutputFilePath = argv[3];
		z_OutputLanguage = argv[4];
	}
	else if (argc == 1)
	{
		wchar_t* z_Argv = Py_DecodeLocale("SCGUI.pyw", NULL);
		Py_Initialize();
		PySys_SetArgv(1, &z_Argv);
		PyObject* z_GUIObject = Py_BuildValue("s", "SCGUI.pyw");
		FILE* z_GUIFile = _Py_fopen_obj(z_GUIObject, "r+");
		PyRun_SimpleFile(z_GUIFile, "SCGUI.pyw");
		Py_FinalizeEx();
		return EXIT_SUCCESS;
	}
	else
	{
		cerr << "Error: Invalid parameters.\nExpecting: SharpCoffeeApp.exe [<Input File Path> <Input Language> <Output File Path> <Output Language>]" << endl;
		return EXIT_FAILURE;
	}
	try 
	{
		std::string z_TranslatedProgram = "";
		//Initialize Translator.
		Translator z_Translator(z_InputFilePath, z_OutputFilePath);
		z_Translator.translate();
		Scanner z_Tokanizer(z_InputFilePath, z_InputLanguage);

		//Print the translated program to the console. This can be captured as a string by the Python GUI.
		//cout << z_TranslatedProgram;
	}
	catch (string e_Error)
	{
		cerr << e_Error << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
#include "Translator.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "json/json.h"
#include <iterator>

using namespace std;

/*
Current issues:
* Currently it is producing output almost never on the first click and most of the time on the second click.
* Replace "using" with "import"
* If "using System;", just ignore
* Find a way to ignore namespace declaration and first set of curly braces
* "public" being deleted instead of just being ignored

Current output:

using System;

namespace HelloWorld
{
    class HelloWorld
    {
        static void main(String[] args)(string[] args)
        {
            System.out.println("Hello World!");
        }
    }
}


Switch to a certain translation protocol based on which language we are translating to.
Leave the main translation function mostly alone if we handle issues like the "public" or 
"namespace" ones in a preprocessing function that then hands the edited C# code to the translation function.
*/

Translator::Translator() {
    m_inputFilePath = "..\\TestPrograms\\CSharp\\HelloWorld.cs";
    m_inputLanguage = "CSharp";
    m_outputLanguage = "Java";

    m_mappingFilePath = "Mappings\\" + m_inputLanguage + "_to_" + m_outputLanguage + "_Mapping.json";

    m_outputFileString = "";
    m_HSB = "";
    m_BSB = "";

    scanner.Init(m_inputFilePath);

    Json::Reader z_Reader;
    Json::Value z_Root;
    ifstream z_ifs(m_mappingFilePath);
    z_Reader.parse(z_ifs, z_Root, false);

    Json::Value::Members popnam = z_Root.getMemberNames();
    Json::Value::Members membername;

    for (string x : popnam) {
        membername = z_Root[x].getMemberNames();
        m_Mapping.insert(make_pair(x, map<string, string>()));

        for (string y : membername) {
            m_Mapping[x].insert(make_pair(y, z_Root[x][y].asString()));
        }
    }
}

Translator::Translator(string a_inputFilePath, string a_inputLanguage, string a_outputLanguage) {
    m_inputFilePath = a_inputFilePath;
    m_inputLanguage = a_inputLanguage;
    m_outputLanguage = a_outputLanguage;

    m_mappingFilePath = "Mappings\\" + m_inputLanguage + "_to_" + m_outputLanguage + "_Mapping.json";

    m_outputFileString = "";
    m_HSB = "";
    m_BSB = "";

    scanner.Init(m_inputFilePath);

    Json::Reader z_Reader;
    Json::Value z_Root;
    ifstream z_ifs(m_mappingFilePath);
    z_Reader.parse(z_ifs, z_Root, false);
    Json::Value::Members popnam = z_Root.getMemberNames();
    Json::Value::Members membername;

    for (string x : popnam) {
        membername = z_Root[x].getMemberNames();
        m_Mapping.insert(make_pair(x, map<string, string>()));

        for (string y : membername) {
            m_Mapping[x].insert(make_pair(y, z_Root[x][y].asString()));
        }
    }
}

void Translator::translate() {
    string z_prevStr = "";
    int z_numCode = 0;
    string z_str = "";

    while (z_numCode != -1) {
        z_prevStr = z_str;
        z_numCode = scanner.NextLexeme();
        z_str = scanner.m_CurrentLexeme;
        // Code to fix "public" issues for C#
        if (m_inputLanguage == "CSharp") {
            if (z_str == "class") {
                if (z_prevStr != "private" || z_prevStr != "protected") {
                    m_BSB += "public ";
                }
            }
            if (z_str == "main") {
                // If "main" is found, go back 19 indices to see if public is there
                size_t index = m_BSB.find("main");
                // If "public" isn't already there, add it
                if (m_BSB.substr(index - 19, 6) != "public") { // The possible index of "public"
                    m_BSB.insert(index - 12, "public "); // Insert before "static"
                }
            }
            // Skip over "namespace", project name, and opening curly brace
            if (z_str == "namespace") {             // Current lexeme is "namespace"
                z_numCode = scanner.NextLexeme();   // Current lexeme is the project name ("HelloWorld")
                z_numCode = scanner.NextLexeme();   // Current lexeme is "{"
                z_numCode = scanner.NextLexeme();   // Current lexeme is "class" or whatever is after the curly brace
            }
        }
        /*
        */
        translateStr(z_str, z_numCode);
    }
    // Skip over closing curly brace of "namespace"
    int i = m_BSB.length;
    size_t found = m_BSB.rfind("}");
    if (found != std::string::npos) {
        m_BSB.replace(found, 1, "");
    }
    /*
    */

    m_outputFileString = m_HSB + "\n" + m_BSB;
    // cout << m_outputFileString;
}

void Translator::translateStr(string a_str, int a_numCode) {
    // str is a keyword/operator
    if (a_numCode >= 0) {
        if (inMapping(a_str)) {
            m_BSB += toOutputString(a_str);
            getHeaderInfo(a_str);
        }
        else if (!inMapping(a_str)) {
            m_BSB += a_str;
        }
     // str is a literal
    } else if (a_numCode < 0) {
        string numCodeStr = to_string(a_numCode);
        if (inMapping(numCodeStr)) {
            m_BSB += toOutputString(numCodeStr);
            getHeaderInfo(numCodeStr);
        }
        else if (!inMapping(numCodeStr)) {
            m_BSB += a_str;
        }
    }
}

bool Translator::inMapping(string a_str) {
    if (m_Mapping.find(a_str) != m_Mapping.end()) {
        return true;
    }
    else return false;
}

void Translator::getHeaderInfo(string a_str) {
    map<string, string> z_outputMap = m_Mapping.find(a_str)->second;
    m_HSB += z_outputMap.find("head")->second;
}

string Translator::toOutputString(std::string a_str) {
    map<string, string> z_outputMap = m_Mapping.find(a_str)->second;
    return z_outputMap.find("body")->second;
}
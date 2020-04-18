#include "Translator.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "json/json.h"
#include <iterator>

using namespace std;

// No longer necessary to take in output file path
Translator::Translator() {
    m_inputFilePath = "..\\TestPrograms\\CSharp\\HelloWorld.cs";
    // m_outputFilePath = "JavaFiles\\HelloWorld.java";
    m_inputLanguage = "CSharp";
    m_outputLanguage = "Java";

    m_mappingFilePath = "Mappings\\" + m_inputLanguage + "_to_" + m_outputLanguage + "_Mapping.json";

    m_javaFileString = "";
    m_HSB = "";
    m_BSB = "";

    scanner = new Scanner(m_inputFilePath);

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
    // m_outputFilePath = a_outputFilePath;
    m_inputLanguage = a_inputLanguage;
    m_outputLanguage = a_outputLanguage;

    m_mappingFilePath = "Mappings\\" + m_inputLanguage + "_to_" + m_outputLanguage + "_Mapping.json";

    m_javaFileString = "";
    m_HSB = "";
    m_BSB = "";

    scanner.Init(m_inputFilePath);
    // scanner = new Scanner(m_inputFilePath);

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
    while (scanner.m_CurrentLexeme != "EOF") {
        int z_numCode = scanner.NextLexeme();
        string z_str = scanner.m_CurrentLexeme;
        translateStr(z_str, z_numCode);
    }
    m_javaFileString = m_HSB + "\n" + m_BSB;

    // Write translated code to console
    cout << m_javaFileString;

    /*
    Call NextLexeme() first, then get m_CurrentLexeme;

    The NextLexeme() function is what the Translator will be calling.
    After each call to the NextLexeme() function the value of m_CurrentLexeme will be
    the string version of the most recently read token.
    */
}

void Translator::translateStr(string a_str, int a_numCode) {
    // str is a keyword/operator
    if (a_numCode >= 0) {
        if (inMapping(a_str)) {
            m_BSB += toJavaString(a_str);
            getHeaderInfo(a_str);
        }
        else if (!inMapping(a_str)) {
            m_BSB += a_str;
        }
     // str is a literal
    } else if (a_numCode < 0) {
        string numCodeStr = to_string(a_numCode);
        if (inMapping(numCodeStr)) {
            m_BSB += toJavaString(a_str);
            getHeaderInfo(a_str);
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
    map<string, string> z_javaMap = m_Mapping.find(a_str)->second;
    m_HSB += z_javaMap.find("head")->second;
    // m_HSB += z_javaMap.begin()->second;
}

string Translator::toJavaString(std::string a_str) {
    map<string, string> z_javaMap = m_Mapping.find(a_str)->second;

    return z_javaMap.find("body")->second;
    // return z_javaMap.begin()->first;
}

/*
Current issues:
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
*/
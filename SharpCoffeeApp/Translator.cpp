#include "Translator.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>
#include "json/json.h"

using namespace std;

/*
Current issues:
1. Translate .Length to .length
    * C# .Length used for arrays and strings. Java .length used for arrays, .length() used for strings
    * 
*/

Translator::Translator() {
    m_inputFilePath = "..\\TestPrograms\\CSharp\\HelloWorld.cs";
    m_inputLanguage = "CSharp";
    m_outputLanguage = "Java";

    m_mappingFilePath = "Mappings\\" + m_inputLanguage + "_to_" + m_outputLanguage + "_Mapping.json";

    //m_outputVector;

    m_HSB = "";
    m_BSB = "";
    m_outputFileString = "";

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
    z_ifs.close();
}

Translator::Translator(string a_inputFilePath, string a_inputLanguage, string a_outputLanguage) {
    m_inputFilePath = a_inputFilePath;
    m_inputLanguage = a_inputLanguage;
    m_outputLanguage = a_outputLanguage;

    m_mappingFilePath = "Mappings\\" + m_inputLanguage + "_to_" + m_outputLanguage + "_Mapping.json";

    m_HSB = "";
    m_BSB = "";
    m_outputFileString = "";

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
    z_ifs.close();
}

void Translator::translate() {
    string z_str = "";
    int z_numCode = 0;

    // while loop to add all string and numeric code pairs to the vector
    while (z_numCode != -1) {
        z_numCode = scanner.NextLexeme();
        z_str = scanner.m_CurrentLexeme;
        m_outputVector.push_back(pair<string, int>(z_str, z_numCode));
    }

    vector<pair<string, int>>::iterator z_it;
    vector<pair<string, int>>::iterator z_it2;
    string z_projectName;
    if (m_inputLanguage == "CSharp" && m_outputLanguage == "Java") {
        for (z_it = m_outputVector.begin(); z_it != m_outputVector.end(); ++z_it) {
            // FIX: Omit "using", " ", "System", and ";"
            if (z_it->first == "using" && (z_it + 2)->first == "System") {
                z_it->first = "";                                           // erase "using"
                z_it->second = 0;
                (z_it + 1)->first = "";                                     // erase " "
                (z_it + 1)->second = 0;
                (z_it + 2)->first = "";                                     // erase "System"
                (z_it + 2)->second = 0;
                (z_it + 3)->first = "";                                     // erase ";"
                (z_it + 3)->second = 0;
            }
            // FIX: Omit "namespace", project name, "{", and "}"
            if (z_it->first == "namespace") {                               // z_it->first: "namespace"
                z_it->first = "";                                           // erase "namespace"
                z_it->second = 0;
                (z_it + 1)->first = "";                                     // erase " "
                (z_it + 1)->second = 0;
                z_projectName = z_it->first;                                // Set the project name
                m_HSB.insert(0, "package " + z_projectName + ";");          // Set the package name
                (z_it + 2)->first = "";                                     // erase project name
                (z_it + 2)->second = 0;
                (z_it + 3)->first = "";                                     // erase " "
                (z_it + 3)->second = 0;
                (z_it + 4)->first = "";                                     // erase "{"
                (z_it + 4)->second = 0;
            }
            // FIX: class -> public class
            if (z_it->first == "class" && ( (z_it - 2)->first != "public"   ||  (z_it - 2)->first != "protected"    ||
                                            (z_it - 2)->first != "internal" ||  (z_it - 2)->first != "private")     &&
                                            z_it != m_outputVector.end()                                                ) {
                m_outputVector.insert(z_it, make_pair(" ", -110));
                m_outputVector.insert(z_it, make_pair("public", 50));
            }
            // FIX: Main method
            if (z_it->first == "main" || z_it->first == "Main") {
                // FIX: Main -> public main
                z_it2 = z_it;
                // Assign z_it2 to the last "{" before "Main"
                while (z_it2->first != "static" && z_it2 != m_outputVector.end()) {
                    --z_it2;
                }
                if ((z_it2 - 2)->first != "public") {
                    m_outputVector.insert(z_it2, make_pair(" ", -110));
                    m_outputVector.insert(z_it2, make_pair("public", 50));
                }
                // FIX: Main() -> Main(String[] args)
                if ((z_it + 2)->first == ")") {
                    (z_it + 2)->first = "String[] args)";
                    (z_it + 2)->second = 0;
                }
                // FIX: Omit returned values within main
                z_it2 = z_it;
                // Assign z_it2 to the first "{" after "Main"
                while (z_it2->first != "{" && z_it2 != m_outputVector.end()) {
                    ++z_it2;
                }
                bool temp = true;
                while ((z_it2->first != "}" || !temp) && z_it2 != m_outputVector.end()) {
                    if (z_it2->first == "{") {
                        temp = false;
                    }
                    if (z_it2->first == "}") {
                        temp = true;
                    }
                    if (z_it2->first == "return") {
                        int i;
                        for (i = 0; z_it2->first != ";"; i++) {
                            (z_it2 + i)->first = "";
                            (z_it2 + i)->second = 0;
                        }
                        (z_it2 + i)->first = "";
                        (z_it2 + i)->second = 0;
                    }
                }
                // FIX: int Main -> void main
                if ((z_it - 2)->first == "int") {
                    (z_it - 2)->first = "void";
                    (z_it - 2)->second = 75;
                }
            } // end FIX: Main method
        }

        for (z_it = m_outputVector.end(); z_it != m_outputVector.begin(); --z_it) {
            if (z_it->first == "}") {
                z_it->first = "";
                z_it->second = 0;
                break;
            }
        }

        for (z_it = m_outputVector.begin(); z_it != m_outputVector.end(); ++z_it) {
            translateStr(z_it->first, z_it->second);
        }
    }

    m_outputFileString = m_HSB + "\n" + m_BSB;
    cout << m_outputFileString;
}

bool Translator::found(string a_vectorStr, string a_str) {
    return a_vectorStr == a_str;
}

void Translator::translateStr(string a_str, int a_numCode) {
    // str is a keyword/operator
    if (a_numCode >= 0) {
        if (inMapping(a_str)) {
            m_BSB += m_outputBodyString(a_str);
            m_HSB += m_outputHeaderString(a_str);
        }
        else if (!inMapping(a_str)) {
            m_BSB += a_str;
        }
     // str is a literal
    } else if (a_numCode < 0) {
        string numCodeStr = to_string(a_numCode);
        if (inMapping(numCodeStr)) {
            m_BSB += m_outputBodyString(numCodeStr);
            m_HSB += m_outputHeaderString(numCodeStr);
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

string Translator::m_outputHeaderString(std::string a_str) {
    map<string, string> z_outputmap = m_Mapping.find(a_str)->second;
    return z_outputmap.find("head")->second;
}

string Translator::m_outputBodyString(std::string a_str) {
    map<string, string> z_outputMap = m_Mapping.find(a_str)->second;
    return z_outputMap.find("body")->second;
}
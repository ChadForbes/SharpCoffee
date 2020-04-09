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
LOOK INTO MAPPING:
Set the token value (z_TokenValue) to the current lexeme found in the keyword table (m_KeywordTable.find(m_CurrentLexeme))
 */

Translator::Translator(string outputFilePath = "JavaFiles\\HelloWorld.java", string mappingFilePath = "CSharp_to_Java_Mapping.json") {
    this->outputFilePath = outputFilePath;
    this->mappingFilePath = mappingFilePath;
    javaFileString = "";
    HSB = "";
    BSB = "";
    Scanner scanner("");
}

Translator::Translator() {
    outputFilePath = "JavaFiles\\HelloWorld.java";
    mappingFilePath = "CSharp_to_Java_Mapping.json";
    javaFileString = "";
    HSB = "";
    BSB = "";
    Scanner scanner("");

    Json::Value root;
    Json::Reader reader;
    ifstream ifs(mappingFilePath);
    reader.parse(ifs, root, false);
    Json::Value::Members popnam = root.getMemberNames();
    Json::Value::Members membername;

    for (string x : popnam) {
        membername = root[x].getMemberNames();
        m_Mapping.insert(make_pair(x, map<string, string>()));
        for (string y : membername) {
            m_Mapping[x].insert(make_pair(y, root[x][y].asString()));
        }
    }
}

void Translator::translate() {
    while(scanner.m_CurrentLexeme != "EOF") {
        int numCode = scanner.NextLexeme();
        string str = scanner.m_CurrentLexeme;
        translateStr(str, numCode);
    }
    javaString = HSB + "\n" + BSB;

    /*
    Call NextLexeme() first, then get m_CurrentLexeme;

    The NextLexeme() function is what the Translator will be calling.
    After each call to the NextLexeme() function the value of m_CurrentLexeme will be
    the string version of the most recently read token.

    */
}


void Translator::translateStr(string str, int numCode) {    
    // str is a keyword/operator
    if (numCode >= 0) {
        if (inMapping(str)) {
            BSB += toJavaString(str);
            getHeaderInfo(str);
        }
        else if (!inMapping(str)) {
            BSB += str;
        }
     // str is a literal
    } else if (numCode < 0) {
        string numCodeStr = to_string(numCode);
        if (inMapping(numCodeStr)) {
            BSB += toJavaString(str);
            getHeaderInfo(str);
        }
        else if (!inMapping(numCodeStr)) {
            BSB += str;
        }
    }
}

bool Translator::inMapping(string str) {
    if (m_Mapping.find(str) != m_Mapping.end()) {
        return true;
    }
    else return false;
}

/*
bool Translator::inMapping(int numCode) {
    string numCodeStr = to_string(numCode);
    return true;
}
*/

void Translator::getHeaderInfo(string str) {
    map<string, string> javaMap = m_Mapping.find(str)->second;
    HSB += javaMap.begin()->second;
}

string Translator::toJavaString(std::string str) {
    map<string, string> javaMap = m_Mapping.find(str)->second;

    return javaMap.begin()->first;
}

/*
Scanner("JavaFiles\\HelloWorld.java");
Scanner returns a STRING and a NUMERIC CODE
-> (STRING, NUMERIC CODE) i.e., "int 57"

public void Translate(string str, int numCode)
{
   if (numCode >= 0) // If the numeric code is 0 or greater
   {
        // The STRING is either a keyword or an operator.
       bool inMapping = SearchMapping(str); // Take the STRING and search the mapping
       if (inMapping == true) // If the string is found in the mapping
       {
           Append the Java version of the STRING to the
           body string builder
           Append any header info to the header string builder
       }
       else if (STRING is not found in the mapping)
       {
           Return the STRING to the body string builder
       }
   }
   if (the NUMERIC CODE is less than 0)
   {
       The STRING is a literal
       Take the NUMERIC CODE and search the mapping
       if (the NUMERIC CODE is found in the mapping)
       {
           Append the Java version of the STRING to the
           body string builder
           Append any header info to the header string builder
       }
       else if (the NUMERIC CODE is not found in the mapping)
       {
           Return the STRING to the body string builder
       }
   }
}
*/
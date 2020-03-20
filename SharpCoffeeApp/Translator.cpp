#include "Translator.h"
#include <iostream>
#include <fstream>

using namespace std;

/*
Strings: 
javaString
-Comprised of HSB and BSB
HSB (top)
-package, import statements
BSB (bottom)
-methods, functions
str
-Individual tokens that make up everything
 */

Translator::Translator(string inputFilePath) {
    //Scanner scanner(inputFilePath);
    javaString = "";
    HSB = "";
    BSB = "";
}

Translator::Translator() {
    javaString = "";
    HSB = "";
    BSB = "";
}


void Translator::translate(string str, int numCode) {
    /*
    Declare a Scanner(file path to C# file), javaString = HSB + BSB
    write entire javaString to the java file
    */
    if (numCode >= 0) {
        /* If the numeric code is 0 or greater, str is either a keyword or an
        operator.

        Take str and search the mapping. If str is found in the mapping
        (inMapping(str) returns true), append the Java version of str to
        BSB
        */
        if (inMapping(str)) {
            BSB += toJavaString(str) + " ";
            // if (there is header info) {
            //  HSB += toJavaString(strHeader);
            // }
        } // if str is not found in the mapping, just append it as is to the
          // BSB
        else if (!inMapping(str)) {
            BSB += str + " ";
        }
    } else if (numCode < 0) { // If numCode is less than 0, str is a literal
        if (inMapping(numCode)) {
            BSB += toJavaString(str) + " ";
            // if (there is header info) {
            //  HSB += toJavaString(strHeader);
            // }
        }
        else if (!inMapping(numCode)) {
            BSB += str + " ";
        }
    }
    
    /*
    */
    //writeToJavaFile(javaStr);
}

bool Translator::inMapping(string str) {
    return true;
}

bool Translator::inMapping(int numCode) {
    return true;
}

string Translator::toJavaString(std::string str) {
    return str;
}

/*
Scanner("JavaFiles\\HelloWorld.java");
Scanner returns a STRING and a NUMERIC CODE
-> (STRING, NUMERIC CODE) i.e., "int 57"

Append each line to the big string, then write the entire thing to a java
file

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
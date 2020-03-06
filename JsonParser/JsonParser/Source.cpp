#include <iostream>
#include <fstream>
#include <string>
#include<vector>
#include <json\json.h>// or jsoncpp/json.h , or json/json.h etc.

using namespace std;
Json::Reader reader;
Json::Value root;

map<string, map<string, string>> getCSharptoJavaParse() {

	// loading in the json
	ifstream ifs("C#_to_Java_Mapping.json");

	// parses the json into a document while ignoring comments
	reader.parse(ifs, root, false); 

	Json::Value::Members popnam = root.getMemberNames();
	Json::Value::Members membername;

	map<string, map<string, string>> z_maps;

	int i = 0;
	for (string x : popnam) {
		membername = root[x].getMemberNames();
		z_maps.insert(make_pair(x, map<string, string>()));
		for (string y : membername) {
			z_maps[x].insert(make_pair(y, root[x][y].asString()));
			//cout <<  << endl;

		}
		i++;

	}

	map<string, map<string, string>>::iterator itr;
	map<string, string>::iterator ptr;

	// print out the data stored in the map
	for (itr = z_maps.begin(); itr != z_maps.end(); itr++) {
  
        for (ptr = itr->second.begin(); ptr != itr->second.end(); ptr++) { 
            cout << "First key is '" << itr->first 
                 << "' And second key is '" << ptr->first 
                 << "' And value is '" << ptr->second << "'"<< endl; 
        } 
    } 
	cout << "Objects parsed: " << i << endl;

	return z_maps;
}

map <string, int> getKeywordsOperatorsCSharp() {

	// loading in the json
	ifstream ifs("KeywordsOperators_CSharp.json");

	// parses the json into a document while ignoring comments
	reader.parse(ifs, root, false);

	Json::Value::Members popnam = root.getMemberNames();

	map <string, int> z_keywordsoperators;
	int i = 0;
	

	for (string x : popnam) {
		z_keywordsoperators.insert(make_pair(x, root[x].asInt()));
		i++;
	}
	cout << popnam[0] << endl;

	map<string, int>::iterator ptr;

	for (ptr = z_keywordsoperators.begin(); ptr != z_keywordsoperators.end(); ptr++) {
		cout << "Key is '" << ptr->first
			<< "' And value is '" << ptr->second << "'" << endl;
	}
	cout << "Objects parsed: " << i << endl;

	return z_keywordsoperators;
}

vector<string> getInputClass() {

	// loading in the json
	ifstream ifs("InputClasses_CSharp.json");

	// parses the json into a document while ignoring comments
	reader.parse(ifs, root, false);

	vector<string> z_inputClasses;
	
	string result;
	Json::Value& inputs = root["CSharpInputClasses"];
	
	for(int i = 0; i < inputs.size(); i++){
		result = inputs[i].asString();
		z_inputClasses.push_back(result);
	}

	for (auto it = z_inputClasses.begin(); it != z_inputClasses.end(); it++)
		cout << *it << endl;

	return z_inputClasses;
}

vector<vector<vector<int>>> getStateTables() {

	// loading in the json
	ifstream ifs("ParserStateTables_CSharp.json");

	// parses the json into a document while ignoring comments
	reader.parse(ifs, root, false);

	int result;

	Json::Value& tables = root["ParserStateTables_CSharp"][0];

	vector<vector<vector<int>>> z_stateTable;

	for (int i = 0; i < tables.size(); i++) {
		result = tables[i].asInt();
		// z_stateTable.push_back(result);
		//cout << result << endl;

	}
	//cout << *it << endl;

	return z_stateTable;
}

int main(){

	// getCSharptoJavaParse();
	// getKeywordsOperatorsCSharp();
	// getInputClass();

	// currently not working
	// getStateTables();
	return 0;
}
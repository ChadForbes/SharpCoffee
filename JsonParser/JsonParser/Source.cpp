#include <iostream>
#include <fstream>
#include <string>
#include <json\json.h>// or jsoncpp/json.h , or json/json.h etc.

using namespace std;

map<string, map<string, string>> Getcpptojavaparse() {
	Json::Reader reader;
	Json::Value root;
	ifstream ifs("C#_to_Java_Mapping.json");
	reader.parse(ifs, root, false); // reader can also read strings
	Json::Value::Members popnam = root.getMemberNames();
	Json::Value::Members membername;
	//map<string, string> z_innermap;
	map<string, map<string, string>> z_maps;
	string v1;
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







int main(){

	Getcpptojavaparse();

	return 0;
}
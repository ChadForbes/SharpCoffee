#include "filereadstream.h"
#include "document.h"
#include <cstdio>

using namespace rapidjson;

int main() {

	FILE* fp = fopen("test.json", "r"); // non-Windows use "r"

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document d;
	d = d.ParseStream(is);

	assert(d.IsObject());
	assert(d.HasMember("hello"));
	assert(d["hello"].IsString());
	printf("hello = %s\n", d["hello"].GetString());
	fclose(fp);

	return 0;

}

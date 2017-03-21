#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class getFileInfo {
	
        public:
		getFileInfo(string = "default");

	private:
		string fileName;
		vector<string> allFiles;
		 
};

getFileInfo::getFileInfo (string s) {
	fileName = s;
	
	FILE *fp;
	char str[100];
	fp = fopen(fileName.c_str(), "r");
	while(fgets(str,100,fp)!= NULL){
		allFiles.push_back(str);
	}
	fclose(fp);
}

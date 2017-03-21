#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

template <typename T>
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
	fp = fopen(fileName,"r");
	while(fgets(str,100,fp)!= NULL){
		allFiles.push_back(str);
	}
	fclose(fp);
}

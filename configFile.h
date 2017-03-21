#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

template <typename T>
class configFile {
	
	public:
		configFile(string = "default");
		int get_PERIOD_FETCH();
		int get_NUM_FETCH();
		int get_NUM_PARSE();
		string get_SEARCH_FILE();
		string get_SITE_FILE();

	private:
		int PERIOD_FETCH = 180;
		int NUM_FETCH = 1;
		int NUM_PARSE = 1;
		string SEARCH_FILE = "Search.txt";
		string SITE_FILE = "Sites.txt";
		vector<string> allFiles;
		 
};

getFileInfo::getFileInfo (string s) {
	
}



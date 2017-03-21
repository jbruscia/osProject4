#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class configFile {
	
	public:
		configFile(string = "default");
		int get_PERIOD_FETCH();
		int get_NUM_FETCH();
		int get_NUM_PARSE();
		string get_SEARCH_FILE();
		string get_SITE_FILE();

	private:
		int PERIOD_FETCH;
		int NUM_FETCH;
		int NUM_PARSE;
		string SEARCH_FILE;
		string SITE_FILE;
		vector<string> allFiles;
		 
};

configFile::configFile (string s) {
        PERIOD_FETCH = 180;
        NUM_FETCH = 1;
        NUM_PARSE = 1;
        SEARCH_FILE = "Search.txt";
        SITE_FILE = "Sites.txt";
	
}



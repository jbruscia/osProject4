#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

class configFile {
	
	public:
		configFile(string = "Config.txt");
		int get_PERIOD_FETCH(){return PERIOD_FETCH;}
		int get_NUM_FETCH(){return NUM_FETCH;}
		int get_NUM_PARSE(){return NUM_PARSE;}
		string get_SEARCH_FILE(){return SEARCH_FILE;}
		string get_SITE_FILE(){return SITE_FILE;}

	private:
		int PERIOD_FETCH;
		int NUM_FETCH;
		int NUM_PARSE;
		string SEARCH_FILE;
		string SITE_FILE;
		vector<string> allFiles;
		string fileName;
                //commetn
		 
};

configFile::configFile (string s) {
        PERIOD_FETCH = 180;
        NUM_FETCH = 1;
        NUM_PARSE = 1;
        SEARCH_FILE = "Search.txt";
        SITE_FILE = "Sites.txt";
        
        fileName = s;
        /*FILE *fp;
		char str[100];
		fp = fopen(fileName.c_str(), "r");
		while(fgets(str,100,fp)!= NULL){
			if(strncmp("PERIOD_FETCH",str,12) == 0) {
				char str2[10];
				strncpy(str2, str+12, sizeof(str2));
				PERIOD_FETCH = atoi(str2);
			} else if(strncmp("NUM_FETCH", str, 9) == 0) {
				char str2[10];
				strncpy(str2, str+9, sizeof(str2));
				NUM_FETCH = atoi(str2);
			} else if(strncmp("NUM_PARSE", str, 9) == 0) {
				char str2[10];
				strncpy(str2, str+9, sizeof(str2));
				NUM_PARSE = atoi(str2);
			} else if(strncmp("SEARCH_FILE", str, 11) == 0) {
				SEARCH_FILE = str+12;
			} else if(strncmp("SITE_FILE", str, 9) == 0) {
				SITE_FILE = str+10;
			}else {
				//error handling
			}
		}
		fclose(fp);
		*/
		string tmp;
		ifstream myfile(fileName.c_str());
		if(myfile.good()){
		    while(getline(myfile, tmp)){
		        if (tmp.compare(0,12,"PERIOD_FETCH") == 0) {
		        	PERIOD_FETCH = atoi(tmp.substr(13).c_str());
		        } else if (tmp.compare(0,9,"NUM_FETCH") == 0) {
		        	NUM_FETCH = atoi(tmp.substr(10).c_str());
		        } else if ( tmp.compare(0,9,"NUM_PARSE") == 0) {
		        	NUM_PARSE = atoi(tmp.substr(10).c_str());
		        } else if (tmp.compare(0,11,"SEARCH_FILE") == 0) {
		        	SEARCH_FILE = tmp.substr(12);
		        } else if (tmp.compare(0,9,"SITE_FILE") == 0) {
		        	SITE_FILE = tmp.substr(10);
		        } else {
		        
		        }
		    }
		}
		myfile.close();
	
}




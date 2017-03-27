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
    string tmp;
    ifstream myfile(fileName.c_str());
    if(myfile.good()){
        while(getline(myfile, tmp)){
            if (tmp.compare(0,12,"PERIOD_FETCH") == 0) {
                try {
                    PERIOD_FETCH = stoi(tmp.substr(13));
                } catch(exception const & e) {
                    cout << "error on PERIOD_FETCH: improper input" << endl;
                }
            } else if (tmp.compare(0,9,"NUM_FETCH") == 0) {
                try {
                    NUM_FETCH = stoi(tmp.substr(10));
                } catch(exception const & e) {
                    cout << "error on NUM_FETCH: improper input" << endl;
                }
            } else if ( tmp.compare(0,9,"NUM_PARSE") == 0) {
                try {
                    NUM_PARSE = stoi(tmp.substr(10));
                } catch (exception const & e) {
                    cout << "error on NUM_PARSE: improper input" << endl;
                }
            } else if (tmp.compare(0,11,"SEARCH_FILE") == 0) {
                SEARCH_FILE = tmp.substr(12);
            } else if (tmp.compare(0,9,"SITE_FILE") == 0) {
                SITE_FILE = tmp.substr(10);
            } else {
                cout << "Unknown parameter: " << tmp << endl; 
            }
        }
    }
    //fix values that might be too high or low; they should be in [1-8]
    if(NUM_FETCH > 8){
        NUM_FETCH = 8;
    } else if(NUM_FETCH < 1){
        NUM_FETCH = 1;
    }
    if(NUM_PARSE > 8){
        NUM_PARSE = 8;
    } else if(NUM_PARSE < 1){
        NUM_PARSE = 1;
    }
    if(PERIOD_FETCH < 1){
        PERIOD_FETCH = 180;
    }
    myfile.close();

}




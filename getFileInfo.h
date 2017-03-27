#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <deque>


using namespace std;

class getFileInfo {
	
    public:
		getFileInfo(string = "default");
		deque<string> getDeque(){return allFiles;}

	private:
		string fileName;
		deque<string> allFiles;
		 
};

getFileInfo::getFileInfo (string s) {
	fileName = s;
       /* 	
	FILE *fp;
	char str[100];
	fp = fopen(fileName.c_str(), "r");
	while(fgets(str,100,fp)!= NULL){
		allFiles.push_back(str);
	}
	fclose(fp);
        */
        string tmp;
        ifstream myfile(fileName.c_str());
        if(myfile.good()){
                while(getline(myfile, tmp)){
                        allFiles.push_back(tmp);
                }
        } else {
                cout << "File cannot be found: " << fileName << endl;
                exit(1);
        }
        myfile.close();
}

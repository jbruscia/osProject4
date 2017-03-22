#include <stdio.h>
#include <iostream>
#include "configFile.h"
#include "getFileInfo.h"
#include <string.h>

using namespace std;

int countPhrase(string page, string phrase) {
	int found;
	int count = 0;
	found = page.find(phrase);
	//cout << found << endl;
	while(found != -1) {
		count++;
		found = page.find(phrase, found+phrase.size());
		//cout << found << endl;
	}
	return count;
}

int main(){
    //configFile configTmp;
    //getFileInfo getTmp;
    
    cout << countPhrase("Thlskdfjkdlkitkdkd", "kd") << endl;
    
    
}

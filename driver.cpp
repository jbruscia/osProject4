#include <stdio.h>
#include <iostream>
#include "configFile.h"
#include "getFileInfo.h"
#include "Curl.h"

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
   Curl c;
   string s = "dad";
   string t = "d";
   //cout << c.fetch("http://nd.edu") << endl;
   configFile conf;
   cout << "serach file: " << conf.get_SEARCH_FILE() << endl;
   cout << "site: " << conf.get_SITE_FILE() << endl;
   string searchFile = conf.get_SEARCH_FILE();
   getFileInfo search(searchFile);
   getFileInfo site(conf.get_SITE_FILE());
   cout << site.getVector()[0] << endl;
   s = c.fetch(site.getVector()[0]);
   //t = search.getVector()[0];
   cout << countPhrase(s,t) << endl;

   
}

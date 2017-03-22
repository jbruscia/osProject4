#include <stdio.h>
#include <iostream>
#include "configFile.h"
#include "getFileInfo.h"
#include "Curl.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>



using namespace std;

char bKeepLooping = 1;
char bKeepLooping2 = 1;
int countPhrase(string page, string phrase);
void runThreading(configFile conf);
void handler(int a) {
	bKeepLooping = 0;
	bKeepLooping2 = 0;
}
void handler2(int a) {
	bKeepLooping2 = 0;
}

int main(){
	configFile conf;
	cout << "search file: " << conf.get_SEARCH_FILE() << endl;
	cout << "site file: " << conf.get_SITE_FILE() << endl;
	
	alarm(conf.get_PERIOD_FETCH());
	int period = conf.get_PERIOD_FETCH();
	signal(SIGINT, handler);
	signal(SIGALRM, handler2);
	while (bKeepLooping) {
		while (bKeepLooping2) {
			
		}
		alarm(period);
		bKeepLooping2 = 1;
		runThreading(conf);
	}

	return 0;
}

void runThreading(configFile conf) {
	Curl c;
	string s; string t;
	getFileInfo search(conf.get_SEARCH_FILE());
	getFileInfo site(conf.get_SITE_FILE());
	
	
	
	
	cout << site.getVector()[0] << endl;
	s = c.fetch(site.getVector()[0]);
	t = search.getVector()[0];
	cout << countPhrase(s,t) << endl;
	
}

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

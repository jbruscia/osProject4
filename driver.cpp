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
void * threadFetch(void * pData);
void * threadParse(void * pData);
deque<string> * fetchQueue;
deque<string> * parseQueue; //should this be a map so you can save the site too?

//pdata struct should have the queues


int main(){
	configFile conf;
	cout << "search file: " << conf.get_SEARCH_FILE() << endl;
	cout << "site file: " << conf.get_SITE_FILE() << endl;
	
	
	alarm(1);
	int period = conf.get_PERIOD_FETCH();
	signal(SIGINT, handler);
	//need a sighup one too? one above does control c i believe
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
	
	//need to populate the fetch queue with safety, how to do this?
	//*fetchQueue = site.getDeque();
	
	
	
	cout << site.getDeque()[0] << endl;
	s = c.fetch(site.getDeque()[0]);
	t = search.getDeque()[0];
	//cout << "s: " << s << endl;
	cout << countPhrase(s,t) << endl;
	
	
	//make threads
	//call threadFetch on the threads
	//call threadParse on the threads
}


void * threadFetch(void * pData) {
	/*
	while (1) {
		//lock Mutex
		while (fetchQueue.getCount() == 0) {
			pthread_cond_wait(mutex,cond_var);
		}
		string s = fetchQueue.pop();
		//unlock fetchque mutex
		string raw = [curl object].fetch([vector]);
		//lock parse queue
		//put data/work item in parse queue
	}
	//unlock parse queue(signal or bcast)
	*/
}

void * threadParse(void * pData) {
	//lock parse queue
	//pop the top fetch data
	//unlock parse que and signal?
	//call countPhrase
	//write to file (need date/time, phrase, site, and count)
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

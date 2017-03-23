#include <stdio.h>
#include <iostream>
#include "configFile.h"
#include "getFileInfo.h"
#include "Curl.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>



using namespace std;


struct parseInfo {
	string pageData;
	string time;
	string siteURL;
};
char bKeepLooping = 1;
int countPhrase(string page, string phrase);
void populateFetch(int a);
void handler(int a) {
	bKeepLooping = 0;
}
void * threadFetch(void * pData);
void * threadParse(void * pData);
deque<string> * fetchQueue;
deque<string> * fullDeque;
deque<parseInfo> * parseQueue; //should this be a map so you can save the site too?
configFile conf;


//pdata struct should have the queues


int main(){
	cout << "search file: " << conf.get_SEARCH_FILE() << endl;
	cout << "site file: " << conf.get_SITE_FILE() << endl;
	
	int count = 0;
	alarm(1);
	signal(SIGINT, handler);
	//need a sighup one too? one above does control c i believe
	signal(SIGALRM, populateFetch); //reload fetch queue with sites
	
	getFileInfo search(conf.get_SEARCH_FILE());
	getFileInfo site(conf.get_SITE_FILE());
	*fullDeque = site.getDeque();
	//thread creation here or in while loop?
	while (bKeepLooping) {
		count++;
		Curl c;
		string raw; string phr;
		/*
		cout << site.getDeque()[0] << endl;
		raw = c.fetch(site.getDeque()[0]);
		phr = search.getDeque()[0];
		cout << countPhrase(raw,phr) << endl;
		*/
		
		//create output file (iteration number)
		//if(count == 1) make threads
		//call threadFetch on the threads
		//call threadParse on the threads
			//should pass in a struct that has vector of phrases
			
		//join threads
		//once all have joined, then signal alarm 
		//clean up mallocs
		//how to know when you have finished all tasks you need?
		alarm(conf.get_PERIOD_FETCH());
		
		//should you be joining and making new threads each time, or using same ones
		
	}

	return 0;
}

void populateFetch(int a) {
	//need to populate the fetch deque with safety, how to do this?
	*fetchQueue = *fullDeque;
	//signal producesrs
}


void * threadFetch(void * pData) {
	/*
	while (1 or gkeeprunning) {
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
	
	//Q:should this be running continuously??
}

void * threadParse(void * pData) {
	//lock parse queue
	//pop the top fetch data
	//unlock parse que and signal?
	//call countPhrases for each phrase in phrases
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



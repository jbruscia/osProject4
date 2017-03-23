#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include "configFile.h"
#include "getFileInfo.h"
#include "Curl.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <deque>




using namespace std;


struct parseInfo {
	string pageData;
	string time;
	string siteURL;
};
char bKeepLooping = 1;
char tKeepLooping = 1;//can all these be the same???
char pKeepLooping = 1;
int countPhrase(string page, string phrase);
void printToFile(string &);
void populateFetch(int a);
void handler(int a) {
	bKeepLooping = 0;
	//need to unwind threads
	//for loop, wake up threads and join
}
void * threadFetch(void * pData);
void * threadParse(void * pData);
void * threadPrint(void * pData);
int batch_count = 1;
int batch_total = 0;
int curr_batch_count = 0;
string batch_data = "";
deque<string>  fetchQueue;
deque<string>  fullDeque;
deque<string>  allPhrases;
deque<parseInfo> parseQueue;
configFile conf;

int waiting = 1;
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
	
	fullDeque = site.getDeque();
	allPhrases = search.getDeque();
	
	//thread creation here or in while loop?
	int num_threads = 3;
	//should be safely done
	batch_total = (search.getDeque().size())*(site.getDeque().size());
	pthread_t *ptr = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
	int rc, rf, rq;
	int x = 0;
	rc = pthread_create(&ptr[0], NULL, threadFetch, NULL);
	rf = pthread_create(&ptr[1], NULL, threadParse, NULL);
	rq = pthread_create(&ptr[2], NULL, threadPrint, NULL);
	//rd = pthread_create(&ptr[2], NULL, (void*)threadParse, &numPrints);
	
	
	//fetchQueue = fullDeque;
	while (bKeepLooping) {
		waiting = 1;
		count++;
		//string raw; string phr;
		/*
		cout << site.getDeque()[0] << endl;
		raw = c.fetch(site.getDeque()[0]);
		phr = search.getDeque()[0];
		cout << countPhrase(raw,phr) << endl;
		*/
		
		//create output file (iteration number)
		while (waiting) {
			
		}
		alarm(conf.get_PERIOD_FETCH());
		cout << "here tho" << endl;
	}
	//join threads, wake up sleeping threads, etc
	free(ptr);
	return 0;
}

void populateFetch(int a) {
	//need to populate the fetch deque with safety, how to do this?
	cout << "in here" << endl;
	fetchQueue = fullDeque;
	
	//signal producesrs
}


void * threadFetch(void * pData) {
	
	while (tKeepLooping) {
		//lock Mutex
		while (fetchQueue.size() == 0) {
			//pthread_cond_wait(mutex,cond_var);
		}
		string s = fetchQueue[0];
		fetchQueue.pop_front();
		//unlock fetchque mutex
		Curl c;
		string raw = c.fetch(s);
		string time = "time to fam";
		string siteURL = s;
		parseInfo p = {raw,time,siteURL};
		parseQueue.push_back(p);
		//lock parse queue
		//put data/work item in parse queue
	}
	//unlock parse queue(signal or bcast)
	
	
	//Q:should this be running continuously??
}

void * threadParse(void * pData) {
        time_t rawtime;
        struct tm * timeinfo;
        char buf[80];
		int pcount = 0;
	while (pKeepLooping) {
		while (parseQueue.size() == 0) {
			
		}
		parseInfo p = parseQueue[0];
		parseQueue.pop_front();
		for (int i = 0; i < allPhrases.size(); i += 1) {
			pcount = countPhrase(p.pageData, allPhrases[i]);
		        time(&rawtime);
		        timeinfo = localtime(&rawtime);
		        strftime(buf, 80, "%I:%M:%S%p", timeinfo);
		        batch_data.append(buf).append(",").append(allPhrases[i]).append(",").append(p.siteURL).append(",").append(to_string(pcount)).append("\n");
				//cout << p.time << ", " << allPhrases[i] << ", " << p.siteURL << ", " << pcount << endl;
		        //cout << batch_data;
		        //batch_data = "";
		        //printToFile(batch_data);
		        curr_batch_count++;
		}
		waiting = 0;
	}
}

void * threadPrint(void * pData) {
	//cond variable
	while (pKeepLooping) {
		
		
		while (curr_batch_count < batch_total) {
	
		}
		printToFile(batch_data);
		curr_batch_count = 0;
	}
}

void printToFile(string &s){
    string fileName = to_string(batch_count).append(".csv");
    ofstream outputFile(fileName);
    outputFile << s;
    outputFile.close();
    s = "";
    batch_count++;
    cout << "written!" << endl;
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



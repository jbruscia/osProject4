#include <stdio.h>
#include <regex>
#include <sstream>
#include <algorithm>
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

//mutexes
pthread_mutex_t fetchlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t parselock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printlock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t fetchcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t parsecond = PTHREAD_COND_INITIALIZER;
pthread_cond_t printcond = PTHREAD_COND_INITIALIZER;

//what goes into parse queue
struct parseInfo {
    string pageData;
    string time;
    string siteURL;
};
//parameters for thread loops
char bKeepLooping = 1;
char tKeepLooping = 1;
char pKeepLooping = 1;
//counting occurunce of phrase function
int countPhrase(string page, string phrase);
void printToFile(string &);
void populateFetch(int a);
//thread function declarations
void * threadFetch(void * pData);
void * threadParse(void * pData);
void * threadPrint(void * pData);
string getBody(string);
int batch_count = 1; //tells which batch it is to create correct file
int batch_total = 0; //how many entries you should have in each file
int curr_batch_count = 0; // how many current entries have been stored in string
int done_printing = 1; //when printing has finished
int ex = 0; // handler changes this to make sure all threads can exit and aren't stuck in while loops
string batch_data = "time,phrase,site,count\n"; //string that gets added to file
deque<string>  fetchQueue; //queue of urls
deque<string>  fullDeque; //the full queue of urls that fetchQueue gets rest to for new batch
deque<string>  allPhrases; //queue of all phrases
deque<parseInfo> parseQueue; //queue of text from website

configFile conf;

void handler(int a) {
    //tell all active threads to exit	
    pKeepLooping = 0;
    tKeepLooping = 0;
    bKeepLooping = 0;
    done_printing = 0;
    //skip wait time to begin exit procedure
    ex = 1;
    alarm(1);
}

int main(){


    alarm(1); //populate parse queue for the first time to initialize program
    signal(SIGINT, handler); //exit with control-c
    signal(SIGALRM, populateFetch); //reload fetch queue with sites

    getFileInfo search(conf.get_SEARCH_FILE()); //create queue of phrases
    getFileInfo site(conf.get_SITE_FILE()); //create queue of sites
	//populate default deques
    fullDeque = site.getDeque();
    allPhrases = search.getDeque();
	
	//get how many entries should be in each file
    batch_total = (search.getDeque().size())*(site.getDeque().size());
    //if there are no entries, output error and exit 
    if(batch_total < 1) {
    	cout << "There is nothing to be done! Please adjust the Site file and/or Search file" << endl;
    	exit(1);
    }
    
    //declare threads
    pthread_t *fetchers = (pthread_t*)malloc(sizeof(pthread_t)*conf.get_NUM_FETCH());
    pthread_t *parsers = (pthread_t*)malloc(sizeof(pthread_t)*conf.get_NUM_PARSE());
    pthread_t *printer = (pthread_t*)malloc(sizeof(pthread_t));
    
    //check to see if there are more threads than there are sites, cap if need be
    int maxThreadsFetch = conf.get_NUM_FETCH();
    int maxThreadsParse = conf.get_NUM_PARSE();
    if(maxThreadsFetch > (int)site.getDeque().size()) maxThreadsFetch = site.getDeque().size();
	if(maxThreadsParse > (int)site.getDeque().size()) maxThreadsParse = site.getDeque().size();
    
	//create threads
    int rc, rf, rq;
    //FETCH THREAD CREATIONS
    for(int i = 0; i < maxThreadsFetch; i++){
        rc = pthread_create(&fetchers[i], NULL, threadFetch, NULL);
        //error checking
        if(rc < 0){
            cout << "threading for the fetchers failed" << endl;
            exit(1);
        }
    }
    //PARSE THREAD CREATIONS
    for(int j = 0; j < maxThreadsParse; j++){
        rf = pthread_create(&parsers[j], NULL, threadParse, NULL);
        if(rf < 0){
            cout << "threading for the parsers failed" << endl; 
            exit(1);
        }
    }
	//PRINT THREAD CREATION
    rq = pthread_create(&printer[0], NULL, threadPrint, NULL);
    if(rq < 0){
        cout << "threading failed for printing" << endl;
        exit(1);
    }
    
    //main program loop
    while (bKeepLooping) {
        done_printing = 1;
        //wait for previous batch to finish before resetting alarm
        while (done_printing) {

        }
        if(!ex) {
        	alarm(conf.get_PERIOD_FETCH()); //reset alarm if not in exit procedure
        	cout << "Batch completed! New file created and alarm reset." << endl;
        }
        else {
        	cout << "Ending program" << endl;
        }
    }
    
    //have finished main program loop, begin exit
    //exit gracefully by joining threads
    for(int i = 0; i < maxThreadsFetch; i++){
        rc = pthread_join(fetchers[i], NULL);
        //error checking
        if(rc < 0){
            cout << "joining for the fetchers failed" << endl;
            exit(1);
        }
    }
    for(int j = 0; j < maxThreadsParse; j++){
        rf = pthread_join(parsers[j], NULL);
        //error checking
        if(rf < 0){
            cout << "joining for the parsers failed" << endl; 
            exit(1);
        }
    }
    rq = pthread_join(printer[0], NULL);
    if(rq < 0){
        cout << "joining failed for printing" << endl;
        exit(1);
    }
    
    //free memory
    free(parsers);
    free(fetchers);
    free(printer);
    return 0;
}

//additional functions
//function called at alarm that resets parsequeue
void populateFetch(int a) {
    pthread_mutex_lock(&fetchlock);
    fetchQueue = fullDeque;
    if(!ex) cout << "New batch started" << endl;
    pthread_cond_signal(&fetchcond);
    pthread_mutex_unlock(&fetchlock);

}


void * threadFetch(void * pData) {
    while (tKeepLooping) {
        pthread_mutex_lock(&fetchlock);
        while (fetchQueue.size() == 0) {
            pthread_cond_wait(&fetchcond,&fetchlock);
        }
        string s = fetchQueue[0]; //get top item
        fetchQueue.pop_front(); //remove that item
        pthread_cond_signal(&fetchcond);
        pthread_mutex_unlock(&fetchlock);
        Curl c; //curl page dat from site
        string raw = c.fetch(s);
        //handle page data
        if(s != ""){
            string time = "time to fam";
            string siteURL = s;

            parseInfo p = {raw,time,siteURL};
            pthread_mutex_lock(&parselock);
            parseQueue.push_back(p); //add struct to queue
            pthread_cond_signal(&parsecond);
            pthread_mutex_unlock(&parselock);
        }
    }

    return 0;
}

void * threadParse(void * pData) {
    time_t rawtime;
    struct tm * timeinfo;
    char buf[80];
    int pcount = 0;
    while (pKeepLooping) {
        pthread_mutex_lock(&parselock);
        while (parseQueue.size() == 0) {
            pthread_cond_wait(&parsecond,&parselock);
        }
        parseInfo p = parseQueue[0];
        parseQueue.pop_front();
        pthread_cond_signal(&parsecond);
        pthread_mutex_unlock(&parselock);
        //check each phrase on the site data
        for (int i = 0; i < (int)allPhrases.size(); i += 1) {
            pcount = countPhrase(p.pageData, allPhrases[i]); //get the number of occurrences
            //acquire rest of file entry data
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(buf, 80, "%I:%M:%S%p", timeinfo);
            pthread_mutex_lock(&printlock);
            //add to string
            batch_data.append(buf).append(",").append(allPhrases[i]).append(",").append(p.siteURL).append(",").append(to_string(pcount)).append("\n");
            
            curr_batch_count++; //signal that batch has been increased
            pthread_cond_signal(&printcond);
            pthread_mutex_unlock(&printlock);
        }
    }
    return 0;
}

//thread that waits on other threads to finish batch
void * threadPrint(void * pData) {
    while (pKeepLooping) {
        pthread_mutex_lock(&printlock);
        //print when all required batch entries have been added to string
        while ((curr_batch_count < batch_total) && ex == 0 ) {
            pthread_cond_wait(&printcond, &printlock);
        }
        //lock for done_printing
        done_printing = 0;
        //unlock for done_printing
        if(!ex) printToFile(batch_data);
        curr_batch_count = 0; //reset batch count
        pthread_cond_signal(&printcond);
        pthread_mutex_unlock(&printlock);
    }
    return 0;
}

//add all batch info to file at one time
void printToFile(string &s){
    string fileName = to_string(batch_count).append(".csv");
    ofstream outputFile(fileName);
    outputFile << s;
    outputFile.close();
    s = "time,phrase,site,count\n";
    batch_count++;
}

string getBody(string html){
    vector<string> res;
    int end = 0, begin = 0;
    while(begin != -1 ){
        begin = html.find("<body");
        end = html.find("</body>");
        if(begin != -1 && end != -1){
            res.push_back(html.substr(begin, end));
        }
        html = html.substr(end);
    }
    string body = "", result = "";
    for(auto str: res){
        body.append(str);
    }
    res.clear();
    begin = body.find(">");
    end = 0;
    if(begin != -1){
        while(end != -1){
            body = body.substr(begin);
            end = body.find("<");
            if(end != -1){
                res.push_back(body.substr(0, end));
                body = body.substr(end);
            }
            begin = body.find(">");
        }
        for(auto str: res){
            result.append(str);
        }

    } else {
        return body;
    }
    return result;
}

//function to count number of occurrences of phrase in larger string
int countPhrase(string page, string phrase) {
    page = getBody(page);
    int found;
    int count = 0;
    found = page.find(phrase);
    while(found != -1) {
        count++;
        found = page.find(phrase, found+1);
    }
    return count;
}


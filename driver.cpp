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

//mutexes
pthread_mutex_t fetchlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t parselock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printlock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t fetchcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t parsecond = PTHREAD_COND_INITIALIZER;
pthread_cond_t printcond = PTHREAD_COND_INITIALIZER;

struct parseInfo {
    string pageData;
    string time;
    string siteURL;
};
char bKeepLooping = 1;
char tKeepLooping = 1;//can all these be the same???
char pKeepLooping = 1;
char xKeepLooping = 1;
int countPhrase(string page, string phrase);
void printToFile(string &);
void populateFetch(int a);
void * threadFetch(void * pData);
void * threadParse(void * pData);
void * threadPrint(void * pData);
int batch_count = 1;
int batch_total = 0;
int curr_batch_count = 0;
int done_printing = 0;
string batch_data = "";
deque<string>  fetchQueue;
deque<string>  fullDeque;
deque<string>  allPhrases;
deque<parseInfo> parseQueue;
configFile conf;

void handler(int a) {
    pKeepLooping = 0;
    tKeepLooping = 0;
    bKeepLooping = 0;
    done_printing = 0;
    //need to unwind threads
    //for loop, wake up threads and join
}

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

    //should be safely done
    batch_total = (search.getDeque().size())*(site.getDeque().size());
    pthread_t *fetchers = (pthread_t*)malloc(sizeof(pthread_t)*conf.get_NUM_FETCH());
    pthread_t *parsers = (pthread_t*)malloc(sizeof(pthread_t)*conf.get_NUM_PARSE());
    pthread_t *printer = (pthread_t*)malloc(sizeof(pthread_t));

    int rc, rf, rq;
    //FETCH THREAD CREATIONS
    for(int i = 0; i < conf.get_NUM_FETCH(); i++){
        rc = pthread_create(&fetchers[i], NULL, threadFetch, NULL);
        //error checking insert here
        if(rc < 0){
            cout << "threading for the fetchers failed" << endl;
            exit(1);
        }
    }
    //PARSE THREAD CREATIONS
    for(int j = 0; j < conf.get_NUM_PARSE(); j++){
        rf = pthread_create(&parsers[j], NULL, threadParse, NULL);
        if(rf < 0){
            cout << "threading for the parsers failed" << endl; 
            exit(1);
        }
    }

    rq = pthread_create(&printer[0], NULL, threadPrint, NULL);
    if(rq < 0){
        cout << "threading failed for printing" << endl;
        exit(1);
    }

    fetchQueue = fullDeque;
    while (bKeepLooping) {
        done_printing = 1;
        while (done_printing) {

        }
        alarm(conf.get_PERIOD_FETCH());
        cout << "reset alarm" << endl;
    }
    //join threads, wake up sleeping threads, etc
    free(parsers);
    free(fetchers);
    free(printer);
    return 0;
}

void populateFetch(int a) {
    cout << "in here" << endl;
    pthread_mutex_lock(&fetchlock);
    //while(xKeepLooping) {
    //	pthread_cond_wait(&fetchcond, &fetchlock);
    //}
    //pthread_cond_wait(&fetchcond, &fetchlock);
    //xKeepLooping = 1;
    fetchQueue = fullDeque;
    pthread_cond_signal(&fetchcond);
    pthread_mutex_unlock(&fetchlock);

}


void * threadFetch(void * pData) {

    while (tKeepLooping) {
        pthread_mutex_lock(&fetchlock);
        while (fetchQueue.size() == 0) {
            pthread_cond_wait(&fetchcond,&fetchlock);
        }
        string s = fetchQueue[0];
        fetchQueue.pop_front();
        //xKeepLooping = 0;
        pthread_cond_signal(&fetchcond);
        pthread_mutex_unlock(&fetchlock);
        Curl c;
        string raw = c.fetch(s);
        if(s != ""){
            string time = "time to fam";
            string siteURL = s;

            parseInfo p = {raw,time,siteURL};
            pthread_mutex_lock(&parselock);
            parseQueue.push_back(p);
            pthread_cond_signal(&parsecond);
            pthread_mutex_unlock(&parselock);
        }
    }


    //Q:should this be running continuously??
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
        for (int i = 0; i < allPhrases.size(); i += 1) {
            pcount = countPhrase(p.pageData, allPhrases[i]);
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(buf, 80, "%I:%M:%S%p", timeinfo);
            pthread_mutex_lock(&printlock);
            batch_data.append(buf).append(",").append(allPhrases[i]).append(",").append(p.siteURL).append(",").append(to_string(pcount)).append("\n");
            //lock batch count
            //while(1){
            //cond wait
            //}
            curr_batch_count++;
            pthread_cond_signal(&printcond);
            pthread_mutex_unlock(&printlock);
        }
        waiting = 0;
    }
    return 0;
}

void * threadPrint(void * pData) {
    while (pKeepLooping) {
        pthread_mutex_lock(&printlock);
        while (curr_batch_count < batch_total) {
            pthread_cond_wait(&printcond, &printlock);
        }
        //lock for done_printing
        done_printing = 0;
        //unlock for done_printing
        printToFile(batch_data);
        curr_batch_count = 0;
        pthread_mutex_unlock(&printlock);
    }
    return 0;
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
    while(found != -1) {
        count++;
        found = page.find(phrase, found+phrase.size());
    }
    return count;
}



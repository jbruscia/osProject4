#include <stdio.h>
#include <cstring>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <curl/curl.h>
using namespace std;

//add timeout if you can't get website data
//don't exit if bad website

static size_t writeToMemory(void *contents, size_t size, size_t nmemb, void *userp);
struct Chunk {
    char *mem;
    size_t size;
};

class Curl {
    public:
        Curl(){};
        string fetch(string);
    private:
};

static size_t writeToMemory(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    struct Chunk *mem = (struct Chunk *)userp;
    mem->mem = (char*)realloc(mem->mem, mem->size + realsize + 1);
    if(mem->mem == NULL){
        cout << "there was not enough memory available: realloc failed\n";
        return 0;
    }
    memcpy(&(mem->mem[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->mem[mem->size] = 0;
    return realsize;
}

string Curl::fetch(string site){
    CURL *curl_handle;
    CURLcode res;
    struct Chunk chunk;
    chunk.mem = (char*)malloc(1);
    chunk.size = 0;
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, site.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeToMemory);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK){
        cout << "libcurl failed on the following site: " << site << endl;
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return "";
    } else {
        cout << "it might have worked" << endl;
    }
    cout << "bytes received: " << chunk.size << endl;
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return chunk.mem;
}







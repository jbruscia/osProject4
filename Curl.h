#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <curl/curl.h>
using namespace std;

struct Chunk {
        char *mem;
        size_t size;
};

class Curl {
        public:
                Curl();
                string fetch(string);
        private:
};








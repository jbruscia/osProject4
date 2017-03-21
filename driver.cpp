#include <stdio.h>
#include <iostream>
//#include "configFile.h"
//#include "getFileInfo.h"
#include "Curl.h"

using namespace std;

int main(){
   Curl c;
   cout << c.fetch("http://nd.edu") << endl;
    
    
}

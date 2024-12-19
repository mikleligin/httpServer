#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <unordered_map>
#include "server.h"

using namespace std;
std::mutex log_mutex;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cerr<<"Usage <port>\n";
        exit(1);
    }
    int sPort = stoi(argv[1]);
    Server server(sPort);
    server.run();
    return 0;
}
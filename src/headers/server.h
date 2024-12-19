#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <unordered_map>
#include "json.hpp"

using namespace std;

extern std::mutex log_mutex;

class Server {
    public:
        Server(int port) : serverPort(port){};
        void run();
        static std::string hours;
        static std::string mins;
        static std::string sec;

    private:
        int serverPort = 0;
        void getQuery(int socket, const string& request);
        void postQuery(int socket, const string& request, string fullReq);
        void handleClient(int socket);
        unordered_map<string, string> parseCredentials(const string& data);
        string getRequestBody(const string& request);
        string getPostCreds(const string& body, const string& paramName);
        string getRequestedPage(const string& request);
        void sendErrorResponse(int socket, const string& errorMessage);
};

#endif // SERVER_H

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

using namespace std;

mutex log_mutex;
class Server{
    public:
        Server(int port):
        serverPort(port){};
        void run(){
            int server_fd, new_socket;
            struct sockaddr_in address;
            int opt = 1;
            int size = sizeof(address);
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                cerr << "Socket failed\n";
                exit(EXIT_FAILURE);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(serverPort);
            if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
                std::cerr << "Bind failed\n";
                exit(EXIT_FAILURE);
            }
            if (listen(server_fd, 3) < 0) {
            std::cerr << "Listen\n";
            exit(EXIT_FAILURE);
            }

            while (true) {
                if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&size)) < 0) {
                    std::cerr << "Accept\n";
                    exit(EXIT_FAILURE);
                }
                std::thread(&Server::handleClient, this, new_socket).detach();
            }
        }
    private:
        int serverPort = 0;
        

        void getQuery(int socket, const string& request){

            string path = "pages/" + request;
            

            if (request.find(".html"))
            {
                
                std::cout << path << std::endl;
                ifstream file(path);
                if (!file.is_open()) {
                    sendErrorResponse(socket, "404 Not Found");
                    return;
                }
                string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                file.close();   

                string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
                send(socket, response.c_str(), response.size(), 0);
            }

        }

        void postQuery(int socket, const string& request, string fullReq){
            string getpost = getRequestBody(fullReq);

            if(request == "login"){
                string username = getPostCreds(getpost, "username");
                string password = getPostCreds(getpost, "password");
                cout << "\n\n\n" << username << password << "\n\n\n"; 
                ifstream file("sec.txt");
                if (!file.is_open()) {
                    cerr << "Cant open sec.txt" << endl;
                    return;
                }
                string line;
                unordered_map<string, string> reqCreds;
                while (getline(file, line)) {
                    unordered_map<string, string> credentials = parseCredentials(line);
                    if (credentials["username"] == username && password == password)
                    {
                        getQuery(socket, "login.html");
                    }
                    else{
                        getQuery(socket, "invalidCreds.html");
                    }
                    
                }
                
            }
        }

        void handleClient(int socket){
            char buffer[1024] = {0};
            int valread = read(socket, buffer, 1024);
            if (valread > 0) {
                string request(buffer, valread);
                std::cout << request << endl;
                string page = getRequestedPage(request);
                page = page.empty()? "index.html" :page;
                //cout << page << endl;
                
                if(!request.find("GET")) getQuery(socket, page);
                if(!request.find("POST")) postQuery(socket, page, request);


                lock_guard<mutex> guard(log_mutex);
                ofstream log_file("log.txt", ios_base::app);
                log_file << string(buffer, valread) << endl;
            }
            close(socket);
        }

        unordered_map<string, string> parseCredentials(const string& data) {
            unordered_map<string, string> credentials;
            size_t pos = 0, start = 0;

            while ((pos = data.find('=', start)) != string::npos) {
                string key = data.substr(start, pos - start);
                start = pos + 1;
                
                pos = data.find('&', start);
                string value;
                if (pos == string::npos) {
                    value = data.substr(start);
                    start = data.size();
                } else {
                    value = data.substr(start, pos - start);
                    start = pos + 1;
                }

                credentials[key] = value;
            }

            return credentials;
        }
        string getRequestBody(const string& request) {
            size_t pos = request.find("\r\n\r\n");
            if (pos == string::npos) return "";
            return request.substr(pos + 4); // Начинаем сразу после разделителя
        }

        string getPostCreds(const string& body, const string& paramName){
            size_t start = body.find(paramName + "=");
            if (start == string::npos) return "";
            start += paramName.length() + 1;
            
            size_t end = body.find("&", start);
            return (end == string::npos) ? body.substr(start) : body.substr(start, end - start);

        }

        string getRequestedPage(const string& request) {
            size_t pos1 = request.find(" ");
            if (pos1 == string::npos) return "";
            size_t pos2 = request.find(" ", pos1 + 1);
            if (pos2 == string::npos) return "";
            string page = request.substr(pos1 + 1, pos2 - pos1 - 1);
            if (page == "/") return "index.html";
            return page.substr(1);
        }

        void sendErrorResponse(int socket, const string& errorMessage) {
            string response = "HTTP/1.1 " + errorMessage + "\r\nContent-Type: text/plain\r\n\r\n" + errorMessage + "\n";
            send(socket, response.c_str(), response.size(), 0);
        }
};

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
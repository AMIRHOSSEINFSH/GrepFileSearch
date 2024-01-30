#include <iostream>
#include <string>
#include <filesystem>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>

#define MAX_LENGTH 1024
#define INVALID_WRITE_PORT -1001
using namespace std;

void incrementCounter(int count = 1);

void isInputValid(const string &path, const string &word);

void startIterationProcessFrom(const string &path, int parentWritePort);

void searchForResult(const string &path);

void readProcessThreadResult(int readPort);

void increaseCounter();

int openConnection();

std::string itWord;

int counter = 0;

pthread_mutex_t mutex;

char arr[MAX_LENGTH];

void appendThreadResult(char newResult[]) {
    strcat(arr, newResult);
}

void incrementCounter(int count) {
    pthread_mutex_lock(&mutex);
    counter+=count;
    pthread_mutex_unlock(&mutex);
}


int main(int argc, char *argv[]) {
    pthread_mutex_init(&mutex, NULL);
    //if (argc == 1) {
        //Open Socket
      //  openConnection();
   // } else {

        std::string path = "/home/amirhossein/Desktop/osProject/testFolder";//argv[argc - 2];
        std::string word = "magna";
        //argv[argc - 1];
        isInputValid(path, word);
        itWord = word;

        std::cout << "main Process :" << getpid() << std::endl;
        startIterationProcessFrom(path, INVALID_WRITE_PORT);
        //format the result
        std::cout << "Final Result is :" << arr << std::endl;

        std::cout << counter << std::endl;
  //  }


    return 0;
}

int openConnection() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(2121); // Port 2121
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address 127.0.0.1

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        return -1;
    }

    std::cout << "Server listening on 127.0.0.1:2121..." << std::endl;

    // Accept connections from clients
    socklen_t clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        return -1;
    }

    std::cout << "Connection accepted from " << inet_ntoa(clientAddress.sin_addr) << ":"
              << ntohs(clientAddress.sin_port) << std::endl;

    // Read messages from the client

    char path[100];
    char word[100];
    if (recv(clientSocket, path, sizeof(path), 0) == -1) {
        perror("Error receiving first array");
        close(clientSocket);
        close(serverSocket);
        return -1;
    }

    if (recv(clientSocket, word, sizeof(word), 0) == -1) {
        perror("Error receiving first array");
        close(clientSocket);
        close(serverSocket);
        return -1;
    }

    itWord = word;
    std::cout<<"path: "<<path << " word: "<< word<<std::endl;
    startIterationProcessFrom(path, INVALID_WRITE_PORT);

    // Close the sockets
    close(clientSocket);
    close(serverSocket);
}


//Validating input path and word
void isInputValid(const string &path, const string &word) {
    if (!std::filesystem::is_directory(path) || word.empty()) {
        std::cout << "The Path" + path + " or word " + word + " is not valid!" << std::endl;
        exit(1);
    }
}

void startIterationProcessFrom(const string &path, int parentWritePort) {
    std::cout << "Start Search in  " + path + "\n" << std::endl;
    // Declare and initialize a vector of integers
    std::vector<std::string> mProcessList;
    std::vector<std::string> mThreadList;

    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path.c_str());

    // Check if the directory opened successfully
    if (dir == nullptr) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    // Read the directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Skip "." and ".." directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char subPath[PATH_MAX];
                snprintf(subPath, sizeof(subPath), "%s/%s", path.c_str(), entry->d_name);
                mProcessList.emplace_back(subPath);
            }
        } else {
            char subPath[PATH_MAX];
            snprintf(subPath, sizeof(subPath), "%s/%s", path.c_str(), entry->d_name);
            mThreadList.emplace_back(subPath);
        }
    }

    // Close the directory
    closedir(dir);

    pthread_t threads[mThreadList.size()];
    pid_t processes[mProcessList.size()];
    pthread_t readProcessThreads[mProcessList.size()];

    int *pipes[mProcessList.size()];

    for (int i = 0; i < mProcessList.size(); ++i) {
        int fd[2];
        if (pipe(fd) < 0) {
            perror("Error Creating Pipe");
        }
        pipes[i] = fd;
        processes[i] = fork();
        if (processes[i] == 0) {
            //Child Process
            close(fd[0]);
            startIterationProcessFrom(mProcessList[i], pipes[i][1]);
            exit(0);
        } else {
            std::cout << "read: " << pipes[i][0] << " Write: " << pipes[i][1] << "\n" << std::endl;
            pthread_create(&readProcessThreads[i], nullptr,
                           reinterpret_cast<void *(*)(void *)>(readProcessThreadResult),
                           reinterpret_cast<void *>(pipes[i][0]));
        }
    }

    for (int i = 0; i < mThreadList.size(); ++i) {
        pthread_create(&threads[i], nullptr, reinterpret_cast<void *(*)(void *)>(&searchForResult),
                       (void *) &mThreadList[i]);
        std::cout << "File with Path: " + mThreadList[i] + "\n" << std::endl;
    }

    for (int i = 0; i < mProcessList.size(); i++) {
        waitpid(processes[i], nullptr, 0);
    }

    // Wait for threads to finish
    for (int i = 0; i < mThreadList.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }



    std::cout << "process Id: " << getpid() << " is waiting for path: " << path << " process list size: "
              << mProcessList.size() << "\n" << std::endl;
    for (int i = 0; i < mProcessList.size(); i++) {
        pthread_join(readProcessThreads[i], nullptr);
    }

    //Ensure child process and files are Done!

    if (parentWritePort != INVALID_WRITE_PORT) {
        std::cout << "Writing Result of " << arr << " from path: " << path << "\n" << std::endl;
        std::cout << "parent write port :" << parentWritePort << std::endl;
        if (write(parentWritePort, arr, sizeof(char) * MAX_LENGTH) == -1) {
            std::cout << "Write Port Error path is: " + path + "\n" << std::endl;
            perror("Error in Write port");
        }
        if (write(parentWritePort, &counter, sizeof(int)) == -1) {
            std::cout << "Write Port Error path is: " + path + "\n" << std::endl;
            perror("Error in Write port");
        }
        close(parentWritePort);
    }
}

//append child process result
void readProcessThreadResult(int readPort) {
    char childResultData[MAX_LENGTH];
    int mCounter;
    std::cout << "Read port :" << readPort << std::endl;
    if (read(readPort, childResultData, sizeof(char) * MAX_LENGTH) == -1) {
        perror("Error Creating Processes");
        return;
    }
    if (read(readPort, &mCounter, sizeof(int)) == -1) {
        perror("Error Creating Processes");
        return;
    }
    close(readPort);
    incrementCounter(mCounter);
    appendThreadResult(childResultData);
    std::cout << "read " << childResultData << " on Port: " << readPort << "\n" << std::endl;
}

void searchForResult(const string& path) {
    std::ifstream file;
    std::string line;
    int lineCount = 0;


    file.open(path.c_str());
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << path << std::endl;
        return;
    }

    // Read lines from file
    while (std::getline(file, line)) {
        // Increment the current line count
        ++lineCount;

        // Find the word in the current line
        std::size_t pos = line.find(itWord);
        while (pos != std::string::npos) {
            // Word found
            std::string result = path + ':'+ std::to_string(lineCount) + ':' + std::to_string(pos+1) + "\n";
            // Copy result string to resultChar array
            char resultChar[MAX_LENGTH];
            strcpy(resultChar, result.c_str());

            appendThreadResult(resultChar);
            incrementCounter(1);
            // Attempt to find next occurrence of the word
            pos = line.find(itWord, pos + itWord.length());
        }
    }

    file.close();

    std::cout << arr  << std::endl;
}



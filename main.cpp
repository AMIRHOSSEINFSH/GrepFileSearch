#include <iostream>
#include <string>
#include <filesystem>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <sys/wait.h>
#include <fstream>

#define MAX_LENGTH 1024
using namespace std;

void isInputValid(const string &path, const string &word);

void startIterationProcessFrom(const string& path,int iteration);

void searchForResult(const string& path);

void increaseCounter();

std::string itWord;

int counter = 0;

pthread_mutex_t mutex;

char arr[MAX_LENGTH];
void appendThreadResult(char newResult[]) {
    strcat(arr,newResult);
}


int main(int argc, char *argv[]) {
    pthread_mutex_init(&mutex, NULL);
    std::string path = argv[argc - 2];
    std::string word = argv[argc - 1];
    isInputValid(path, word);
    itWord = word;

    startIterationProcessFrom(path,0);
    //format the result
    std::cout << "Final Result is :" << arr << std::endl;

    std::cout << counter << std::endl;

    return 0;
}

//Validating input path and word
void isInputValid(const string &path, const string &word) {
    if (!std::filesystem::is_directory(path) || word.empty()) {
        std::cout << "The Path" + path + " or word " + word + " is not valid!" << std::endl;
        exit(1);
    }
}

void startIterationProcessFrom(const string& path,int iteration) {
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


    for (int i = 0; i < mProcessList.size() ; ++i) {
        int fd[2];
        if (pipe(fd) < 0) {
            perror("ERROR");
            std::cout<<"Error"<<std::endl;
        }

        std::cout<<fd[0]<<" " << fd[1]<<"\n"<<std::endl;
        processes[i] = fork();
        if (processes[i] == 0) {
            startIterationProcessFrom(mProcessList[i],iteration+1);
            close(fd[0]);
            appendThreadResult("Hel");
            std::cout<< "Child process "<<getpid() << " is writing "<<arr<< " for parent process "<< getppid()<<" on open pin of: "<<fd[0] << " And Write pin of: "<<fd[1] <<"\n"<< std::endl;
            //std::cout<<"Arr: "<<arr<<" in iteration of: "<<iteration+1<<"\n"<<std::endl;
            write(fd[1],arr, sizeof(char) * MAX_LENGTH);
            close(fd[1]);

            exit(0);
        }
        else {
            close(fd[1]);
            char result[MAX_LENGTH];
            read(fd[0],result, sizeof(char) * MAX_LENGTH);
            std::cout<< "Parent process "<<getpid() << " is Reading "<< result<<" And merging with "<< arr <<" on open pin of: "<<fd[0] << " And Write pin of: "<<fd[1] <<"\n"<< std::endl;
            appendThreadResult(result);
            std::cout<<"threadResult is "<<arr <<"\n"<<std::endl;
            close(fd[0]);
        }
        wait(nullptr);
    }


    for (int i = 0; i < mThreadList.size() ; ++i) {
        pthread_create(&threads[i], nullptr, reinterpret_cast<void *(*)(void *)>(&searchForResult), (void*)&mThreadList[i]);
        //std::cout << "File with Path: "+ mThreadList[i] + "\n" << std::endl;
    }

    // Wait for threads to finish
    for (int i = 0; i < mThreadList.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }


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
            increaseCounter();
            // Attempt to find next occurrence of the word
            pos = line.find(itWord, pos + itWord.length());
        }
    }

    file.close();
    
    std::cout << arr  << std::endl;
}

void increaseCounter() {
    pthread_mutex_lock(&mutex);
    counter++;
    pthread_mutex_unlock(&mutex);
}

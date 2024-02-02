#include "grepui.h"
#include "ui_grepui.h"
#include <QFileDialog>
#include <QDir>          

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


GrepUI::GrepUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GrepUI)
{
    ui->setupUi(this);
}

GrepUI::~GrepUI()
{
    delete ui;
}

void GrepUI::on_folderButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QDir::currentPath());
    if (!directory.isEmpty()) {
        ui->folderInput->setText(directory);
    }
}

std::string itWord;
int counter = 0;
pthread_mutex_t mutex;
char arr[MAX_LENGTH];

void GrepUI::on_searchButton_clicked()
{
    pthread_mutex_init(&mutex, NULL);
    std::string directory = ui->folderInput->text().toStdString();
    std::string searchWord = ui->searchInput->text().toStdString();


    isInputValid(directory, searchWord);
    itWord = searchWord;
    startIterationProcessFrom(directory, INVALID_WRITE_PORT);
    ui->resultOutput->setText(arr);
    ui->counterOutput->setText(QString::number(counter));
}

void appendThreadResult(char newResult[]) {
    strcat(arr, newResult);
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

void incrementCounter(int count) {
    pthread_mutex_lock(&mutex);
    counter+=count;
    pthread_mutex_unlock(&mutex);
}




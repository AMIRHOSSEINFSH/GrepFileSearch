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
#include <functional>
#include <future>
#include <thread>
#include <queue>
#include <mutex>

#define MAX_LENGTH 1024
#define INVALID_WRITE_PORT -1001
#define THREAD_POOL_SIZE 2
using namespace std;

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(std::size_t numThreads) {
        start(numThreads);
    }

    ~ThreadPool() {
        stop();
    }

    template<class T>
    auto enqueue(T task) -> std::future<decltype(task())> {
        auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));

        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            mTasks.emplace([=] {
                (*wrapper)();
            });
        }

        mEventVar.notify_one();
        return wrapper->get_future();
    }

private:
    std::vector<std::thread> mThreads;
    std::condition_variable mEventVar;
    std::mutex mEventMutex;
    bool mStopping = false;
    std::queue<Task> mTasks;

    void start(std::size_t numThreads) {
        for (auto i = 0u; i < numThreads; ++i) {
            mThreads.emplace_back([=] {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{mEventMutex};
                        mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });
                        if (mStopping && mTasks.empty())
                            break;
                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void stop() noexcept {
        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            mStopping = true;
        }

        mEventVar.notify_all();

        for (auto &thread: mThreads)
            thread.join();
    }
};

void incrementCounter(int count = 1);

void writeResult();

void isInputValid(const string &path, const string &word);

void startIterationProcessFrom(const string &path, int parentWritePort);

void searchForResult(const string &path);

void readProcessThreadResult(int readPort);

std::string itWord;

int counter = 0;

pthread_mutex_t mmutex;

char arr[MAX_LENGTH];

void appendThreadResult(char newResult[]) {
    strcat(arr, newResult);
}

void incrementCounter(int count) {
    pthread_mutex_lock(&mmutex);
    counter += count;
    pthread_mutex_unlock(&mmutex);
}


int main(int argc, char *argv[]) {
    pthread_mutex_init(&mmutex, NULL);
        std::string path = argv[argc - 2];
        std::string word = argv[argc - 1];
        isInputValid(path, word);
        itWord = word;

	std::cout << "Path " <<  path << std::endl;
	std::cout << "word " <<  word << std::endl;
        std::cout << "main Process :" << getpid() << std::endl;
        startIterationProcessFrom(path, INVALID_WRITE_PORT);
        //format the result
        writeResult();
        std::cout << "Final Result is :" << arr << std::endl;

        std::cout << counter << std::endl;

    return 0;
}

void writeResult() {
    const char* fileName = "output.txt";

    // Open a file for writing, create if not exists, truncate existing content
    std::ofstream outFile(fileName, std::ios::out | std::ios::trunc);

    // Obtain and print the absolute path of the output file
    std::filesystem::path absolutePath = std::filesystem::absolute(fileName);
    std::cout << "Absolute path of outFile: " << absolutePath << std::endl;
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    // Write char* to file
    outFile << arr << std::endl;
    outFile << '*' << std::endl;
    // Write int to file
    outFile << counter << std::endl;

    // Close the file
    outFile.close();

    std::cout << "Data written to file successfully." << std::endl;
}
//Validating input path and word
void isInputValid(const string &path, const string &word) {
    if (!std::filesystem::is_directory(path) || word.empty()) {
        std::cout << "The Path" + path + " or word " + word + " is not valid!" << std::endl;
        exit(1);
    }
}

void startIterationProcessFrom(const string &path, int parentWritePort) {
    ThreadPool pool{THREAD_POOL_SIZE};
    std::vector<std::future<void>> futures;
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
            pthread_create(&readProcessThreads[i], nullptr,
                           reinterpret_cast<void *(*)(void *)>(readProcessThreadResult),
                           reinterpret_cast<void *>(pipes[i][0]));
        }
    }

    for (int i = 0; i < mThreadList.size(); ++i) {
        futures.push_back(pool.enqueue(std::bind(searchForResult, mThreadList[i])));
    }

    for (int i = 0; i < mProcessList.size(); i++) {
        waitpid(processes[i], nullptr, 0);
    }

    // Wait for threads to finish
    for (auto &future: futures) {
        future.wait();  // This will wait for each task to complete.
    }

    for (int i = 0; i < mProcessList.size(); i++) {
        pthread_join(readProcessThreads[i], nullptr);
    }

    //Ensure child process and files are Done!

    if (parentWritePort != INVALID_WRITE_PORT) {
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
}

//append child process result
void searchForResult(const string &path) {
    auto start_time = std::chrono::high_resolution_clock::now();

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
            std::string result = path + ':' + std::to_string(lineCount) + ':' + std::to_string(pos + 1) + "\n";
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

    std::cout << arr << std::endl;

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "File : " << path << " Done! TimeSpent: " << duration.count() << " microseconds" << std::endl;

}



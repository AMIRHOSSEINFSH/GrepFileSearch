#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>

void isInputValid(char *path,const char* word);

void startIterationProcessFrom(char *path);

void searchForResult(char *path);

char *itWord;

int main(int argc, char *argv[]) {
    char *path = argv[argc - 2];
    char *word = argv[argc -1];
    isInputValid(path,word);
    itWord = word;
    startIterationProcessFrom(path);

    return 0;
}

//Validating input path and word
void isInputValid(char *path,const char* word) {
    struct stat stats;
    stat(path, &stats);
    // Check for file existence
    if (!S_ISDIR(stats.st_mode) || word == NULL != 0) {
        printf("The Path %s or word %s is not valid!", path,word);
        exit(1);
    }
}

//recursively
void startIterationProcessFrom(char *path) {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path);

    // Check if the directory opened successfully
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    // Read the directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Skip "." and ".." directories

            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("Directory: %s\n", entry->d_name);
                pid_t childPid = fork();

                if (childPid == -1) {
                   perror("Error forking process");
                   exit(EXIT_FAILURE);
                }

               if (childPid == 0) {
                    // Child process
                   char subPath[PATH_MAX];
                   snprintf(subPath, sizeof(subPath), "%s/%s", path, entry->d_name);
                    startIterationProcessFrom(subPath);
                   exit(EXIT_SUCCESS);
               } else {
                    // Parent process
                    waitpid(childPid, NULL, 0);
                    //todo read from pipLine ?
                }
            }
        } else {
            pthread_t tId;
            char subPath[PATH_MAX];
            snprintf(subPath, sizeof(subPath), "%s/%s", path, entry->d_name);
            tId = pthread_create(&tId, NULL, (void *(*)(void *)) &searchForResult, subPath);
            if (tId != 0) {
                perror("Failed to create thread!");
            }
            pthread_join(tId,NULL);
        }
    }

    // Close the directory
    closedir(dir);
}


void searchForResult(char* path) {
    printf("File Path is %s\n",path);
    //use itWord...
    //start Searching for Result
    //todo write to pipeline ?


}



#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

void isInputValid(char *path);
void startIterationProcessFrom(char *path);

int main(int argc, char *argv[]) {
    char *path = argv[argc-1];
    isInputValid(path);

    startIterationProcessFrom(path);

    return 0;
}

void isInputValid(char *path) {
    struct stat stats;
    stat(path, &stats);
    // Check for file existence
    if (!S_ISDIR(stats.st_mode)) {
        printf("The Path %s is not valid!",path);
        exit(1);
    }
}

//recursively
void startIterationProcessFrom(char *path) {

}







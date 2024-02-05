#include "grepui.h"
#include "ui_grepui.h"
#include <QFileDialog>
#include <QDir>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iostream>
#include <sys/stat.h>

using std::cout;

GrepUI::GrepUI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GrepSearchUI) {
    ui->setupUi(this);
}

GrepUI::~GrepUI()
{
    delete ui;
}
bool fileExists(const char* filePath) {
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(filePath);
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buffer;
    return (stat(filePath, &buffer) == 0 && S_ISREG(buffer.st_mode));
#endif
}
void GrepUI::on_folderButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QDir::currentPath());
    if (!directory.isEmpty()) {
        ui->folderInput->setText(directory);
    }
}

void GrepUI::on_searchButton_clicked()
{
    std::string directory = ui->folderInput->text().toStdString();
    std::string searchWord = ui->searchInput->text().toStdString();

    std::string executablePath = "../a.out " + directory + " " + searchWord;

    // Use execvp to replace the current process with the specified executable
    if (system(executablePath.c_str()) == -1) {
        // Execvp returns -1 if an error occurs
        perror("execvp");
        //return 1;
    }

    sleep(3);
    const char* fileName = "output.txt";


    // Open the file for reading
    std::ifstream inFile(fileName);

    if (!inFile.is_open()) {
        return;
    }

    // Read the entire content of the file into a string
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string fileContent = buffer.str();

    std::string firstPart;
    std::string secondPart;

    // Create a string stream from the input string
    std::istringstream iss(fileContent);

    // Split the string using the delimiter '*'
    std::getline(iss, firstPart, '*');
    std::getline(iss, secondPart, '*');

    ui->resultOutput->setText(QString::fromStdString(firstPart));
    ui->counterInput->setText(QString::fromStdString(secondPart));

    // Close the file
    inFile.close();


}







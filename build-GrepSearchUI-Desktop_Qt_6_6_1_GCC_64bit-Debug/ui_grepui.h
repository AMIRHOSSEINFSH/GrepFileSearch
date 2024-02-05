/********************************************************************************
** Form generated from reading UI file 'grepui.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GREPUI_H
#define UI_GREPUI_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GrepSearchUI
{
public:
    QWidget *centralwidget;
    QLabel *seachLabel;
    QLabel *folderLabel;
    QLabel *counterLabel;
    QLabel *resultLabel;
    QLineEdit *searchInput;
    QLineEdit *folderInput;
    QLineEdit *counterInput;
    QTextEdit *resultOutput;
    QPushButton *searchButton;
    QPushButton *folderButton;

    void setupUi(QMainWindow *GrepSearchUI)
    {
        if (GrepSearchUI->objectName().isEmpty())
            GrepSearchUI->setObjectName("GrepSearchUI");
        GrepSearchUI->resize(409, 295);
        centralwidget = new QWidget(GrepSearchUI);
        centralwidget->setObjectName("centralwidget");
        seachLabel = new QLabel(centralwidget);
        seachLabel->setObjectName("seachLabel");
        seachLabel->setGeometry(QRect(30, 20, 91, 17));
        folderLabel = new QLabel(centralwidget);
        folderLabel->setObjectName("folderLabel");
        folderLabel->setGeometry(QRect(30, 50, 111, 17));
        counterLabel = new QLabel(centralwidget);
        counterLabel->setObjectName("counterLabel");
        counterLabel->setGeometry(QRect(30, 80, 191, 21));
        resultLabel = new QLabel(centralwidget);
        resultLabel->setObjectName("resultLabel");
        resultLabel->setGeometry(QRect(30, 110, 67, 17));
        searchInput = new QLineEdit(centralwidget);
        searchInput->setObjectName("searchInput");
        searchInput->setGeometry(QRect(140, 10, 221, 25));
        folderInput = new QLineEdit(centralwidget);
        folderInput->setObjectName("folderInput");
        folderInput->setGeometry(QRect(140, 50, 221, 25));
        counterInput = new QLineEdit(centralwidget);
        counterInput->setObjectName("counterInput");
        counterInput->setGeometry(QRect(220, 80, 141, 25));
        counterInput->setReadOnly(true);
        resultOutput = new QTextEdit(centralwidget);
        resultOutput->setObjectName("resultOutput");
        resultOutput->setGeometry(QRect(30, 130, 331, 121));
        resultOutput->setReadOnly(true);
        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName("searchButton");
        searchButton->setGeometry(QRect(170, 260, 89, 25));
        folderButton = new QPushButton(centralwidget);
        folderButton->setObjectName("folderButton");
        folderButton->setGeometry(QRect(370, 50, 31, 25));
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("folder")));
        folderButton->setIcon(icon);
        GrepSearchUI->setCentralWidget(centralwidget);

        retranslateUi(GrepSearchUI);

        QMetaObject::connectSlotsByName(GrepSearchUI);
    } // setupUi

    void retranslateUi(QMainWindow *GrepSearchUI)
    {
        GrepSearchUI->setWindowTitle(QCoreApplication::translate("GrepSearchUI", "Grep Search UI", nullptr));
        seachLabel->setText(QCoreApplication::translate("GrepSearchUI", "Search Text:", nullptr));
        folderLabel->setText(QCoreApplication::translate("GrepSearchUI", "Choose Folder:", nullptr));
        counterLabel->setText(QCoreApplication::translate("GrepSearchUI", "Number Of Results Found:", nullptr));
        resultLabel->setText(QCoreApplication::translate("GrepSearchUI", "Result:", nullptr));
        searchButton->setText(QCoreApplication::translate("GrepSearchUI", "Search", nullptr));
        folderButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GrepSearchUI: public Ui_GrepSearchUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GREPUI_H

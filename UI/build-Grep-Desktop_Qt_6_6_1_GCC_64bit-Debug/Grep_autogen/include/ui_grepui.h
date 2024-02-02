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

class Ui_GrepUI
{
public:
    QWidget *centralwidget;
    QLabel *searchLabel;
    QLabel *folderLabel;
    QLabel *counterLabel;
    QLabel *resultLabel;
    QLineEdit *searchInput;
    QLineEdit *folderInput;
    QLineEdit *counterOutput;
    QTextEdit *resultOutput;
    QPushButton *searchButton;
    QPushButton *folderButton;

    void setupUi(QMainWindow *GrepUI)
    {
        if (GrepUI->objectName().isEmpty())
            GrepUI->setObjectName("GrepUI");
        GrepUI->resize(431, 295);
        centralwidget = new QWidget(GrepUI);
        centralwidget->setObjectName("centralwidget");
        searchLabel = new QLabel(centralwidget);
        searchLabel->setObjectName("searchLabel");
        searchLabel->setGeometry(QRect(20, 10, 101, 17));
        folderLabel = new QLabel(centralwidget);
        folderLabel->setObjectName("folderLabel");
        folderLabel->setGeometry(QRect(20, 40, 91, 17));
        counterLabel = new QLabel(centralwidget);
        counterLabel->setObjectName("counterLabel");
        counterLabel->setGeometry(QRect(20, 70, 181, 17));
        resultLabel = new QLabel(centralwidget);
        resultLabel->setObjectName("resultLabel");
        resultLabel->setGeometry(QRect(20, 100, 51, 17));
        searchInput = new QLineEdit(centralwidget);
        searchInput->setObjectName("searchInput");
        searchInput->setGeometry(QRect(120, 10, 261, 25));
        folderInput = new QLineEdit(centralwidget);
        folderInput->setObjectName("folderInput");
        folderInput->setGeometry(QRect(120, 40, 261, 25));
        counterOutput = new QLineEdit(centralwidget);
        counterOutput->setObjectName("counterOutput");
        counterOutput->setGeometry(QRect(200, 70, 181, 25));
        counterOutput->setReadOnly(true);
        resultOutput = new QTextEdit(centralwidget);
        resultOutput->setObjectName("resultOutput");
        resultOutput->setGeometry(QRect(20, 120, 361, 131));
        resultOutput->setReadOnly(true);
        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName("searchButton");
        searchButton->setGeometry(QRect(170, 260, 89, 25));
        folderButton = new QPushButton(centralwidget);
        folderButton->setObjectName("folderButton");
        folderButton->setGeometry(QRect(390, 40, 31, 25));
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("folder")));
        folderButton->setIcon(icon);
        GrepUI->setCentralWidget(centralwidget);

        retranslateUi(GrepUI);

        QMetaObject::connectSlotsByName(GrepUI);
    } // setupUi

    void retranslateUi(QMainWindow *GrepUI)
    {
        GrepUI->setWindowTitle(QCoreApplication::translate("GrepUI", "Grep Search UI", nullptr));
        searchLabel->setText(QCoreApplication::translate("GrepUI", "Search Word:", nullptr));
        folderLabel->setText(QCoreApplication::translate("GrepUI", "Search Path:", nullptr));
        counterLabel->setText(QCoreApplication::translate("GrepUI", "Number Of Found Result:", nullptr));
        resultLabel->setText(QCoreApplication::translate("GrepUI", "Result:", nullptr));
        searchButton->setText(QCoreApplication::translate("GrepUI", "Search", nullptr));
        folderButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GrepUI: public Ui_GrepUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GREPUI_H

#ifndef GREPUI_H
#define GREPUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class GrepUI;
}
QT_END_NAMESPACE

class GrepUI : public QMainWindow
{
    Q_OBJECT

public:
    GrepUI(QWidget *parent = nullptr);
    ~GrepUI();

private slots:
    void on_folderButton_clicked();

    void on_searchButton_clicked();

private:
    Ui::GrepUI *ui;
};
#endif // GREPUI_H

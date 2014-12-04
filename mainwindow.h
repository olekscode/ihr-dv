#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringListModel>

#include <string>
#include <sstream>
#include <fstream>
#include <math.h>

#include "dictionary.h"
#include "unknownwords.h"
#include "newword.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_searchButton_clicked();

    void on_editButton_clicked();

    void on_deleteButton_clicked();

    void addNewWord();

    void dontAddNewWord();

    void saveNewWord();

    void translateUnknown(QModelIndex);

    void saveTranslatedUnknown();

    void on_actionDivided_in_chapters_triggered();

    void on_actionAs_solid_dictionary_triggered();

    void hideShowUsages();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
    NewWord* newWordDialog;
    QStandardItemModel* tableModel;
    QStringListModel* unknownListModel;
    int numOfSaved;
    int numOfSavedUnk;
    //avoid this
    QString prevTextWhenChanged;
    QString dir;

    bool getDictFromFile();
    void saveDictInFile() const;
    void deleteWord(const QString &word);
    void buildTable();
    void buildUnknownList();
    float usageAsFloat() const;
    void disableDictActions(const bool &trigger);

    void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H

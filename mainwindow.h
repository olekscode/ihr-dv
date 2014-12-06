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
#include <QWidgetAction>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEvent>
#include <QSignalMapper>


#include <string>
#include <sstream>
#include <fstream>
#include <math.h>

#include "dictionary.h"
#include "unknownwords.h"
#include "newword.h"
#include "verifyingdialog.h"

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

    void translateUnknown(QModelIndex index);

    void saveTranslatedUnknown();

    void on_actionDivided_in_chapters_triggered();

    void on_actionAs_solid_dictionary_triggered();

    void hideShowUsages();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void buildTable(const QString &beginsWith = QString());

    void buildUnknownList();

    void on_actionGoogle_Translate_Mode_triggered(bool checked);

    void on_actionMagic_triggered();

    void on_verifyButton_clicked();

    void saveVerifiedTranslation();

    void on_actionShow_Unverified_triggered();

    void showItemFromTable(QModelIndex index);

private:
    Ui::MainWindow *ui;
    NewWord* newWordDialog;
    VerifyingDialog* verifyDialog;
    QStandardItemModel* tableModel;
    QStringListModel* unknownListModel;
    int numOfSaved;
    int numOfSavedUnk;
    //avoid this
    QString prevTextWhenChanged;
    QString dir;
    bool googleTranslateMode;

    bool getDictFromFile();
    void saveDictInFile() const;
    void deleteWord(const QString &word);
    float usageAsFloat() const;
    void disableDictActions(const bool &trigger);
    QList<QString> googleTranslate(QString keyword, QString from, QString to);
    void unknownGoogleTranslate();

//    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H

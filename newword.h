#ifndef NEWWORD_H
#define NEWWORD_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>

namespace Ui {
class NewWord;
}

class NewWord
      : public QDialog
{
    Q_OBJECT

public:
    explicit NewWord(QWidget *parent = 0);
    ~NewWord();

    QString translation();
    void setWord(QString word);

    //void closeEvent(QCloseEvent *event);

signals:
    void wordSaved();

private slots:
    void on_saveButton_clicked();

private:
    Ui::NewWord *ui;
    QString word;
    QString transl;
};

#endif // NEWWORD_H

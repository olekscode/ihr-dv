#include "newword.h"
#include "ui_newword.h"

NewWord::NewWord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewWord)
{
    ui->setupUi(this);
    setWindowTitle(word);
}

NewWord::~NewWord()
{
    transl = ui->textEdit->toPlainText();
    delete ui;
}

void NewWord::on_saveButton_clicked()
{
    transl = ui->textEdit->toPlainText();
    emit wordSaved();
}

QString NewWord::translation()
{
    QString result = transl;
    transl = QString();
    ui->textEdit->setText(transl);
    return result;
}

void NewWord::setWord(QString word)
{
    word = word;
}

//void NewWord::closeEvent(QCloseEvent *event)
//{
//    emit dialogClosed();
//    event->accept();
//}

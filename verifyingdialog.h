#ifndef VERIFYINGDIALOG_H
#define VERIFYINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QStandardItem>
#include <QStandardItemModel>

namespace Ui {
class VerifyingDialog;
}

class VerifyingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VerifyingDialog(QWidget *parent = 0);
    ~VerifyingDialog();

    QList<QString> verified() const;

    void setUnverified(QList<QString> transl);

signals:
    void translationVerified();

private slots:
    void on_verifyButton_clicked();

private:
    Ui::VerifyingDialog *ui;
    QList<QString> mUnverified;
    QList<QString> mVerified;
    QStandardItemModel* model;

    void buildList();
};

#endif // VERIFYINGDIALOG_H

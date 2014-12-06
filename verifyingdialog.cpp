#include "verifyingdialog.h"
#include "ui_verifyingdialog.h"

VerifyingDialog::VerifyingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerifyingDialog)
{
    ui->setupUi(this);
}

VerifyingDialog::~VerifyingDialog()
{
    delete ui;
}

QList<QString> VerifyingDialog::verified() const
{
    return mVerified;
}

void VerifyingDialog::setUnverified(QList<QString> transl)
{
    mUnverified = transl;
    mVerified = mUnverified;
    buildList();
}

void VerifyingDialog::buildList()
{
    model = new QStandardItemModel();
    QStandardItem* item;
    int row = 0;

    foreach (QString t, mUnverified)
    {
        item = new QStandardItem(t);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        model->setItem(row++, item);
    }

    ui->listView->setModel(model);
}

void VerifyingDialog::on_verifyButton_clicked()
{
    mVerified.clear();

    foreach (QStandardItem* i, model->takeColumn(0))
    {
        if (i->checkState() == Qt::Checked)
        {
            mVerified.append(i->text());
        }
    }
    emit translationVerified();
}

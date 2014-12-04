#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    newWordDialog = new NewWord(this);
    dir = QString();

    disableDictActions(true);

    connect(newWordDialog, SIGNAL(wordSaved()), this, SLOT(saveNewWord()));
}

MainWindow::~MainWindow()
{
    if (Dictionary::instance()->size() > numOfSaved
     || UnknownWords::instance()->size() > numOfSavedUnk)
    {
        int question = QMessageBox::question(this, "Closing", "Save changes?", "Yes", "No");
        if (question == 0)
        {
            saveDictInFile();
        }
    }

    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   hideShowUsages();
}

bool MainWindow::getDictFromFile()
{
    QString word;
    QList<QString> transl;
    QSet<float> usages;

    std::string buff;
    std::string buffItem;
    std::string line;

    QString path = dir + "dictionary.txt";
    std::ifstream in(path.toStdString());
    if (!in)
    {
        ui->statusBar->showMessage("Empty dictionary", 3000);
        return false;
    }
    else
    {
        while (!in.eof())
        {
            word = "";
            buffItem = "";

            std::getline(in, line);
            std::istringstream iss(line);

            while (iss >> buff && buff != "-")
            {
                word += QString(buff.c_str()) + ' ';
            }
            word.remove(word.length() - 1, 1);

            while (iss >> buff && buff != "-")
            {
                buffItem += buff;
                if (buffItem[buffItem.length() - 1] == ',')
                {
                    buffItem.erase(buffItem.length() - 1, 1);
                    transl.append(QString(buffItem.c_str()));
                    buffItem = "";
                }
                else
                    buffItem += ' ';
            }
            if (buffItem.length())
            {
                buffItem.erase(buffItem.length() - 1);
                transl.append(QString(buffItem.c_str()));
                buffItem = "";
            }

            while (iss >> buff && buff != "-")
            {
                buffItem += buff;
                if (buffItem[buffItem.length() - 1] == ',')
                {
                    buffItem.erase(buffItem.length() - 1);
                    usages.insert(QString(buffItem.c_str()).toFloat());
                    buffItem = "";
                }
                else
                    buffItem += ' ';
            }
            if (buffItem.length())
            {
                buffItem.erase(buffItem.length() - 1);
                usages.insert(QString(buffItem.c_str()).toFloat());
                buffItem = "";
            }

            if (!word.isEmpty() && word != "--END--")
            {
                word.remove(word.length() - 1);
                Dictionary::instance()->add(word, transl, usages);
                transl.clear();
                usages.clear();
            }

            return true;
        }
    }
    in.close();

    path = dir + "unknown.txt";
    std::ifstream unk(path.toStdString());

    if (unk)
    {
        while (!unk.eof())
        {
            word = "";
            buffItem = "";

            std::getline(unk, line);
            std::istringstream iss(line);

            while (iss >> buff && buff != "-")
            {
                word += QString(buff.c_str()) + ' ';
            }
            word.remove(word.length() - 1, 1);

            while (iss >> buff && buff != "-")
            {
                buffItem += buff;
                if (buffItem[buffItem.length() - 1] == ',')
                {
                    buffItem.erase(buffItem.length() - 1);
                    usages.insert(QString(buffItem.c_str()).toFloat());
                    buffItem = "";
                }
                else
                    buffItem += ' ';
            }
            if (buffItem.length())
            {
                buffItem.erase(buffItem.length() - 1);
                usages.insert(QString(buffItem.c_str()).toFloat());
                buffItem = "";
            }

            if (!word.isEmpty() && word != "--END--")
            {
                word.remove(word.length() - 1);
                UnknownWords::instance()->add(word, usages);
                usages.clear();
            }
        }
    }

    unk.close();
}

void MainWindow::saveDictInFile() const
{
    QString path = dir + "dictionary.txt";
    std::ofstream out(path.toStdString());
    QList<QString> transl;
    QSet<float> usages;
    int counter;

    for (auto pair : Dictionary::instance()->toStdMap())
    {
        out << pair.first.toStdString() << " - ";

        transl = Dictionary::instance()->translations(pair.first);
        usages = Dictionary::instance()->usages(pair.first);

        counter = 0;
        foreach (QString t, transl)
        {
            out << t.toStdString();
            if (counter++ < transl.size() - 1)
                out << ", ";
        }

        out << " - ";

        counter = 0;
        foreach (float u, usages)
        {
            out << QString::number(u).toStdString();
            if (counter++ < usages.size() - 1)
                out << ", ";
        }
        out << '\n';
    }
    out << "--END--";

    out.close();

    path = dir + "unknown.txt";
    std::ofstream unk(path.toStdString());

    foreach (auto p, UnknownWords::instance()->toQList())
    {
        unk << p.first.toStdString() << " - ";

        usages = p.second;
        counter = 0;
        foreach (float u, usages)
        {
            unk << QString::number(u).toStdString();
            if (counter++ < usages.size() - 1)
                unk << ", ";
        }
        unk << '\n';
    }
    unk << "--END--";

    unk.close();
}

void MainWindow::deleteWord(const QString &word)
{
    Dictionary::instance()->remove(word);
}

void MainWindow::buildTable()
{
    tableModel = new QStandardItemModel(0, 3, this);
    tableModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Word")));
    tableModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Translations")));
    tableModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Usages")));

    QString transl = "";
    QString usages = "";

    QList<QStandardItem*> itemsInRow;

    for (auto pair : Dictionary::instance()->toStdMap())
    {
        foreach (QString t, pair.second.first)
        {
            transl += t + ", ";
        }
        transl.remove(transl.size() - 2, 2);

        foreach (float u, pair.second.second)
        {
            usages += QString::number(u) + ", ";
        }
        usages.remove(usages.size() - 2, 2);

        itemsInRow.append(new QStandardItem(pair.first));
        itemsInRow.append(new QStandardItem(transl));
        itemsInRow.append(new QStandardItem(usages));

        tableModel->appendRow(itemsInRow);
        itemsInRow.clear();
        transl = QString();
        usages = QString();
    }

    ui->tableView->setModel(tableModel);

    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->setColumnWidth(0, 150);
    ui->tableView->setColumnWidth(2, 75);
}

void MainWindow::buildUnknownList()
{
    unknownListModel = new QStringListModel(this);
    QStringList unknownList;

    foreach (auto p, UnknownWords::instance()->toQList())
    {
        unknownList << p.first;
    }

    unknownListModel->setStringList(unknownList);
    ui->unknownView->setModel(unknownListModel);

    connect(ui->unknownView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(translateUnknown(QModelIndex)));
}

float MainWindow::usageAsFloat() const
{
    QString usg2Str = ui->usage2Edit->text();

    float usg1 = ui->usage1Edit->text().toFloat();
    float usg2 = usg2Str.toFloat();

    return usg1 + float(usg2) / pow(10, usg2Str.length());
}

void MainWindow::on_searchButton_clicked()
{
    QString word = ui->lineEdit->text();

    if (!Dictionary::instance()->contains(word))
    {
        prevTextWhenChanged = ui->lineEdit->text();

        ui->textBrowser->setText("[Nothing was found]");
        ui->searchButton->setText("Add");
        connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(addNewWord()));
        connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(dontAddNewWord()));
    }

    else
    {
        QString result = QString();
        foreach (QString t, Dictionary::instance()->translations(word))
        {
            result += t + '\n';
        }

        ui->textBrowser->setText(result);

        Dictionary::instance()->addUsage(word, usageAsFloat());
        buildTable();
    }
}

void MainWindow::on_editButton_clicked()
{
    QString word = ui->lineEdit->text();

    newWordDialog->setWord(word);
    newWordDialog->show();
}

void MainWindow::addNewWord()
{
    newWordDialog->setWord(ui->lineEdit->text());
    newWordDialog->show();

    ui->textBrowser->clear();
    ui->searchButton->setText("Search");
    disconnect(ui->searchButton, SIGNAL(clicked()), 0, 0);
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(on_searchButton_clicked()));
    disconnect(ui->lineEdit, SIGNAL(textChanged(QString)), 0, 0);
}

void MainWindow::dontAddNewWord()
{
    ui->textBrowser->clear();
    ui->searchButton->setText("Search");
    disconnect(ui->searchButton, SIGNAL(clicked()), 0, 0);
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(on_searchButton_clicked()));
    disconnect(ui->lineEdit, SIGNAL(textChanged(QString)), 0, 0);

    QSet<float> usages;
    usages.insert(usageAsFloat());
    UnknownWords::instance()->add(prevTextWhenChanged, usages);

    buildUnknownList();
}

void MainWindow::saveNewWord()
{
    QString word = ui->lineEdit->text();
    QString translStr = newWordDialog->translation();

    newWordDialog->close();

    QList<QString> translations = translStr.split(", ");
    QSet<float> usages;

    usages.insert(usageAsFloat());

    if (!translations.isEmpty())
    {
        Dictionary::instance()->add(word, translations, usages);
        // TODO: make Dictionary emit signal changed()
        //       and connect it to buildTable()
        buildTable();

        if (UnknownWords::instance()->contains(word))
        {
            UnknownWords::instance()->remove(word);
            // TODO: make UnknownWords emit signal changed()
            //       and connect it to buildUnknownList()
            buildUnknownList();
        }

        QString result = QString();
        foreach (QString t, translations)
        {
            result += t + '\n';
        }

        ui->textBrowser->setText(result);

        QString msg = QString("Your dictionary has ")
                + QString::number(Dictionary::instance()->size()) + " words";
        ui->statusBar->showMessage(msg, 4000);
    }
}

void MainWindow::translateUnknown(QModelIndex index)
{
    QString word = unknownListModel->data(index, 0).toString();

    ui->lineEdit->setText(word);

    disconnect(newWordDialog, SIGNAL(wordSaved()), 0, 0);
    connect(newWordDialog, SIGNAL(wordSaved()), this, SLOT(saveTranslatedUnknown()));

    newWordDialog->setWord(word);
    newWordDialog->show();
}

void MainWindow::saveTranslatedUnknown()
{
    QString word = ui->lineEdit->text();
    QString translStr = newWordDialog->translation();

    newWordDialog->close();

    QList<QString> translations = translStr.split(", ");
    QSet<float> usages = UnknownWords::instance()->usages(word);

    if (!translations.isEmpty())
    {
        Dictionary::instance()->add(word, translations, usages);
        UnknownWords::instance()->remove(word);

        QString result = QString();
        foreach (QString t, translations)
        {
            result += t + '\n';
        }

        ui->textBrowser->setText(result);

        QString msg = QString("Your dictionary has ")
                + QString::number(Dictionary::instance()->size()) + " words";
        ui->statusBar->showMessage(msg, 4000);

        buildTable();
        buildUnknownList();
    }

    disconnect(newWordDialog, SIGNAL(wordSaved()), 0, 0);
    connect(newWordDialog, SIGNAL(wordSaved()), this, SLOT(saveNewWord()));
}

void MainWindow::on_deleteButton_clicked()
{
    int question = QMessageBox::question(this, "Deleting", "Are you sure you want to permanently delete this word?");
    if (question == 16384)
    {
        deleteWord(ui->lineEdit->text());

        ui->lineEdit->clear();
        ui->textBrowser->clear();
        buildTable();
    }
}

void MainWindow::hideShowUsages()
{
    bool hide;

    if (ui->tableView->isColumnHidden(2))
    {
        hide = ui->tableView->columnWidth(1)
                < 3 * ui->tableView->columnWidth(0);
    }
    else
    {
        // to avoid blinking
        hide = ui->tableView->columnWidth(1) + ui->tableView->columnWidth(2)
                < 3 * ui->tableView->columnWidth(0);
    }

    ui->tableView->setColumnHidden(2, hide);
}

void MainWindow::on_actionDivided_in_chapters_triggered()
{
    QMap<float, QList<QPair<QString, QList<QString> > > > dictByUsages;

    for (auto p : Dictionary::instance()->toStdMap())
    {
        foreach (float u, p.second.second)
        {
            dictByUsages[u].append(QPair<QString, QList<QString> >(p.first, p.second.first));
        }
    }

    std::ofstream out("export.txt");

    int counter;
    for (auto p : dictByUsages.toStdMap())
    {
        out << "---------- " << p.first << " ----------" << '\n';

        for (auto q : p.second)
        {
            out << q.first.toStdString() << " - ";

            counter = 0;
            foreach (QString t, q.second)
            {
                out << t.toStdString();
                if (counter++ < q.second.size() - 1)
                    out << ", ";
            }
            out << '\n';
        }
        out << '\n';
    }

    out.close();
}

void MainWindow::on_actionAs_solid_dictionary_triggered()
{
    std::ofstream out("export_solid.txt");

    int counter;
    QChar curr = ' ';

    for (auto p : Dictionary::instance()->toStdMap())
    {
        if (p.first[0].toLower() != curr.toLower())
        {
            if (curr != ' ')
            {
                out << '\n';
            }

            curr = p.first[0];

            out << "------- " << QString(curr).toStdString() << " -------\n";
        }
        out << p.first.toStdString() << " - ";

        counter = 0;
        for (auto t : p.second.first)
        {
            out << t.toStdString();
            if (counter++ < p.second.first.size() - 1)
                out << ", ";
        }
        out << '\n';
    }

    out.close();
}

void MainWindow::on_actionOpen_triggered()
{
    // TODO: fix it and add filters
    QFileDialog* getDir = new QFileDialog(this);
    getDir->show();

    QDir directory = getDir->directory();
    dir = directory.absolutePath();

    if (getDictFromFile())
    {
        numOfSaved = Dictionary::instance()->size();
        numOfSavedUnk = UnknownWords::instance()->size();

        buildTable();
        buildUnknownList();

        hideShowUsages();
        disableDictActions(false);
    }
}

void MainWindow::on_actionSave_triggered()
{
    saveDictInFile();
    numOfSaved = Dictionary::instance()->size();
    numOfSavedUnk = UnknownWords::instance()->size();
}

void MainWindow::disableDictActions(const bool &trigger)
{
    ui->actionAs_solid_dictionary->setDisabled(trigger);
    ui->actionDivided_in_chapters->setDisabled(trigger);
    ui->actionCopy->setDisabled(trigger);
    ui->actionCut->setDisabled(trigger);
    ui->actionPaste->setDisabled(trigger);
    ui->actionDelete->setDisabled(trigger);
    ui->actionUndo->setDisabled(trigger);
    ui->actionRedo->setDisabled(trigger);
    ui->actionSave->setDisabled(trigger);
}

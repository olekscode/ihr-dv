#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    newWordDialog = new NewWord(this);
    verifyDialog = new VerifyingDialog(this);
    dir = QString();
    googleTranslateMode = false;

    disableDictActions(true);

    connect(newWordDialog, SIGNAL(wordSaved()), this, SLOT(saveNewWord()));
    connect(verifyDialog, SIGNAL(translationVerified()), this, SLOT(saveVerifiedTranslation()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(on_searchButton_clicked()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(buildTable(QString)));
    connect(ui->unknownView, SIGNAL(activated(QModelIndex)), this, SLOT(translateUnknown(QModelIndex)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showItemFromTable(QModelIndex)));

//    // to select all text in ui->lineEdit when user doubleclicks on it
//    ui->lineEdit->installEventFilter(this);

    getDictFromFile();

    numOfSaved = Dictionary::instance()->size();
    numOfSavedUnk = UnknownWords::instance()->size();

    buildTable();
    buildUnknownList();

    hideShowUsages();
    disableDictActions(false);
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

//bool MainWindow::eventFilter(QObject *object, QEvent *event)
//{
//    // to select all text in ui->lineEdit when user doubleclicks on it
//    if (object == ui->lineEdit && event->type() == QEvent::MouseButtonDblClick)
//    {
//        ui->statusBar->showMessage("Here", 3000);
//        ui->lineEdit->selectAll();
//        return false;
//    }
//    return false;
//}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   hideShowUsages();
}

bool MainWindow::getDictFromFile()
{
    bool ret;
    QString word;
    QList<QString> transl;
    QSet<float> usages;
    bool verified;

    std::string buff;
    std::string buffItem;
    std::string line;

    QString path = (dir.isEmpty()) ? "dictionary.txt" : dir + "/dictionary.txt";
    std::ifstream in(path.toStdString());
    if (!in)
    {
        ui->statusBar->showMessage("Empty dictionary", 3000);
        ret = false;
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
                if (buffItem[buffItem.length() - 1] == '\\')
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
                if (buffItem[buffItem.length() - 1] == '\\')
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

            iss >> verified;

            if (!word.isEmpty() && word != "--END--")
            {
                word.remove(word.length() - 1);
                Dictionary::instance()->add(word, transl, usages, verified);
                transl.clear();
                usages.clear();
            }
        }
        ret = true;
    }
    in.close();

    path = (dir.isEmpty()) ? "unknown.txt" : dir + "/unknown.txt";

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
                if (buffItem[buffItem.length() - 1] == '\\')
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

    return ret;
}

void MainWindow::saveDictInFile() const
{
    QString path = (dir.isEmpty()) ? "dictionary.txt" : dir + "/dictionary.txt";
    std::ofstream out(path.toStdString());

    QString word;
    QList<QString> transl;
    QSet<float> usages;
    int counter;

    for (auto pair : Dictionary::instance()->toStdMap())
    {
        word = pair.first;
        out << word.toStdString() << " - ";

        transl = Dictionary::instance()->translations(word);
        usages = Dictionary::instance()->usages(word);

        counter = 0;
        foreach (QString t, transl)
        {
            out << t.toStdString();
            if (counter++ < transl.size() - 1)
                out << "\\ ";
        }

        out << " - ";

        counter = 0;
        foreach (float u, usages)
        {
            out << QString::number(u).toStdString();
            if (counter++ < usages.size() - 1)
                out << "\\ ";
        }
        out << " - ";
        out << Dictionary::instance()->isVerified(word);
        out << '\n';
    }
    out << "--END--";

    out.close();

    path = (dir.isEmpty()) ? "unknown.txt" : dir + "/unknown.txt";
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
                unk << "\\ ";
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

void MainWindow::buildTable(const QString &beginsWith)
{
    bool unverified = ui->actionShow_Unverified->isChecked();

    tableModel = new QStandardItemModel(0, 4, this);
    tableModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Word")));
    tableModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Translations")));
    tableModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Usages")));
    tableModel->setHorizontalHeaderItem(3, new QStandardItem(QString("Verified")));

    QString transl = "";
    QString usages = "";
    QString verified = "";

    QList<QStandardItem*> itemsInRow;
    bool show;
    bool isVerified;

    for (auto pair : Dictionary::instance()->toStdMap())
    {
        show = true;
        for (int i = 0; i < beginsWith.length(); ++i)
        {
            if (pair.first[i] != beginsWith[i])
            {
                show = false;
                break;
            }
        }

        isVerified = Dictionary::instance()->isVerified(pair.first);
        verified = (isVerified) ? "true" : "false";

        if (show && !(unverified && isVerified))
        {
            foreach (QString t, pair.second.first)
            {
                transl += t + "\\\\";
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
            itemsInRow.append(new QStandardItem(verified));

            tableModel->appendRow(itemsInRow);
            itemsInRow.clear();
            transl = QString();
            usages = QString();
        }
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

    if (!word.isEmpty() && !Dictionary::instance()->contains(word))
    {
        if (googleTranslateMode)
        {
            unknownGoogleTranslate();
        }
        else
        {
            prevTextWhenChanged = ui->lineEdit->text();

            ui->textBrowser->setText("[Nothing was found]");
            ui->searchButton->setText("Add");

            QSet<float> usages;
            usages.insert(usageAsFloat());
            UnknownWords::instance()->add(prevTextWhenChanged, usages);

            buildUnknownList();

            connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(addNewWord()));
            connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(dontAddNewWord()));
        }
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

void MainWindow::saveVerifiedTranslation()
{
    QString word = ui->lineEdit->text();
    QList<QString> transl = verifyDialog->verified();
    verifyDialog->close();

    Dictionary::instance()->setTranslations(word, transl, true);
    buildTable();
    on_searchButton_clicked();
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

    QString path = (dir.isEmpty()) ? "export.txt" : dir + "/export.txt";
    std::ofstream out(path.toStdString());

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
    QString path = (dir.isEmpty()) ? "export_solid.txt" : dir + "/export_solid.txt";
    std::ofstream out(path.toStdString());

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
    dir = QFileDialog::getExistingDirectory(this, "Choose the dictionary folder");

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
    QList<QAction*> dictActions;
        dictActions.append(ui->actionAs_solid_dictionary);
        dictActions.append(ui->actionDivided_in_chapters);
        dictActions.append(ui->actionCopy);
        dictActions.append(ui->actionCut);
        dictActions.append(ui->actionPaste);
        dictActions.append(ui->actionDelete);
        dictActions.append(ui->actionUndo);
        dictActions.append(ui->actionRedo);
        dictActions.append(ui->actionSave);

    QList<QWidget*> dictWidgets;
        dictWidgets.append(ui->searchButton);
        dictWidgets.append(ui->editButton);
        dictWidgets.append(ui->deleteButton);
        dictWidgets.append(ui->lineEdit);
        dictWidgets.append(ui->usage1Edit);
        dictWidgets.append(ui->usage2Edit);
        dictWidgets.append(ui->tableView);
        dictWidgets.append(ui->unknownView);

    foreach (QAction* a, dictActions)
    {
        a->setDisabled(trigger);
    }

    foreach (QWidget* w, dictWidgets)
    {
        w->setDisabled(trigger);
    }
}

QList<QString> MainWindow::googleTranslate(QString keyword, QString from, QString to)
{
    //Translate URL
    QString url = QString("http://translate.google.com/") +
            "translate_a/t?client=t&text=%0&hl=%1&sl=%2&tl=%1&multires=1&prev=enter&oc=2&ssel=0&tsel=0&uptl=%1&sc=1";
    url = url.arg(keyword).arg(to).arg(from);

    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

    //Get reply from Google
    do {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    } while(!reply->isFinished());

    //Convert to string
    QString translation(reply->readAll());
    reply->close();

    QList<QString> transl;
    QString buff = QString();
    int level = 0;
    bool quotesOpened = false;

    foreach (QChar ch, translation)
    {
        if (ch == '[') ++level;
        else if (ch == ']') --level;
        else if (ch == '"')
        {
            if (quotesOpened)
            {
                if (level == 5) transl.append(buff);
                buff = QString();
            }
            quotesOpened = !quotesOpened;
        }
        else if (ch == ',') buff = QString();
        else buff += ch;
    }

    return transl;
}

void MainWindow::unknownGoogleTranslate()
{
    QString word = ui->lineEdit->text();
    QList<QString> transl = googleTranslate(word, "en", "uk");
    QSet<float> usages;
    usages.insert(usageAsFloat());

    QString result = QString();
    foreach (QString t, transl)
    {
        result += t + '\n';
    }

    ui->textBrowser->setText(result);

    Dictionary::instance()->add(word, transl, usages, false);
    buildTable();
}

void MainWindow::on_actionGoogle_Translate_Mode_triggered(bool checked)
{
    googleTranslateMode = checked;
}

void MainWindow::on_actionMagic_triggered()
{
    foreach (auto p, UnknownWords::instance()->toQList())
    {
        Dictionary::instance()->add(p.first, googleTranslate(p.first, "en", "uk"), p.second, false);
        UnknownWords::instance()->remove(p.first);

        buildTable();
        buildUnknownList();
    }
}

void MainWindow::on_verifyButton_clicked()
{
    QString word = ui->lineEdit->text();
    QList<QString> unverified = Dictionary::instance()->translations(word);

    verifyDialog->setUnverified(unverified);
    verifyDialog->show();
}

void MainWindow::on_actionShow_Unverified_triggered()
{
    buildTable();
}

void MainWindow::showItemFromTable(QModelIndex index)
{
    QString word = tableModel->item(index.row())->text();
    QList<QString> transl = Dictionary::instance()->translations(word);

    ui->lineEdit->setText(word);

    QString result = QString();
    foreach (QString t, transl)
    {
        result += t + '\n';
    }

    ui->textBrowser->setText(result);
}

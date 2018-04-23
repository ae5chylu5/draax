/********************************************************************

Name: Draax
Homepage: http://github.com/ae5chylu5/draax
Author: ae5chylu5
Description: A front end for converting aax files

Copyright (C) 2017 ae5chylu5

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************/

#include "draax.h"

draax::draax(QWidget *parent, bool devEnabled)
    : QMainWindow(parent)
{
    // tree columns are in this order:
    // name | path | hash | activation bytes | codec | title | author | narrator

    int minWidth = 450;
    int minHeight = 500;

    reqApps << _EXIFTOOL << _FFPROBE << _FFMPEG;

    titRE << "[ \\-]{1,3}\\(?[Uu]nabridged\\)?$"
          << "[ \\-]{1,3}\\(?[Aa]bridged\\)?$";

    autNarRE << "[ \\-]{1,3}\\(?[Tt]ranslator\\)?$"
             << "[ \\-]{1,3}\\(?[Ee]ditor\\)?$"
             << "[ \\-]{1,3}\\(?[Ii]ntroduction\\)?$"
             << "[ \\-]{1,3}\\(?[Ff]oreword\\)?$";

    createToolBars(devEnabled);
    createStatusBar();
    createTreeWidget();

    setDialog = new Settings();
    abtDialog = new About();

    // load default output path from settings window
    setRootItem(setDialog->outputPath->text());

    setMinimumSize(minWidth, minHeight);
    setWindowTitle("Draax");
    setWindowIcon(QIcon(":/images/draax48.png"));

    // events must be connected AFTER dialogs are created
    // since we use the show() slot on each dialog.
    connectEvents();
}

draax::~draax()
{

}

void draax::createToolBars(bool devEnabled)
{
    QStringList nf = hasApps(reqApps);
    if (nf.size() > 0) {
        qDebug() << "Some features may be disabled. The following apps are missing:" << nf;
    }

    outAction = new QAction(QIcon(":/images/output48.png"), "");
    outAction->setStatusTip(tr("Select output folder"));

    metAction = new QAction(QIcon(":/images/metaData48.png"), "");
    metAction->setStatusTip(tr("Extract meta-data from aax files"));
    if (nf.contains("exiftool")) metAction->setDisabled(1);

    valAction = new QAction(QIcon(":/images/validate48.png"), "");
    valAction->setStatusTip(tr("Validate activation codes"));
    if (nf.contains("ffprobe")) valAction->setDisabled(1);

    craAction = new QAction(QIcon(":/images/crack48.png"), "");
    craAction->setStatusTip(tr("Generate missing activation codes"));
    craAction->setDisabled(1);

    genAction = new QAction(QIcon(":/images/generate48.png"), "");
    genAction->setStatusTip(tr("Generate folder layout"));

    encAction = new QAction(QIcon(":/images/encode48.png"), "");
    encAction->setStatusTip(tr("Convert aax files"));
    if (nf.contains("ffprobe") || nf.contains("ffmpeg")) encAction->setDisabled(1);

    wizAction = new QAction(QIcon(":/images/wizard48.png"), "");
    wizAction->setStatusTip(tr("Automated wizard"));
    if (nf.contains("ffprobe") || nf.contains("ffmpeg")) wizAction->setDisabled(1);

    QWidget *spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAction = new QAction(QIcon(":/images/settings48.png"), "");
    setAction->setStatusTip(tr("Settings"));

    abtAction = new QAction(QIcon(":/images/about48.png"), "");
    abtAction->setStatusTip(tr("About"));

    topToolBar = new QToolBar(tr("File Operations"));
    topToolBar->setIconSize(QSize(40,40));
    topToolBar->addAction(outAction);
    if (devEnabled) topToolBar->addAction(metAction);
    if (devEnabled) topToolBar->addAction(valAction);
    if (devEnabled) topToolBar->addAction(craAction);
    if (devEnabled) topToolBar->addAction(genAction);
    if (devEnabled) topToolBar->addAction(encAction);
    topToolBar->addAction(wizAction);
    topToolBar->addWidget(spacer1);
    topToolBar->addAction(setAction);
    topToolBar->addAction(abtAction);
    topToolBar->setMovable(0);
    topToolBar->setFloatable(0);
    addToolBar(Qt::TopToolBarArea, topToolBar);

    inpAction = new QAction(QIcon(":/images/input48.png"), "");
    inpAction->setStatusTip(tr("Select aax files"));

    remAction = new QAction(QIcon(":/images/remove48.png"), "");
    remAction->setStatusTip(tr("Remove"));
    remAction->setDisabled(1);

    QWidget *spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    rmaAction = new QAction(QIcon(":/images/removeAll48.png"), "");
    rmaAction->setStatusTip(tr("Remove all"));

    botToolBar = new QToolBar(tr("Tree Operations"));
    botToolBar->setIconSize(QSize(24,24));
    botToolBar->addAction(inpAction);
    botToolBar->addAction(remAction);
    botToolBar->addWidget(spacer2);
    botToolBar->addAction(rmaAction);
    botToolBar->setMovable(0);
    botToolBar->setFloatable(0);
    addToolBar(Qt::BottomToolBarArea, botToolBar);
}

void draax::createStatusBar()
{
    statusBar = new QStatusBar();
    statusBar->setSizeGripEnabled(0);
    setStatusBar(statusBar);
}

void draax::createTreeWidget()
{
    tree = new TreeWidget();
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setColumnCount(8);
    tree->setHeaderHidden(1);
    tree->setDragEnabled(1);
    tree->setAcceptDrops(1);
    tree->setDropIndicatorShown(1);
    tree->setDragDropMode(QAbstractItemView::InternalMove);
    tree->hideColumn(1);
    tree->hideColumn(2);
    tree->hideColumn(3);
    tree->hideColumn(4);
    tree->hideColumn(5);
    tree->hideColumn(6);
    tree->hideColumn(7);
    tree->setMouseTracking(1);
    tree->viewport()->setAcceptDrops(1);

    tree->rootItem = new QTreeWidgetItem(tree, QStringList() << "Output");
    tree->rootItem->setFlags(Qt::ItemIsEnabled);
    tree->rootItem->setTextAlignment(colName, Qt::AlignVCenter | Qt::AlignLeft);
    tree->rootItem->setStatusTip(colName, "Output");
    tree->rootItem->setToolTip(colName, "Output");
    tree->rootItem->setExpanded(1);
    tree->rootItem->setIcon(colName, QIcon(":/images/folder48.png"));
    tree->rootItem->setData(colName, ItemRole, Root);

    tree->addTopLevelItem(tree->rootItem);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(tree, 1);

    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(vbox);
    setCentralWidget(mainWidget);
}

void draax::connectEvents()
{
    connect(outAction, SIGNAL(triggered()), this, SLOT(selectOutputFolder()));
    connect(metAction, SIGNAL(triggered()), this, SLOT(getMetaData()));
    connect(valAction, SIGNAL(triggered()), this, SLOT(validate()));
    connect(craAction, SIGNAL(triggered()), this, SLOT(crack()));
    connect(genAction, SIGNAL(triggered()), this, SLOT(genLayout()));
    connect(encAction, SIGNAL(triggered()), this, SLOT(encode()));
    connect(wizAction, SIGNAL(triggered()), this, SLOT(wizard()));

    connect(tree, SIGNAL(itemSelectionChanged()), this, SLOT(toggleButtonAccess()));

    connect(inpAction, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(remAction, SIGNAL(triggered()), tree, SLOT(removeSelected()));
    connect(rmaAction, SIGNAL(triggered()), tree, SLOT(removeAll()));

    connect(abtAction, SIGNAL(triggered()), abtDialog, SLOT(show()));
    connect(setAction, SIGNAL(triggered()), setDialog, SLOT(show()));

    connect(setDialog->outputPath, SIGNAL(textChanged(QString)), this, SLOT(setRootItem(QString)));
}

void draax::toggleButtonAccess()
{
    QList<QTreeWidgetItem*> items = tree->selectedItems();
    int total = items.size();

    remAction->setDisabled((total < 1));
}

void draax::addFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("Select AAX Files"),
                                                      QDir::homePath(),
                                                      "AAX (*.aax)");
    int total = files.size();
    if (total < 1) return;

    for (int i = 0; i < total; ++i) {
        tree->addFile(files[i]);
    }
}

void draax::selectOutputFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Output Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) setRootItem(dir);
}

void draax::setRootItem(QString path)
{
    // remove extra slashes
    path = QDir::cleanPath(path);
    /* replace slashes /->\ for display on windows */
    path = QDir::toNativeSeparators(path);
    QString dirName = QFileInfo(path).fileName();

    tree->rootItem->setText(colName, dirName);
    tree->rootItem->setText(colPath, path);
    tree->rootItem->setStatusTip(colName, dirName);
    tree->rootItem->setToolTip(colName, path);
}

QString draax::sanitize(QString str, MetaType mtype)
{
    QRegExp re;
    QRegExp nonAlpha("[^a-zA-Z0-9_\\.\\-\\(\\) ]");
    QRegExp ws("\\s{2,}");
    QRegExp period("\\.(?=\\S)");
    QStringList list;

    if (mtype == metTitle) {
        str.remove(nonAlpha);
        str = str.trimmed();
        str = str.replace(ws, " ");
        str = str.replace(period, ". ");
        if (setDialog->sanTitle->isChecked()) {
            for (int i = 0, reTotal = titRE.size(); i < reTotal; ++i) {
                re.setPattern(titRE[i]);
                str.remove(re);
            }
        }
    } else if (mtype == metAuthor) {
        list = str.split(",");
        for (int i = 0, total = list.size(); i < total; ++i) {
            if (list[i].isEmpty()) continue;
            list[i].remove(nonAlpha);
            list[i] = list[i].trimmed();
            list[i] = list[i].replace(ws, " ");
            list[i] = list[i].replace(period, ". ");
            if (setDialog->sanAuthor->isChecked()) {
                for (int j = 0, reTotal = autNarRE.size(); j < reTotal; ++j) {
                    re.setPattern(autNarRE[j]);
                    list[i].remove(re);
                }
            }
        }
        str = list.join("||");
    } else {
        // must be metNarrator
        list = str.split(",");
        for (int i = 0, total = list.size(); i < total; ++i) {
            if (list[i].isEmpty()) continue;
            list[i].remove(nonAlpha);
            list[i] = list[i].trimmed();
            list[i] = list[i].replace(ws, " ");
            list[i] = list[i].replace(period, ". ");
            if (setDialog->sanNarrator->isChecked()) {
                for (int j = 0, reTotal = autNarRE.size(); j < reTotal; ++j) {
                    re.setPattern(autNarRE[j]);
                    list[i].remove(re);
                }
            }
        }
        str = list.join("||");
    }
    return str;
}

void draax::getMetaData()
{
    // retrieve only aax items
    QList<QTreeWidgetItem*> list = tree->findItems(".aax", Qt::MatchEndsWith | Qt::MatchRecursive, 1);
    int total = list.size();
    if (total < 1) return;

    QProcess exiftool;
    QByteArray output;
    QString title, author, narrator, format;
    QJsonDocument jsonDoc;
    QJsonArray jsonArray;
    QJsonObject meta;

    for (int i = 0; i < total; ++i) {
        qDebug() << tr("Getting meta-data: %1 of %2").arg(i+1).arg(total);

        // make sure we're only working on items representing aax files
        if (list[i]->data(colName, ItemRole).toInt() != AaxFile) continue;

        // skip items that already have meta data
        if (!list[i]->text(colTitle).isEmpty() &&
            !list[i]->text(colAuthor).isEmpty() &&
            !list[i]->text(colNarrator).isEmpty()) continue;

        exiftool.start(tr("%1 -j -Title -Artist -Author -Narrator -AudioFormat -Codec -Duration \"%2\"").arg(_EXIFTOOL).arg(list[i]->text(colPath)));
        if (!exiftool.waitForStarted(-1) || !exiftool.waitForFinished(-1)) return;

        output = exiftool.readAll();

        jsonDoc = QJsonDocument::fromJson(output);
        if (jsonDoc.isNull() || !jsonDoc.isArray()) continue;

        jsonArray = jsonDoc.array();
        meta = jsonArray.at(0).toObject();

        title = meta["Title"].toString();
        title = sanitize(title, metTitle);

        author = meta["Artist"].toString();
        author = sanitize(author, metAuthor);

        narrator = meta["Narrator"].toString();
        narrator = sanitize(narrator, metNarrator);

        format = meta["AudioFormat"].toString();

        if (title.isEmpty() || author.isEmpty() ||
            narrator.isEmpty() || format.isEmpty()) continue;

        list[i]->setText(colCodec, format);
        list[i]->setText(colTitle, title);
        list[i]->setText(colAuthor, author);
        list[i]->setText(colNarrator, narrator);
        list[i]->setStatusTip(colName, title);
    }
}

void draax::validate()
{
    QStringList actBytesList = setDialog->getActivationBytes();
    if (actBytesList.size() < 1) return;

    // retrieve only aax items
    QList<QTreeWidgetItem*> list = tree->findItems(".aax", Qt::MatchEndsWith | Qt::MatchRecursive, 1);
    int total = list.size();
    if (total < 1) return;

    QProcess ffprobe;
    QString output;
    QString checksum;
    bool isValid;
    QRegularExpression re("\\[aax\\] file checksum == ([a-zA-Z0-9]{40})");
    QRegularExpressionMatch match;

    for (int i = 0; i < total; ++i) {
        qDebug() << tr("Validating activation bytes: %1 of %2").arg(i+1).arg(total);

        // make sure we're only working on items representing aax files
        if (list[i]->data(colName, ItemRole).toInt() != AaxFile) continue;

        // skip items that already have checksum AND activation bytes
        if (!list[i]->text(colChecksum).isEmpty() && !list[i]->text(colActivationBytes).isEmpty()) continue;

        isValid = false;
        for (int j = 0, actTotal = actBytesList.size(); j < actTotal; ++j) {
            output = "", checksum = "";

            ffprobe.start(tr("%1 -activation_bytes %2 -i \"%3\"").arg(_FFPROBE).arg(actBytesList[j]).arg(list[i]->text(colPath)));
            if (!ffprobe.waitForStarted(-1) || !ffprobe.waitForFinished(-1)) return;

            // standard output is empty. must read from standard error.
            output = ffprobe.readAllStandardError();

            // the checksum is displayed in the output everytime we run ffprobe.
            // we only need to save it the first time.
            if (list[i]->text(colChecksum).isEmpty()) {
                match = re.match(output);
                if (match.hasMatch() && match.capturedTexts().size() > 1) {
                    checksum = match.captured(1);
                    list[i]->setText(colChecksum, checksum);
                }
            }

            isValid = !output.contains(QRegExp("\\[aax\\] mismatch in checksums!"));
            if (isValid) {
                list[i]->setText(colActivationBytes, actBytesList[j]);
                list[i]->setIcon(colName, QIcon(":/images/supported48.png"));
                list[i]->setData(colName, AaxRole, Supported);
                break;
            }
        }

        if (!isValid) {
            list[i]->setIcon(colName, QIcon(":/images/unsupported48.png"));
            list[i]->setData(colName, AaxRole, Unsupported);
        }
    }
}

void draax::crack()
{
    // retrieve only aax items
    QList<QTreeWidgetItem*> list = tree->findItems(".aax", Qt::MatchEndsWith | Qt::MatchRecursive, 1);
    int total = list.size();
    if (total < 1) return;

    // it doesn't seem to be possible to specify the full path
    // of the folder containing the rtc files. it doesn't matter
    // if the command is executed from a console window or from
    // this gui. the only way to call rcrack from this program
    // is to change the PWD. that enables us to use "./rcrack ."
    // instead of the full folder path.
    // *** THESE DO NOT WORK! ***
    // rcrack /opt/rcrack -h checksum
    // /opt/rcrack/rcrack /opt/rcrack -h checksum

    QProcess rcrack;
    if (!rcrackPath.isEmpty()) rcrack.setWorkingDirectory(rcrackPath);

    QString output, abytes;
    QRegularExpression re("hex\\:([a-zA-Z0-9]{8})");
    QRegularExpressionMatch match;

    for (int i = 0; i < total; ++i) {
        qDebug() << tr("Cracking: %1 of %2").arg(i+1).arg(total);

        // make sure we're only working on items representing aax files
        if (list[i]->data(colName, ItemRole).toInt() != AaxFile) continue;

        // skip items that don't have a checksum or already have their activation bytes
        if (list[i]->text(colChecksum).isEmpty() || !list[i]->text(colActivationBytes).isEmpty()) continue;

        rcrack.start(tr("./%1 . -h %2").arg(_RCRACK).arg(list[i]->text(colChecksum)));

        // these will timeout if we don't use -1. rcrack will take
        // at least a minute to retrieve the activation bytes.
        if (!rcrack.waitForStarted(-1) || !rcrack.waitForFinished(-1)) return;

        output = rcrack.readAll();
        match = re.match(output);
        if (match.hasMatch() && match.capturedTexts().size() > 1) {
            abytes = match.captured(1);
            setDialog->setActivationBytes(abytes);
            list[i]->setText(colActivationBytes, abytes);
            list[i]->setIcon(colName, QIcon(":/images/supported48.png"));
            list[i]->setData(colName, AaxRole, Supported);
            // most users should only need one activation code for all of
            // their audiobooks. since we just retrieved a new activation
            // code we need to re-validate to hopefully avoid running
            // rcrack again.
            validate();
        }
    }
}

QTreeWidgetItem* draax::createTreeItem(QTreeWidgetItem *twiParent, QString label, QString path, ItemMode imode, QTreeWidgetItem *twiClone)
{
    QList<QTreeWidgetItem*> list = tree->findItems(label, Qt::MatchRecursive);
    for (int i = 0, total = list.size(); i < total; ++i) {
        if (list[i]->parent() == twiParent) return list[i]; // duplicate found
    }

    if (twiClone) {
        twiParent->addChild(twiClone);
        return twiClone;
    }

    QTreeWidgetItem *twi = new QTreeWidgetItem(twiParent, QStringList() << label << path);
    twi->setFlags(Qt::ItemIsEnabled);
    twi->setTextAlignment(colName, Qt::AlignVCenter | Qt::AlignLeft);
    twi->setStatusTip(colName, label);
    twi->setToolTip(colName, path);
    twi->setIcon(colName, QIcon(":/images/folder48.png"));
    twi->setData(colName, ItemRole, imode);
    tree->addTopLevelItem(twi);

    return twi;
}

void draax::genLayout()
{
    // we can't generate the folder layout without an output folder
    QString outputPath = tree->rootItem->text(colPath);
    if (outputPath.isEmpty()) {
        qDebug() << "You must select an output folder!";
        return;
    }

    QTreeWidgetItem *autItem = createTreeItem(tree->rootItem, "Authors", tr("%1%2%3").arg(outputPath).arg(QDir::separator()).arg("Authors"));
    QTreeWidgetItem *bacItem = createTreeItem(tree->rootItem, "Backups", tr("%1%2%3").arg(outputPath).arg(QDir::separator()).arg("Backups"));
    QTreeWidgetItem *narItem = createTreeItem(tree->rootItem, "Narrators", tr("%1%2%3").arg(outputPath).arg(QDir::separator()).arg("Narrators"));
    QTreeWidgetItem *titItem = createTreeItem(tree->rootItem, "Titles", tr("%1%2%3").arg(outputPath).arg(QDir::separator()).arg("Titles"));

    // retrieve only aax items
    QList<QTreeWidgetItem*> aaxList = tree->findItems(".aax", Qt::MatchEndsWith | Qt::MatchRecursive, 1);
    int aaxTotal = aaxList.size();
    if (aaxTotal < 1) return;

    QFileInfo fileInfo;
    QString titStr, autStr, narStr, bacStr, autPath, narPath, titPath;
    QTreeWidgetItem *aaxAutItem, *aaxNarItem, *aaxBacItem;
    QStringList autList, narList;

    for (int i = 0; i < aaxTotal; ++i) {
        qDebug() << tr("Generating layout: %1 of %2").arg(i+1).arg(aaxTotal);

        // make sure we're only working on items representing aax files
        if (aaxList[i]->data(colName, ItemRole).toInt() != AaxFile) continue;

        // skip items that are already in backup folder
        if (aaxList[i]->parent() == bacItem) continue;

        titStr = aaxList[i]->text(colTitle);
        if (titStr.isEmpty()) {
            fileInfo.setFile(aaxList[i]->text(colName));
            titStr = fileInfo.baseName();
        }
        bacStr = aaxList[i]->text(colName);
        autStr = aaxList[i]->text(colAuthor);
        narStr = aaxList[i]->text(colNarrator);

        titPath = tr("%1%2%3%4%5").arg(outputPath).arg(QDir::separator()).arg("Titles").arg(QDir::separator()).arg(titStr);

        if (!autStr.isEmpty()) {
            autList = autStr.split("||");
            for (int j = 0, autTotal = autList.size(); j < autTotal; ++j) {
                if (autList[j].isEmpty()) continue;
                autPath = tr("%1%2%3%4%5").arg(outputPath).arg(QDir::separator()).arg("Authors").arg(QDir::separator()).arg(autList[j]);
                aaxAutItem = createTreeItem(autItem, autList[j], autPath);
                createTreeItem(aaxAutItem, titStr, titPath, SymLink);
            }
        }

        if (!narStr.isEmpty()) {
            narList = narStr.split("||");
            for (int j = 0, narTotal = narList.size(); j < narTotal; ++j) {
                if (narList[j].isEmpty()) continue;
                narPath = tr("%1%2%3%4%5").arg(outputPath).arg(QDir::separator()).arg("Narrators").arg(QDir::separator()).arg(narList[j]);
                aaxNarItem = createTreeItem(narItem, narList[j], narPath);
                createTreeItem(aaxNarItem, titStr, titPath, SymLink);
            }
        }

        createTreeItem(titItem, titStr, titPath);

        aaxBacItem = aaxList[i]->clone();
        createTreeItem(bacItem, bacStr, "", AaxFile, aaxBacItem);
        aaxList[i]->parent()->removeChild(aaxList[i]);
    }
}

bool draax::validateParentDirs(QString path)
{
    bool success = false;

    QFileInfo finfo(path);

    QDir dir;
    dir.setPath(finfo.dir().path());
    success = dir.exists();

    if (!success) {
        success = dir.mkpath(dir.path());
    }
    return success;
}

void draax::encode()
{
    // we can't continue without an output folder
    QString outputPath = tree->rootItem->text(colPath);
    if (outputPath.isEmpty()) {
        qDebug() << "You must select an output folder!";
        return;
    }

    // retrieve all items
    QList<QTreeWidgetItem*> list = tree->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int total = list.size();
    if (total < 1) return;

    // get the default output format
    QString format = setDialog->containerMenu->currentText();

    // is atomicparsley installed?
    QStringList nf = hasApps(QStringList() << _ATOMICPARSLEY);
    bool enableATOMPARS = (!nf.size());

    // only these non-alphanumeric characters are allowed [.-_() ]
    QRegExp re("[^a-zA-Z0-9_\\.\\-\\(\\) ]");

    int imode;
    QDir dir;
    QFileInfo finfo;
    QProcess ffmpeg, exiftool, atompars;
    QString parPath, realPath, fakePath, abytes, titStr, outFilePath, bacPath, coverPath;

    for (int i = 0; i < total; ++i) {
        qDebug() << tr("Creating folders/Extracting covers/Encoding: %1 of %2").arg(i+1).arg(total);

        imode = list[i]->data(colName, ItemRole).toInt();
        if (imode == AaxFile) {
            abytes = list[i]->text(colActivationBytes);
            realPath = list[i]->text(colPath);
            if (abytes.isEmpty() || realPath.isEmpty()) continue;

            // use title meta data as file name. fall back to
            // original aax file name if no meta data.
            titStr = list[i]->text(colTitle);
            if (titStr.isEmpty()) {
                finfo.setFile(list[i]->text(colName));
                titStr = finfo.baseName();
            }
            titStr.remove(re);

            // set the full path to output file
            outFilePath = tr("%1%2%3%4%5%6%7.%8").arg(outputPath).arg(QDir::separator()).arg("Titles").arg(QDir::separator()).arg(titStr).arg(QDir::separator()).arg(titStr).arg(format);
            if (!validateParentDirs(outFilePath)) {
                qDebug() << "Cannot create output folders! Exiting!" << outFilePath;
                return;
            }

            // set the full path to cover image
            coverPath = tr("%1%2%3%4%5%6cover.jpg").arg(outputPath).arg(QDir::separator()).arg("Titles").arg(QDir::separator()).arg(titStr).arg(QDir::separator());

            // set the full path to the backup file
            bacPath = tr("%1%2%3%4%5").arg(outputPath).arg(QDir::separator()).arg("Backups").arg(QDir::separator()).arg(list[i]->text(colName));
            if (!validateParentDirs(bacPath)) {
                qDebug() << "Cannot create output folders! Exiting!" << bacPath;
                return;
            }

            ffmpeg.start(tr("%1 -activation_bytes %2 -i \"%3\" -vn -sn -c:a copy \"%4\"").arg(_FFMPEG).arg(abytes).arg(realPath).arg(outFilePath));

            // these will timeout if we don't use -1
            if (!ffmpeg.waitForStarted(-1) || !ffmpeg.waitForFinished(-1)) return;

            // test file size of output file to determine success.
            // temporarily using 1kb as indicator of success.
            // TODO: compare input/output duration to determine success.
            finfo.setFile(outFilePath);
            if (!finfo.exists() || finfo.size() < 1000) {
                qDebug() << "The file conversion was not successful. Exiting!" << realPath << outFilePath << abytes;
                return;
            }

            // extract cover image
            exiftool.setStandardOutputFile(coverPath, QIODevice::Truncate);
            exiftool.start(tr("%1 -b -CoverArt %2").arg(_EXIFTOOL).arg(realPath));
            if (!exiftool.waitForStarted(-1) || !exiftool.waitForFinished(-1)) return;

            finfo.setFile(coverPath);
            if (!finfo.exists() || !finfo.size()) {
                qDebug() << "Could not find cover image!" << coverPath;
            } else if (enableATOMPARS && (format == "m4a" || format == "mp4")) {
                // embed cover image in output file
                atompars.start(tr("%1 \"%2\" --artwork \"%3\" --overWrite").arg(_ATOMICPARSLEY).arg(outFilePath).arg(coverPath));
                if (!atompars.waitForStarted(-1) || !atompars.waitForFinished(-1)) return;
            }

            finfo.setFile(bacPath);
            if (!finfo.exists()) {
                if (!dir.rename(realPath, bacPath)) {
                    qDebug() << "Cannot move file to Backups! Exiting!" << realPath << bacPath;
                    return;
                }
            } else {
                qDebug() << "The file already exists in Backups!" << bacPath;
            }
            continue;
        }

        if (imode == Folder || imode == Root) {
            realPath = list[i]->text(colPath);
            if (realPath.isEmpty()) continue;

            dir.setPath(realPath);
            if (!dir.exists()){
                if (!dir.mkpath(dir.path())) {
                    qDebug() << "The folder could not be created.  Exiting!" << dir.path();
                    return;
                }
            }
            continue;
        }

        if (imode == SymLink) {
            parPath = list[i]->parent()->text(colPath);
            realPath = list[i]->text(colPath);
            fakePath = tr("%1%2%3").arg(parPath).arg(QDir::separator()).arg(list[i]->text(colName));
            if (parPath.isEmpty() || realPath.isEmpty()) continue;

            // make sure parent exists
            dir.setPath(parPath);
            if (!dir.exists()){
                if (!dir.mkpath(dir.path())) {
                    qDebug() << "The folder could not be created.  Exiting!" << dir.path();
                    return;
                }
            }

            // make sure path to symlink target exists
            dir.setPath(realPath);
            if (!dir.exists()){
                if (!dir.mkpath(dir.path())) {
                    qDebug() << "The folder could not be created.  Exiting!" << dir.path();
                    return;
                }
            }

            // IF WINDOWS SUPPORT IS ADDED IN A FUTURE VERSION THE fakePath STRING MUST
            // END WITH THE EXTENSION .lnk

            // make sure symlink does NOT exist
            finfo.setFile(fakePath);
            if (!finfo.exists()) {
                // create the symlink
                if (!QFile::link(realPath, fakePath)) {
                    qDebug() << "The symlink could not be created. Exiting!" << realPath << fakePath;
                    return;
                }
            }
        }
    }
}

// returns names of apps not found OR list of found paths
QStringList draax::hasApps(QStringList apps, QStringList pathsToSearch, bool returnFound)
{
    QString path;
    QStringList found;
    QStringList notFound;
    for (int i = 0, total = apps.size(); i < total; ++i) {
        if (pathsToSearch.size() > 0) {
            path = QStandardPaths::findExecutable(apps[i], pathsToSearch);
        } else {
            path = QStandardPaths::findExecutable(apps[i]);
        }
        if (path.isEmpty()) {
            notFound << apps[i];
        } else {
            found << path;
        }
    }
    return (returnFound) ? found : notFound;
}

void draax::wizard()
{
    QMessageBox::information(this, tr("ALERT!"),
                                   tr("This application is merely a functional demo. Some blocking code was used for calling the external applications so it will appear at times as if your interface has frozen. Please run this program from a console window. You will be alerted through the console of any errors encountered. Another message box will be shown when all operations are completed.  There is no progress indicator so the final popup alert is the only way to know when the operations have finished. Be patient during this process and don't close the application."),
                                   QMessageBox::Ok);

    // retrieve only aax items
    QList<QTreeWidgetItem*> list = tree->findItems(".aax", Qt::MatchEndsWith | Qt::MatchRecursive, 1);
    int total = list.size();
    if (total < 1) {
        addFile();
        list = tree->findItems(".aax", Qt::MatchEndsWith | Qt::MatchRecursive, 1);
        total = list.size();
        if (total < 1) {
            qDebug() << "You must select at least one aax file to continue!";
            return;
        }
    }

    if (tree->rootItem->text(colPath).isEmpty()) {
        selectOutputFolder();
        if (tree->rootItem->text(colPath).isEmpty()) {
            qDebug() << "You must select an output folder to continue!";
            return;
        }
    }

    if (metAction->isEnabled()) getMetaData();
    if (valAction->isEnabled()) validate();
    if (craAction->isEnabled()) crack();
    if (genAction->isEnabled()) genLayout();
    if (encAction->isEnabled()) encode();

    QMessageBox::information(this, tr("ALERT!"),
                                   tr("All operations completed! Please check the console output for errors."),
                                   QMessageBox::Ok);
}

TreeWidget::TreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{

}

void TreeWidget::addFile(QString path)
{
    if (path.right(4) != ".aax") return; // only aax files supported

    // remove extra slashes
    path = QDir::cleanPath(path);
    /* replace slashes /->\ for display on windows */
    path = QDir::toNativeSeparators(path);
    QString fileName = QFileInfo(path).fileName();

    QList<QTreeWidgetItem*> list = this->findItems(fileName, Qt::MatchRecursive);
    if (list.size() > 0) return; // duplicate found so skip it

    QTreeWidgetItem *twi = new QTreeWidgetItem(this->rootItem, QStringList() << fileName << path);
    twi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren);
    twi->setTextAlignment(draax::colName, Qt::AlignVCenter | Qt::AlignLeft);
    twi->setStatusTip(draax::colName, fileName);
    twi->setToolTip(draax::colName, path);
    twi->setIcon(draax::colName, QIcon(":/images/unknown48.png"));
    twi->setData(draax::colName, draax::ItemRole, draax::AaxFile);
    twi->setData(draax::colName, draax::AaxRole, draax::Unknown);

    this->addTopLevelItem(twi);
}

void TreeWidget::removeSelected()
{
    QList<QTreeWidgetItem*> items = this->selectedItems();
    for (int i = 0, total = items.size(); i < total; ++i) {
        items[i]->parent()->removeChild(items[i]);
    }
}

void TreeWidget::removeAll()
{
    while (this->rootItem->childCount() > 0) {
        this->rootItem->removeChild(this->rootItem->child(0));
    }
}

void TreeWidget::dropEvent(QDropEvent *event)
{
    QModelIndex modelIndex = this->indexAt(event->pos());
    const QMimeData *mimeData = event->mimeData();
    event->setDropAction(Qt::IgnoreAction);

    if (!modelIndex.isValid() && !mimeData->hasUrls()) {
        event->ignore();
        return;
    }

    if (mimeData->hasUrls()) {
        QString path;
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0, total = urlList.size(); i < total; ++i) {
            path = urlList[i].toLocalFile();
            this->addFile(path);
        }
    }

    event->accept();
}

void TreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void TreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void TreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

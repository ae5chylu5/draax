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

#ifndef DRAAX_H
#define DRAAX_H

#include <QAction>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QStandardPaths>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtEvents>

#include "settings.h"
#include "about.h"

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget *parent = 0);
    void addFile(QString path);
    QTreeWidgetItem *rootItem;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void removeAll();
    void removeSelected();
};

class draax : public QMainWindow
{
    Q_OBJECT

public:
    draax(QWidget *parent = 0, bool devEnabled = false);
    ~draax();

    QString rcrackPath;
    QAction *craAction;

    enum Columns { colName, colPath, colChecksum, colActivationBytes, colCodec, colTitle, colAuthor, colNarrator };
    enum MetaType { metTitle, metAuthor, metNarrator };
    enum AaxMode { Unsupported, Supported, Unknown };
    enum ItemMode { Root, Folder, AaxFile, SymLink, Chapter };
    // Qt::UserRole starts at 0x0100 so we define our custom
    // roles beginning at 101
    enum DataRoles { ItemRole = 0x0101, AaxRole };

private:
    QString sanitize(QString str, MetaType mtype);
    void createToolBars(bool devEnabled = false);
    void createStatusBar();
    void createTreeWidget();
    void connectEvents();
    bool validateParentDirs(QString path);
    QTreeWidgetItem* createTreeItem(QTreeWidgetItem *twiParent, QString label, QString path = "", ItemMode imode = Folder, QTreeWidgetItem *twiClone = 0);
    QStringList hasApps(QStringList apps, QStringList pathsToSearch = QStringList(), bool returnFound = false);

    Settings *setDialog;
    About *abtDialog;

    TreeWidget *tree;

    QToolBar *topToolBar;
    QToolBar *botToolBar;
    QStatusBar *statusBar;

    QAction *outAction;
    QAction *metAction;
    QAction *valAction;
    QAction *genAction;
    QAction *encAction;
    QAction *wizAction;
    QAction *setAction;
    QAction *abtAction;
    QAction *inpAction;
    QAction *remAction;
    QAction *rmaAction;

#ifdef Q_OS_WIN
    const QString _EXIFTOOL = "exiftool.exe";
    const QString _FFPROBE = "ffprobe.exe";
    const QString _FFMPEG = "ffmpeg.exe";
    const QString _RCRACK = "rcrack.exe";
    const QString _ATOMICPARSLEY = "atomicparsley.exe";
#else
    const QString _EXIFTOOL = "exiftool";
    const QString _FFPROBE = "ffprobe";
    const QString _FFMPEG = "ffmpeg";
    const QString _RCRACK = "rcrack";
    const QString _ATOMICPARSLEY = "atomicparsley";
#endif

    QStringList reqApps;
    QStringList autNarRE;
    QStringList titRE;

private slots:
    void setRootItem(QString path);
    void addFile();
    void toggleButtonAccess();
    void selectOutputFolder();
    void getMetaData();
    void validate();
    void crack();
    void genLayout();
    void encode();
    void wizard();
};

#endif // DRAAX_H

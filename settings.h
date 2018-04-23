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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QSettings>
#include <QTextEdit>
#include <QToolButton>
#include <QtEvents>

class Settings : public QDialog
{
    Q_OBJECT

public:
    Settings(QWidget *parent = 0);
    QStringList getActivationBytes();
    void setActivationBytes(QString abytes);

    QLineEdit *outputPath;
    QTextEdit *actBytesText;
    QComboBox *containerMenu;
    QCheckBox *sanTitle;
    QCheckBox *sanAuthor;
    QCheckBox *sanNarrator;

private:
    void saveSettings();
    void importSettings();
    void connectEvents();

    QToolButton *outputBtn;
    QStringList supportedFormats;
    QStringList excTitStr;
    QStringList excAutStr;
    QStringList excNarStr;

private slots:
    void selectOutput();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
};

#endif // SETTINGS_H

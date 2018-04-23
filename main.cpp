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
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QCommandLineParser parser;
    QCommandLineOption rcrackOption(QStringList() << "r" << "path",
                                    "Path to rcrack executable. Rtc files must be located in the same folder.",
                                    "path");
    parser.addOption(rcrackOption);
    QCommandLineOption devOption(QStringList() << "d",
                                 "Enable additional toolbar buttons for devs.");
    parser.addOption(devOption);
    parser.process(a);

    QString rcrackPath = parser.value(rcrackOption);
    bool devEnabled = parser.isSet(devOption);
    
    bool disableRcrack = true;

    QFileInfo fileInfo;

    if (!rcrackPath.isEmpty()) {
        fileInfo.setFile(rcrackPath);
        if (fileInfo.exists() && fileInfo.isFile() && fileInfo.isExecutable()) {
            // we currently set the PWD from the crack() method in draax.cpp
            //disableRcrack = !QDir::setCurrent(rcrackPath);
            disableRcrack = false;
            qDebug() << "Rcrack features are now enabled!";
        }
    }

    draax w(0, devEnabled);

    w.craAction->setDisabled(disableRcrack);
    w.rcrackPath = (disableRcrack) ? "" : fileInfo.dir().path();

    w.show();

    return a.exec();
}

#********************************************************************
#
# Name: Draax
# Homepage: http://github.com/ae5chylu5/draax
# Author: ae5chylu5
# Description: A front end for converting aax files
#
# Copyright (C) 2017 ae5chylu5
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#********************************************************************

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = draax
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
           draax.cpp \
           settings.cpp \
           about.cpp

HEADERS += draax.h \
           settings.h \
           about.h

RESOURCES += draax.qrc

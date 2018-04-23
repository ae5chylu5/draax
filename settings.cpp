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

#include "settings.h"

Settings::Settings(QWidget *parent)
    : QDialog(parent)
{
    int minWidth = 250;
    int minHeight = 300;

    supportedFormats << "3gp" << "avi" << "flv" << "m4a" << "mkv" << "mov" << "mp4" << "wma";

    excTitStr << "unabridged" << "abridged";
    excAutStr << "translator" << "editor" << "introduction" << "foreword";
    excNarStr << "introduction" << "foreword";

    QLabel *outputLabel = new QLabel(tr("Output folder"));
    outputLabel->setStyleSheet(tr("QLabel { font-weight:700; }"));

    outputPath = new QLineEdit();
    outputPath->setReadOnly(1);
    outputPath->setWhatsThis(tr("This will be used as the default output folder. You can change it on a per use basis in the main window."));

    outputBtn = new QToolButton();
    outputBtn->setIcon(QIcon(":/images/output48.png"));
    outputBtn->setFixedSize(QSize(24,24));
    outputBtn->setToolTip("Select output folder");

    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->addWidget(outputPath, 1);
    outputLayout->addWidget(outputBtn);

    QLabel *containerLabel = new QLabel(tr("Output container"));
    containerLabel->setStyleSheet(tr("QLabel { font-weight:700; }"));

    containerMenu = new QComboBox();
    containerMenu->addItems(supportedFormats);
    containerMenu->setWhatsThis(tr("This will be used as the output format. The aac stream will be copied into the container you choose."));

    QHBoxLayout *containerLayout = new QHBoxLayout();
    containerLayout->addWidget(containerMenu);
    containerLayout->addStretch(1);

    sanTitle = new QCheckBox(tr("Sanitize titles"));
    sanTitle->setWhatsThis(tr("Checking this option will remove (%1) from all titles.").arg(excTitStr.join(", ")));

    sanAuthor = new QCheckBox(tr("Sanitize authors"));
    sanAuthor->setWhatsThis(tr("Checking this option will remove (%1) from the author names.").arg(excAutStr.join(", ")));

    sanNarrator = new QCheckBox(tr("Sanitize narrators"));
    sanNarrator->setWhatsThis(tr("Checking this option will remove (%1) from the narrator names.").arg(excNarStr.join(", ")));

    QLabel *actBytesLabel = new QLabel(tr("Activation bytes"));
    actBytesLabel->setStyleSheet(tr("QLabel { font-weight:700; }"));

    actBytesText = new QTextEdit();
    actBytesText->setWhatsThis(tr("Enter activation bytes. One per line. Max 8 characters per line."));

    QVBoxLayout *vboxLayout = new QVBoxLayout();
    vboxLayout->addWidget(outputLabel);
    vboxLayout->addLayout(outputLayout);
    vboxLayout->addWidget(containerLabel);
    vboxLayout->addLayout(containerLayout);
    vboxLayout->addWidget(sanTitle);
    vboxLayout->addWidget(sanAuthor);
    vboxLayout->addWidget(sanNarrator);
    vboxLayout->addWidget(actBytesLabel);
    vboxLayout->addWidget(actBytesText, 1);
    setLayout(vboxLayout);

    setWindowTitle("Settings");
    setWindowIcon(QIcon(":/images/settings48.png"));
    resize(minWidth, minHeight);

    importSettings();
    connectEvents();
}

void Settings::connectEvents()
{
    connect(outputBtn, SIGNAL(clicked()), this, SLOT(selectOutput()));
}

void Settings::selectOutput()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Output Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) outputPath->setText(dir);
}

void Settings::saveSettings()
{
    QStringList actBytesList = getActivationBytes();
    actBytesText->setPlainText(actBytesList.join("\n"));

    QSettings settings("Draax", "Draax");
    settings.setValue("settings/activationBytes", actBytesText->toPlainText());
    settings.setValue("settings/outputPath", outputPath->text());
    settings.setValue("settings/outputFormat", containerMenu->currentIndex());
    settings.setValue("settings/sanTitle", sanTitle->isChecked());
    settings.setValue("settings/sanAuthor", sanAuthor->isChecked());
    settings.setValue("settings/sanNarrator", sanNarrator->isChecked());
}

void Settings::importSettings()
{
    QSettings settings("Draax", "Draax");
    actBytesText->setPlainText(settings.value("settings/activationBytes").toString());
    outputPath->setText(settings.value("settings/outputPath").toString());
    containerMenu->setCurrentIndex(settings.value("settings/outputFormat").toInt());
    sanTitle->setChecked(settings.value("settings/sanTitle").toBool());
    sanAuthor->setChecked(settings.value("settings/sanAuthor").toBool());
    sanNarrator->setChecked(settings.value("settings/sanNarrator").toBool());
}

void Settings::setActivationBytes(QString abytes)
{
    actBytesText->append("\n" + abytes);
    saveSettings();
}

QStringList Settings::getActivationBytes()
{
    QString actBytes = actBytesText->toPlainText();
    QStringList actBytesList = actBytes.split(QRegExp("\\s"));

    // we need to call size() each time through the loop since
    // list size may change.
    for (int i = 0; i < actBytesList.size(); ++i) {
        actBytesList[i].remove(QRegExp("[^a-zA-Z0-9]"));
        if (actBytesList[i].size() < 8) {
            actBytesList.removeAt(i);
            --i;
        }
    }

    actBytesList.removeDuplicates();

    return actBytesList;
}

void Settings::showEvent(QShowEvent *event)
{
    importSettings();
    event->accept();
}

void Settings::hideEvent(QHideEvent *event)
{
    saveSettings();
    event->accept();
}

/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2025 ORPAL Technology, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QProcess>
#include <QScreen>
#include <QSysInfo>
#include <QStorageInfo>
#include "aapi_about_app_view.h"
#include "aapi_application.h"


///////////////////////////////////////////////////////////////////////////////
// class QAAPiAboutAppView
///////////////////////////////////////////////////////////////////////////////


QAAPiAboutAppView::QAAPiAboutAppView(AAPiConfig *config, QObject *parent)
    : QAAPiViewBackend(config, nullptr, nullptr, parent)
{
}

void QAAPiAboutAppView::quit()
{
    emit quitApplication();
}

void QAAPiAboutAppView::reboot()
{
    emit rebootApplication();
}

QString QAAPiAboutAppView::getSystemInformation() const
{
    QString text;

    QString memInfo;
    QProcess p;
    p.start("awk", QStringList() << "/MemAvailable/ { print $2 }" << "/proc/meminfo");
    p.waitForFinished();
    memInfo = QString::number(p.readAllStandardOutput().toUInt() / 1024) + "MB / ";
    p.close();
    p.start("awk", QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
    p.waitForFinished();
    memInfo += QString::number(p.readAllStandardOutput().toUInt() / 1024) + "MB";
    p.close();

    QStorageInfo stg = QStorageInfo::root();
    QString diskInfo = stg.fileSystemType() + " | " + QString::number(stg.bytesAvailable()/1000/1000) + "MB / " + QString::number(stg.bytesTotal()/1000/1000) + "MB";

    QScreen *scr = QApplication::primaryScreen();
    QString scrInfo = scr->name() + " | ";
    scrInfo += QString::number(scr->availableSize().width()) + "x" + QString::number(scr->availableSize().height()) + "@";
    scrInfo += QVariant(scr->refreshRate()).toString() + "Hz | ";
    scrInfo += QString::number(scr->physicalDotsPerInch(), 'f', 2) + "dpi";

    QAAPiApplication *aapi = qobject_cast<QAAPiApplication *>(parent( ));

    text =  "System:\t" + QSysInfo::prettyProductName() + "\n" +
            "Kernel:\t" + QSysInfo::kernelVersion() + "\n" +
            "CPU:   \t" + QSysInfo::buildCpuArchitecture() + " | " + QSysInfo::buildAbi() + "\n" +
            "GPU:   \t" + aapi->getGraphicsDeviceInfo() + "\n" +
            "RAM:   \t" + memInfo + "\n" +
            "Disk:  \t" + diskInfo + "\n" +
            "Screen:\t" + scrInfo;

    return text;
}

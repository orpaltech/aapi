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

#ifndef UI_AAPI_ABOUT_APP_VIEW_H
#define UI_AAPI_ABOUT_APP_VIEW_H

#include "aapi_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiAboutAppView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiAboutAppView class
///
class QAAPiAboutAppView : public QAAPiViewBackend
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(AboutAppViewBackend)

    /* Properties */
    Q_PROPERTY(QString  system_information  READ    getSystemInformation CONSTANT)

public:
    explicit QAAPiAboutAppView(AAPiConfig *config, QObject *parent = Q_NULLPTR);

    // --------- Property accessors -------
    QString getSystemInformation() const;

signals:

public slots:
    void quit();
    void reboot();
};

#endif // UI_AAPI_ABOUT_APP_VIEW_H

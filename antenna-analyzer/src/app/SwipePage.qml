/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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

import QtQuick
import QtQuick.Controls
import aapi
import ru.orpaltech.aapi


Page {
    id: root

    property ViewBackend backend
    property int viewStatus: -1
    property bool isPageActive: false

    contentHeight: 656
    contentWidth: 1280

    anchors.margins: 0
    anchors.fill: parent


    // Trigger the events directly off the property change
    onIsPageActiveChanged: {
        if (isPageActive) {
            if (backend)
                backend.handleActivated()
            activated()
        } else {
            if (backend)
                backend.handleDeactivated()
            deactivated()
        }
   }

    /*
     * Signal declaration
     */
    signal loaded()
    signal activated()
    signal deactivated()


    Component.onCompleted: {
        if (backend) {
            backend.handleLoaded()
        }
        viewStatus = ViewBackend.VS_IDLE
        loaded()

        // Handle initial activation if it loads directly into view
        if (isPageActive) {
           if (backend)
               backend.handleActivated()
           activated()
        }
    }

    Component.onDestruction: {
        // If the app is closing while this page is actively running,
        // force a clean deactivation event sequence first.
        if (isPageActive) {
            if (backend) {
                backend.handleDeactivated()
            }
            deactivated()
        }

        if (backend) {
            backend.handleDestroyed()
        }
    }
}

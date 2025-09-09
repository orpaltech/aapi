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


import QtQuick
import QtQuick.Controls
import aapi
import ru.orpaltech.aapi


Page {
    property ViewBackend backend
    property int viewStatus: -1

    contentHeight: 656
    contentWidth: 1280

    anchors.margins: 0
    anchors.fill: parent


    /*
     * Signal declaration
     */
    signal loaded()
    signal activated()
    signal deactivated()

    onActivated: {
        backend.activated()
    }
    onDeactivated: {
        backend.deactivated()
    }

    Component.onCompleted: {
        backend.loaded()
        viewStatus = ViewBackend.VS_IDLE
        loaded()
    }

    Component.onDestruction: {
        backend.destroyed()
    }
}

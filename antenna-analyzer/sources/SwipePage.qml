/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-PI software.
 *
 * 	ORPAL-AA-PI is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-PI is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-PI. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQuick.Controls 2.2
import com.orpaltech.aapi 1.0

Page {
    property ViewBackend backend
    property int viewStatus: -1

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

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

MeasurementViewForm {

    btnErrorClose.onClicked: {
        popupError.close()
    }

    /*
    onActivated: {
        if (backend.activated() !== 0){
            //Error occurred
            labelError.text = backend.error_message
            //popupError.open()
        } else {
            //popupError.close()
        }
    }
    onDeactivated: {
        backend.deactivated()
    }

    onLoaded: {
        if (backend.loaded() !== 0){
            //Error occurred
            labelError.text = backend.error_message
            //popupError.open()
        } else {
            //popupError.close()
        }
    }

    Component.onCompleted: {
        loaded()
    }

    Component.onDestruction: {
        backend.destroyed()
    }*/
}

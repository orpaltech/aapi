/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import com.orpaltech.aapi 1.0

Item {
    property ConfigViewBackend backend: analyzer.config

    id: itemControl
    anchors.fill: parent

    Frame {
        id: frameControl
        anchors.fill: parent

        ColumnLayout {
            id: layoutControl
            anchors.fill: parent

            Label {
                id: labelTitle
                text: "Here will be the parameter title"
                font.pointSize: 14
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            ComboBox {
                id: cbOptions
                topPadding: 7
                textRole: "text"
                model: ListModel {
                    id:cbOptionsModel
                }
                font.pointSize: 16
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            }
        }

    }

    Connections {
        target: cbOptions
        onCurrentIndexChanged: {
            backend.set_param_option(cbOptions.currentIndex)
        }
    }

    Component.onCompleted: {
        console.log("param enum loaded")
        labelTitle.text = backend.param_description
        var opt_labels = backend.param_opt_labels
        var opt_values = backend.param_opt_values
        var cur_sel = 0
        cbOptions.model.clear()
        for (var i = 0; i < backend.param_num_options; i++) {
            var el = { text: opt_labels[i], value: opt_values[i] };
            cbOptions.model.append(el);
            if (backend.param_value === opt_values[i]) {
                cur_sel = i;
            }
        }
        cbOptions.currentIndex = cur_sel;
    }

}

/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

SwipePage {
    backend: analyzer.config

    id: itemConfig

    SwipeView {
        property int previousIndex: -1
        id: swipeConfig
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: indConfig.top
        }
        orientation: Qt.Vertical

        Repeater {
            id: repeaterConfig
            model: backend.num_params
            Loader {
                active: SwipeView.isCurrentItem
                source: createParamViewUrl()
            }
        }

    }

    PageIndicator {
        id: indConfig
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        count: swipeConfig.count
        currentIndex: swipeConfig.currentIndex

        delegate: Rectangle {
            implicitWidth: 10
            implicitHeight: 10
            radius: width/2
            color: "#21be2b"
            opacity: index === indConfig.currentIndex ? 0.95 : pressed ? 0.7 : 0.45
            Behavior on opacity {
                OpacityAnimator {
                    duration: 100
                }
            }
        }
    }

    Connections {
        target: swipeConfig
        onCurrentIndexChanged: {
            console.log("config swipe view: current = " + swipeConfig.currentIndex + ", previous:" + swipeConfig.previousIndex);
            if (swipeConfig.currentIndex < 0) {
                // all pages have been removed
                swipeConfig.previousIndex = -1;
            } else {
                if (swipeConfig.previousIndex < 0) {
                    backend.move_first_param();
                } else {
                    if (swipeConfig.currentIndex > swipeConfig.previousIndex) {
                        // move next
                        backend.move_next_param()
                    } else {
                        // move prev
                        backend.move_prev_param()
                    }
                }
                var loader = swipeConfig.itemAt(swipeConfig.currentIndex);
                loader.source = createParamViewUrl()
                swipeConfig.previousIndex = swipeConfig.currentIndex
            }
        }
    }

    function createParamViewUrl() {
        return backend.param_num_options > 0 ? "ParamEnumView.qml" : "ParamTextView.qml"
    }
}

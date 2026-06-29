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
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

SwipePage {
    id: swpConfig
    title: qsTr("Configuration")
    backend: aapi.view_configuration
    
    ColumnLayout {
        id: col1
        width: 1280
        height: 656
        spacing: 0
        
        RowLayout {
            id: row1
            spacing: 0
            
            Frame {
                id: frmConfig
                contentHeight: 602
                contentWidth: 1262
                
                ToolBar {
                    id: toolBar1
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    contentHeight: 47
                    contentWidth: 1250
                    font.pointSize: 16
                    font.bold: false
                    
                    ToolButton {
                        id: btnDiscardChanges
                        width: 280
                        text: qsTr("Reload")
                        font.bold: false
                        font.pointSize: 22
                        
                        DesignEffect {
                            effects: [ DesignDropShadow { } ]
                        }
                    }
                    
                    
                    ToolButton {
                        id: btnUseDefaults
                        x: 282
                        width: 280
                        text: qsTr("Use Defaults")
                        font.bold: false
                        font.pointSize: 22
                        
                        DesignEffect {
                            effects: [ DesignDropShadow { } ]
                        }
                    }

                    ToolButton {
                        id: btnAcceptChanges
                        x: 564
                        width: 280
                        text: qsTr("Accept Changes")
                        font.bold: false
                        font.pointSize: 22
                        
                        DesignEffect {
                            effects: [ DesignDropShadow { } ]
                        }
                    }
                }
                
                SwipeView {
                    id: svConfigViews
                                        
                    anchors.fill: parent
                    anchors.topMargin: 48
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    
                    property int previousIndex: 0

                    Repeater {
                        model: getNumParams()
                        Loader {
                            // keep only one page instantiated as the backend status is updated dynamically
                            active: SwipeView.isCurrentItem
                            source: getParamViewUrl()
                        }
                    }
                    
                    onCurrentIndexChanged: {
                        console.log("config swipe view: current = " + svConfigViews.currentIndex + ", previous:" + svConfigViews.previousIndex);
                        if (svConfigViews.currentIndex < 0) {
                            // all pages have been removed
                            svConfigViews.previousIndex = 0;
                            return;
                        }
                        
                        var direction = svConfigViews.currentIndex - svConfigViews.previousIndex;
                        
                        if (svConfigViews.currentIndex === 0) {
                            onMoveFirstParam();
                        } else {
                            if (direction > 0) {
                                onMoveNextParam();
                            } else {
                                onMovePrevParam();
                            }
                        }
                        
                        // save current index as previous for the next call
                        svConfigViews.previousIndex = svConfigViews.currentIndex;
                        
                        var viewItem = svConfigViews.itemAt(svConfigViews.currentIndex);
                        viewItem.source = getParamViewUrl();
                    }
                }
            }
        }
        
        RowLayout {
            id: row2
            spacing: 0
            
            Item {
                width: 1280
                height: 36
                
                PageIndicator {
                    id: pageIndicator
                    anchors.horizontalCenter: parent.horizontalCenter
                    bottomPadding: 4
                    topPadding: 4
                    padding: 0
                    spacing: 8
                    count: getNumParams()
                    currentIndex: svConfigViews.currentIndex
                    
                    delegate: Rectangle {
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: width/2
                        color: "#05789c"
                        border.color: "#022936"
                        border.width: 2
                        opacity: index == pageIndicator.currentIndex ? 0.95 : pressed ? 0.7 : 0.45
                        
                        Behavior on opacity {
                            OpacityAnimator {
                                duration: 100
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: btnUseDefaults
        onClicked: {
            backend.handleUseDefaults()
        }
    }

    Connections {
        target: btnDiscardChanges
        onClicked: {
            backend.handleDiscardChanges()
        }
    }

    Connections {
        target: btnAcceptChanges
        onClicked: {
            backend.handleAcceptChanges()
        }
    }
    
    function getNumParams() {
        if (AAPI_DESIGN_MODE) {
            // testing
            return 3;
        }
        
        return backend.num_params;
    }
    
    function onMoveFirstParam() {
        if (AAPI_DESIGN_MODE)
            return;
        
        // move first
        backend.handleMoveFirstParam();
    }
    
    function onMovePrevParam() {
        if (AAPI_DESIGN_MODE)
            return;
        
        // move prev
        backend.handleMovePrevParam();
    }
    
    function onMoveNextParam() {
        if (AAPI_DESIGN_MODE)
            return;
        
        // move next
        backend.handleMoveNextParam();
    }
    
    function getParamViewUrl() {
        if (AAPI_DESIGN_MODE)
            return "ViewParamText.qml";
        
        return backend.param_num_options > 0 ? "ViewParamEnum.qml" : "ViewParamText.qml";
    }
}

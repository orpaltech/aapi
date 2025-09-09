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
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

/**
 * Signal Processing View
 */
SwipePage {
    id: swpSignalProcess
    title: qsTr("DSP View")
    backend: aapi.view_signal_process

    ColumnLayout {
        id: col1
        spacing: 0

        RowLayout {
            id: row1
            spacing: 0

            TabBar {
                id: tabBarDSP
                position: TabBar.Header
                contentHeight: 36
                contentWidth: 1280

                TabButton {
                    id: btnSpectrum
                    width: 165
                    text: qsTr("Spectrum")
                    font.pointSize: 19
                    rightPadding: 6
                    leftPadding: 6

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }

                TabButton {
                    id: btnOscilloscope
                    width: 165
                    text: qsTr("Oscilloscope")
                    font.pointSize: 19
                    rightPadding: 6
                    leftPadding: 6

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }
            }
        }

        RowLayout {
            id: row2
            spacing: 0

            StackLayout {
                id: tabsDSP
                width: 1280
                height: 620
                currentIndex: tabBarDSP.currentIndex

                Loader {
                    id: ldrSpectrum
                    active: StackLayout.isCurrentItem
                    sourceComponent: TabSpectrum {
                        id: tabSpectrum
                    }
                }

                Loader {
                    id: ldrOscilloscope
                    active: StackLayout.isCurrentItem
                    sourceComponent: TabOscilloscope {
                        id: tabOscilloscope
                    }
                }

                onCurrentIndexChanged: backend.tab_changed(currentIndex)
            }
        }
    }


}

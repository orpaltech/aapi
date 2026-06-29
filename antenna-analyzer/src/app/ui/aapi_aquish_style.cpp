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

#include "aapi_aquish_style.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiAquishStyle
///////////////////////////////////////////////////////////////////////////////
/// \brief QAAPiAquishStyle::QAAPiAquishStyle
/// \param parent
///
QAAPiAquishStyle::QAAPiAquishStyle(QObject *parent)
    : QAAPiBaseStyle(parent)
{
    m_textLabelColor = "#8E92A2";
    m_warningColor = "#FF3344";
    m_accentColor = "#00FFCC";
    m_accentMutedColor = "#006866";

    m_highContrastRedColor = "#af0404";

    m_headerBackColor = "#2dd6d3";

    m_headerGradientColor1 = "#1f8f8b";
    m_headerGradientColor2 = "#1ee3dc";

    m_progressGradientColor1 = "#303243";
    m_progressGradientColor2 = "#00FFCC";

    m_dialogBackColor = "#212330";

    m_titleFontColor = "#061f1e";

    m_controlBackColor = "#f5f6f7";
    m_controlBorderColor = "#4a5054"; //"#061f1e";
    m_controlFontColor = "#061f1e";
    m_controlPressedFontColor = "#041212";

    m_buttonOnTextColor = "#FF5555";
    m_buttonOffTextColor = "#00FFCC";
    m_buttonOnBackColor = "#3d2126";
    m_buttonOffBackColor = "#1c2e2a";
    m_buttonOnBorderColor = "#FF3344";
    m_buttonOffBorderColor = "#00CCAA";
    m_buttonDisabledTextColor = "#606478";
    m_buttonDisabledBackColor = "#20222B";
    m_buttonDisabledBorderColor = "#424558";

    m_textAreaColor = "#e3fafa";

    m_highlightColor = "#259c9a";

    m_backdropPanelBackColor = "#252733";
    m_backdropPanelBorderColor = "#3B3E51";

    m_telemetryDeadColor = "#FF3344";   // Critical Dead Signal Red
    m_telemetryAlertColor = "#EF6C00";  // Vibrant Amber Orange for saturation/mismatch warnings


    m_chart->m_plotAreaColor    = "#cdf8f6"; //"#b4f4f2" "#defaf9";
    m_chart->m_axisLineColor    = "#061f1e";
    m_chart->m_gridLineColor    = "#164d4b";
    m_chart->m_minorGridLineColor = "#0e3332";
    m_chart->m_lineSeriesVColor = "#5bb500";
    m_chart->m_lineSeriesIColor = "#0088e3";
    m_chart->m_dropShadowEnabled = true;

    m_smith->m_tickColor = "#333333";
    m_smith->m_tickFont = "6pt sans-serif";
    m_smith->m_fillColor = "#9bf0ee";//"#82edea"; //"#b4f4f2";
    m_smith->m_circleColor = m_smith->m_fillColor.darker(3);    //color that has one-third the brightness
    m_smith->m_axisColor = "#333333";
    m_smith->m_gammaColor = "#dc0e0e";
    m_smith->m_swrColor = QColor(51, 51, 51, 128); // Semi-transparent version of axisColor (#333333)
    m_smith->m_swrFont = "5pt sans-serif";         // Slightly smaller than main ticks

    m_smith->m_helperColor = "#666666";
    m_smith->m_helperFont = "italic 8pt sans-serif";

    m_smith->m_legendBkgndColor = QColor(10, 25, 30, 180);  // #b40a191e
    m_smith->m_legendBorderColor = QColor(255, 255, 255, 38);

}

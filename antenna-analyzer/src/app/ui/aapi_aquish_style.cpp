#include "aapi_aquish_style.h"

QAAPiAquishStyle::QAAPiAquishStyle(QObject *parent)
    : QAAPiBaseStyle(parent)
{
    m_headerBackColor = "#2dd6d3";
    m_titleFontColor = "#061f1e";

    m_barGradientColor1 = "#1f8f8b";
    m_barGradientColor2 = "#1ee3dc";

    m_controlBorderColor = "#061f1e";
    m_controlFontColor = "#061f1e";
    m_controlPressedFontColor = "#041212";

    m_textAreaColor = "#e3fafa";

    m_highlightColor = "#259c9a";


    m_chart->m_plotAreaColor = "#defaf9";
    m_chart->m_axisLineColor = "#061f1e";
    m_chart->m_gridLineColor = "#0c3836";
    m_chart->m_minorGridLineColor = "#688c8b";
    m_chart->m_lineSeriesVColor = "#5bb500";
    m_chart->m_lineSeriesIColor = "#0088e3";

    m_smith->m_tickColor = "#333333";
    m_smith->m_tickFont = "6pt sans-serif";
    m_smith->m_fillColor = "#82edea";
    //Setting factor to 3.0 returns a color that has one-third the brightness
    m_smith->m_circleColor = m_smith->m_fillColor.darker(3);
    m_smith->m_axisColor = "#333333";
    m_smith->m_gammaColor = "#dc0e0e";
}

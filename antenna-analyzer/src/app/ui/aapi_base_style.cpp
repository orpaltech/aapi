#include "aapi_base_style.h"

QAAPiBaseStyle::QAAPiBaseStyle(QObject *parent)
    : QObject(parent)
    , m_chart(new QAAPiStyleChart(this))
    , m_smith(new QAAPiStyleSmith(this))
{
    m_scrollBarWidth = 24;

    m_chart->m_labelsXFontSize = 10;
    m_chart->m_labelsYFontSize = 11;

    m_statusSuccessColor = "#3ecf9a";
    m_statusBusyColor = "#f74f6e";
    m_statusDisableColor = "#bdbdbd";
}

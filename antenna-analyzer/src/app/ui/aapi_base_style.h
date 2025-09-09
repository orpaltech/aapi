#ifndef UI_AAPI_BASE_STYLE_H
#define UI_AAPI_BASE_STYLE_H

#include <QObject>
#include <QQmlEngine>
#include "aapi_style_chart.h"
#include "aapi_style_smith.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiBaseStyle
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The QAAPiBaseStyle class
///
class QAAPiBaseStyle : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(AapiStyle)

    /* Properties */
    Q_PROPERTY(QColor headerBackColor READ getHeaderBackColor CONSTANT)

    Q_PROPERTY(QColor barGradientColor1 READ getBarGradientColor1 CONSTANT)
    Q_PROPERTY(QColor barGradientColor2 READ getBarGradientColor2 CONSTANT)

    Q_PROPERTY(QColor titleFontColor READ getTitleFontColor CONSTANT)

    Q_PROPERTY(QColor controlFontColor READ getControlFontColor CONSTANT)
    Q_PROPERTY(QColor controlPressedFontColor READ getControlPressedFontColor CONSTANT)
    Q_PROPERTY(QColor controlBorderColor READ getControlBorderColor CONSTANT)

    Q_PROPERTY(QColor textAreaColor READ getTextAreaColor CONSTANT)

    Q_PROPERTY(QColor highlightColor READ getHighlightColor CONSTANT)

    Q_PROPERTY(quint32 scrollBarWidth READ getScrollBarWidth CONSTANT)

    Q_PROPERTY(QAAPiStyleChart* chart READ getChart CONSTANT)
    Q_PROPERTY(QAAPiStyleSmith* smith READ getSmith CONSTANT)

    /* status indicator colors */
    Q_PROPERTY(QColor statusSuccessColor READ getStatusSuccessColor CONSTANT)
    Q_PROPERTY(QColor statusBusyColor READ getStatusBusyColor CONSTANT)
    Q_PROPERTY(QColor statusDisableColor READ getStatusDisableColor CONSTANT)

public:
    explicit QAAPiBaseStyle(QObject *parent = nullptr);

    /* non-gradient solid header color */
    QColor getHeaderBackColor() const { return m_headerBackColor;}

    /* bar gradient colors */
    QColor getBarGradientColor1() const { return m_barGradientColor1;}
    QColor getBarGradientColor2() const { return m_barGradientColor2;}

    QColor getTitleFontColor() const { return m_titleFontColor;}

    QColor getControlFontColor() const { return m_controlFontColor;}
    QColor getControlPressedFontColor() const { return m_controlPressedFontColor;}
    QColor getControlBorderColor() const { return m_controlBorderColor;}

    QColor getTextAreaColor() const { return m_textAreaColor;}

    QColor getHighlightColor() const { return m_highlightColor;}

    quint32 getScrollBarWidth() const { return m_scrollBarWidth;}

    QAAPiStyleChart *getChart() const { return m_chart;}
    QAAPiStyleSmith *getSmith() const { return m_smith;}

    QColor getStatusSuccessColor() const { return m_statusSuccessColor;}
    QColor getStatusBusyColor() const { return m_statusBusyColor;}
    QColor getStatusDisableColor() const { return m_statusDisableColor;}

protected:
    QColor  m_headerBackColor;
    QColor  m_titleFontColor;

    QColor  m_barGradientColor1;
    QColor  m_barGradientColor2;

    QColor  m_controlFontColor;
    QColor  m_controlPressedFontColor;
    QColor  m_controlBorderColor;

    QColor  m_textAreaColor;

    QColor  m_highlightColor;

    quint32 m_scrollBarWidth;

    QAAPiStyleSmith *m_smith;
    QAAPiStyleChart *m_chart;

    QColor  m_statusSuccessColor;
    QColor  m_statusBusyColor;
    QColor  m_statusDisableColor;
};

#endif // UI_AAPI_BASE_STYLE_H

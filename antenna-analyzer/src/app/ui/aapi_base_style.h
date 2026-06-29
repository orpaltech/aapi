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
    Q_PROPERTY(QColor textLabelColor READ getTextLabelColor CONSTANT)
    Q_PROPERTY(QColor warningColor READ getWarningColor CONSTANT)
    Q_PROPERTY(QColor accentColor READ getAccentColor CONSTANT)
    Q_PROPERTY(QColor accentMutedColor READ getAccentMutedColor CONSTANT)

    Q_PROPERTY(QColor highContrastRedColor READ getHighContrastRedColor CONSTANT)

    /* non-gradient solid header color */
    Q_PROPERTY(QColor headerBackColor READ getHeaderBackColor CONSTANT)
    /* header gradient colors */
    Q_PROPERTY(QColor headerGradientColor1 READ getHeaderGradientColor1 CONSTANT)
    Q_PROPERTY(QColor headerGradientColor2 READ getHeaderGradientColor2 CONSTANT)
    /* progress bar gradient colors */
    Q_PROPERTY(QColor progressGradientColor1 READ getProgressGradientColor1 CONSTANT)
    Q_PROPERTY(QColor progressGradientColor2 READ getProgressGradientColor2 CONSTANT)

    Q_PROPERTY(QColor dialogBackColor READ getDialogBackColor CONSTANT)

    Q_PROPERTY(QColor titleFontColor READ getTitleFontColor CONSTANT)

    Q_PROPERTY(QColor controlFontColor READ getControlFontColor CONSTANT)
    Q_PROPERTY(QColor controlPressedFontColor READ getControlPressedFontColor CONSTANT)
    Q_PROPERTY(QColor controlBackColor READ getControlBackColor CONSTANT)
    Q_PROPERTY(QColor controlBorderColor READ getControlBorderColor CONSTANT)

    Q_PROPERTY(QColor buttonOnTextColor READ getButtonOnTextColor CONSTANT)
    Q_PROPERTY(QColor buttonOffTextColor READ getButtonOffTextColor CONSTANT)
    Q_PROPERTY(QColor buttonOnBackColor READ getButtonOnBackColor CONSTANT)
    Q_PROPERTY(QColor buttonOffBackColor READ getButtonOffBackColor CONSTANT)
    Q_PROPERTY(QColor buttonOnBorderColor READ getButtonOnBorderColor CONSTANT)
    Q_PROPERTY(QColor buttonOffBorderColor READ getButtonOffBorderColor CONSTANT)
    Q_PROPERTY(QColor buttonDisabledTextColor READ getButtonDisabledTextColor CONSTANT)
    Q_PROPERTY(QColor buttonDisabledBackColor READ getButtonDisabledBackColor CONSTANT)
    Q_PROPERTY(QColor buttonDisabledBorderColor READ getButtonDisabledBorderColor CONSTANT)

    Q_PROPERTY(QColor textAreaColor READ getTextAreaColor CONSTANT)

    Q_PROPERTY(QColor highlightColor READ getHighlightColor CONSTANT)

    Q_PROPERTY(QColor backdropPanelBackColor READ getBackdropPanelBackColor CONSTANT)
    Q_PROPERTY(QColor backdropPanelBorderColor READ getBackdropPanelBorderColor CONSTANT)

    Q_PROPERTY(quint32 scrollBarWidth READ getScrollBarWidth CONSTANT)

    Q_PROPERTY(QAAPiStyleChart* chart READ getChart CONSTANT)
    Q_PROPERTY(QAAPiStyleSmith* smith READ getSmith CONSTANT)

    /* status indicator colors */
    Q_PROPERTY(QColor statusSuccessColor READ getStatusSuccessColor CONSTANT)
    Q_PROPERTY(QColor statusBusyColor READ getStatusBusyColor CONSTANT)
    Q_PROPERTY(QColor statusDisableColor READ getStatusDisableColor CONSTANT)

    Q_PROPERTY(QColor telemetryDeadColor READ getTelemetryDeadColor CONSTANT)
    Q_PROPERTY(QColor telemetryAlertColor READ getTelemetryAlertColor CONSTANT)

public:
    explicit QAAPiBaseStyle(QObject *parent = nullptr);

    QColor getTextLabelColor() const { return m_textLabelColor;}

    QColor getWarningColor() const { return m_warningColor;}

    QColor getAccentColor() const { return m_accentColor;}
    QColor getAccentMutedColor() const { return m_accentMutedColor;}

    QColor getHighContrastRedColor() const { return m_highContrastRedColor; }

    QColor getHeaderBackColor() const { return m_headerBackColor;}

    QColor getHeaderGradientColor1() const { return m_headerGradientColor1;}
    QColor getHeaderGradientColor2() const { return m_headerGradientColor2;}

    QColor getProgressGradientColor1() const { return m_progressGradientColor1;}
    QColor getProgressGradientColor2() const { return m_progressGradientColor2;}

    QColor getDialogBackColor() const { return m_dialogBackColor;}

    QColor getTitleFontColor() const { return m_titleFontColor;}

    QColor getControlFontColor() const { return m_controlFontColor;}
    QColor getControlPressedFontColor() const { return m_controlPressedFontColor;}
    QColor getControlBackColor() const { return m_controlBackColor;}
    QColor getControlBorderColor() const { return m_controlBorderColor;}

    QColor getButtonOnTextColor() const { return m_buttonOnTextColor;}
    QColor getButtonOffTextColor() const { return m_buttonOffTextColor;}
    QColor getButtonOnBackColor() const { return m_buttonOnBackColor;}
    QColor getButtonOffBackColor() const { return m_buttonOffBackColor;}
    QColor getButtonOnBorderColor() const { return m_buttonOnBorderColor;}
    QColor getButtonOffBorderColor() const { return m_buttonOffBorderColor;}
    QColor getButtonDisabledTextColor() const { return m_buttonDisabledTextColor;}
    QColor getButtonDisabledBackColor() const { return m_buttonDisabledBackColor;}
    QColor getButtonDisabledBorderColor() const { return m_buttonDisabledBorderColor;}

    QColor getTextAreaColor() const { return m_textAreaColor;}

    QColor getHighlightColor() const { return m_highlightColor;}

    QColor getBackdropPanelBackColor() const { return m_backdropPanelBackColor; }
    QColor getBackdropPanelBorderColor() const { return m_backdropPanelBorderColor;}

    quint32 getScrollBarWidth() const { return m_scrollBarWidth;}

    QAAPiStyleChart *getChart() const { return m_chart;}
    QAAPiStyleSmith *getSmith() const { return m_smith;}

    QColor getStatusSuccessColor() const { return m_statusSuccessColor;}
    QColor getStatusBusyColor() const { return m_statusBusyColor;}
    QColor getStatusDisableColor() const { return m_statusDisableColor;}

    QColor getTelemetryDeadColor() const { return m_telemetryDeadColor;}
    QColor getTelemetryAlertColor() const { return m_telemetryAlertColor;}

protected:
    QColor  m_textLabelColor;

    QColor  m_warningColor;

    QColor  m_accentColor;
    QColor  m_accentMutedColor;

    QColor  m_highContrastRedColor;

    QColor  m_headerBackColor;

    QColor  m_headerGradientColor1;
    QColor  m_headerGradientColor2;

    QColor  m_progressGradientColor1;
    QColor  m_progressGradientColor2;

    QColor  m_dialogBackColor;

    QColor  m_titleFontColor;

    QColor  m_controlFontColor;
    QColor  m_controlPressedFontColor;
    QColor  m_controlBackColor;
    QColor  m_controlBorderColor;

    QColor  m_buttonOnTextColor;
    QColor  m_buttonOffTextColor;
    QColor  m_buttonOnBackColor;
    QColor  m_buttonOffBackColor;
    QColor  m_buttonOnBorderColor;
    QColor  m_buttonOffBorderColor;
    QColor  m_buttonDisabledTextColor;
    QColor  m_buttonDisabledBackColor;
    QColor  m_buttonDisabledBorderColor;

    QColor  m_textAreaColor;

    QColor  m_highlightColor;

    QColor  m_backdropPanelBackColor;
    QColor  m_backdropPanelBorderColor;

    quint32 m_scrollBarWidth;

    QAAPiStyleSmith *m_smith;
    QAAPiStyleChart *m_chart;

    QColor  m_statusSuccessColor;
    QColor  m_statusBusyColor;
    QColor  m_statusDisableColor;

    QColor  m_telemetryDeadColor;
    QColor  m_telemetryAlertColor;
};

#endif // UI_AAPI_BASE_STYLE_H

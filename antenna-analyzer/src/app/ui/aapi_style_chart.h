#ifndef UI_AAPI_STYLE_CHART_H
#define UI_AAPI_STYLE_CHART_H

#include <QObject>
#include <QQmlEngine>
#include <QColor>


///////////////////////////////////////////////////////////////////////////////
// class QAAPiStyleChart
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The QAAPiStyleChart class
///
class QAAPiStyleChart : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(StyleChart)

    /* Properties */
    Q_PROPERTY(QColor plotAreaColor READ getPlotAreaColor CONSTANT)
    Q_PROPERTY(QColor axisLineColor READ getAxisLineColor CONSTANT)
    Q_PROPERTY(QColor gridLineColor READ getGridLineColor CONSTANT)
    Q_PROPERTY(QColor minorGridLineColor READ getMinorGridLineColor CONSTANT)
    Q_PROPERTY(bool dropShadowEnabled READ getDropShadowEnabled CONSTANT)
    Q_PROPERTY(qint32 labelsXFontSize READ getLabelsXFontSize CONSTANT)
    Q_PROPERTY(qint32 labelsYFontSize READ getLabelsYFontSize CONSTANT)
    Q_PROPERTY(QColor lineSeriesVColor READ getLineSeriesVColor CONSTANT)
    Q_PROPERTY(QColor lineSeriesIColor READ getLineSeriesIColor CONSTANT)

public:
    explicit QAAPiStyleChart(QObject *parent = nullptr);

    QColor getPlotAreaColor() const { return m_plotAreaColor; }
    QColor getAxisLineColor() const { return m_axisLineColor; }
    QColor getGridLineColor() const { return m_gridLineColor; }
    QColor getMinorGridLineColor() const { return m_minorGridLineColor; }
    bool getDropShadowEnabled() const { return m_dropShadowEnabled; }
    qint32 getLabelsXFontSize() const { return m_labelsXFontSize; }
    qint32 getLabelsYFontSize() const { return m_labelsYFontSize; }
    QColor getLineSeriesVColor() const { return m_lineSeriesVColor; }
    QColor getLineSeriesIColor() const { return m_lineSeriesIColor; }

public:
    QColor  m_plotAreaColor;
    QColor  m_axisLineColor;
    QColor  m_gridLineColor;
    QColor  m_minorGridLineColor;
    bool    m_dropShadowEnabled;
    qint32  m_labelsXFontSize;
    qint32  m_labelsYFontSize;
    QColor  m_lineSeriesVColor;
    QColor  m_lineSeriesIColor;
};

#endif // UI_AAPI_STYLE_CHART_H

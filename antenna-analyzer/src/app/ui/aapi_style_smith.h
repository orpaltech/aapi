#ifndef UI_AAPI_STYLE_SMITH_H
#define UI_AAPI_STYLE_SMITH_H

#include <QObject>
#include <QQmlEngine>
#include <QColor>


///////////////////////////////////////////////////////////////////////////////
// class QAAPiStyleSmith
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The QAAPiStyleSmith class
///
class QAAPiStyleSmith : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(StyleSmith)

    /* Properties */
    Q_PROPERTY(QColor tickColor READ getTickColor CONSTANT)
    Q_PROPERTY(QString tickFont READ getTickFont CONSTANT)

    Q_PROPERTY(QColor fillColor READ getFillColor CONSTANT)
    Q_PROPERTY(QColor circleColor READ getCircleColor CONSTANT)
    Q_PROPERTY(QColor axisColor READ getAxisColor CONSTANT)
    Q_PROPERTY(QColor gammaColor READ getGammaColor CONSTANT)

    Q_PROPERTY(QColor swrColor READ getSwrColor CONSTANT)
    Q_PROPERTY(QString swrFont READ getSwrFont CONSTANT)

    Q_PROPERTY(QColor helperColor READ getHelperColor CONSTANT)

public:
    explicit QAAPiStyleSmith(QObject *parent = nullptr);

    QColor getTickColor() const { return m_tickColor; }
    QString getTickFont() const { return m_tickFont; }

    QColor getFillColor() const { return m_fillColor; }
    QColor getCircleColor() const { return m_circleColor; }
    QColor getAxisColor() const { return m_axisColor; }
    QColor getGammaColor() const { return m_gammaColor; }

    QColor getSwrColor() const { return m_swrColor; }
    QString getSwrFont() const { return m_swrFont; }

    QColor getHelperColor() const { return m_helperColor; }
    QString getHelperFont() const { return m_helperFont; }

public:
    QColor m_tickColor;
    QString m_tickFont;

    QColor m_fillColor;
    QColor m_circleColor;
    QColor m_axisColor;
    QColor m_gammaColor;

    QColor m_swrColor;
    QString m_swrFont;

    QColor m_helperColor;
    QString m_helperFont;
};

#endif // UI_AAPI_STYLE_SMITH_H

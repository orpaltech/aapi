#ifndef UI_AAPI_ERRORS_H
#define UI_AAPI_ERRORS_H

#include <QObject>
#include <QQmlEngine>
#include <QSettings>

///////////////////////////////////////////////////////////////////////////////
// class QAAPiMessages
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The QAAPiMessages class
///
class QAAPiMessages : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(AapiMessages)
public:
    explicit QAAPiMessages(QObject *parent = nullptr);

    Q_INVOKABLE QString error(int errorCode) const;

private:
    QSettings m_errors;
};

#endif // UI_AAPI_ERRORS_H

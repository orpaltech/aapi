#include "aapi_messages.h"

QAAPiMessages::QAAPiMessages(QObject *parent)
    : QObject(parent)
    , m_errors(":/errors/errors-en_US", QSettings::IniFormat)
{
}

QString QAAPiMessages::error(int errorCode) const
{
    return m_errors.value("errors/" + QVariant(std::abs(errorCode)).toString(), "").toString();
}

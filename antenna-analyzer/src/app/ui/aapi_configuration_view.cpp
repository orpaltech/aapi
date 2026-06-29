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

#include "aapi_configuration_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiConfigurationView
///////////////////////////////////////////////////////////////////////////////

QAAPiConfigurationView::QAAPiConfigurationView(AAPiConfig *config, QObject *parent)
    : QAAPiViewBackend(config, nullptr, nullptr, parent)
    , m_tmpConfig( AAPiConfig::create( false ))
    , m_index(-1)
{
}

QAAPiConfigurationView::~QAAPiConfigurationView()
{
}

AAPiError QAAPiConfigurationView::loadView()
{
    // Copy configuration into temp 
    *m_tmpConfig = *m_config;
    m_index = 0;

    emit raiseNumParamsChanged();

    return AAPI_SUCCESS;
}

void QAAPiConfigurationView::destroyView()
{
    if (is_dirty_config( )) {
        handleAcceptChanges( );
    }
}

QAAPiConfigurationView::ConfigParamType QAAPiConfigurationView::get_type() const
{
    return static_cast<ConfigParamType> (AAPiConfig::get_type( m_index ));
}

uint32_t QAAPiConfigurationView::get_num_params() const
{
    return m_tmpConfig->get_num_valid_params();
}

uint32_t QAAPiConfigurationView::get_num_options() const
{
    return m_tmpConfig->get_num_opts( m_index );
}

QString QAAPiConfigurationView::get_description() const
{
    return (const char *)m_tmpConfig->get_description( m_index );
}

QStringList QAAPiConfigurationView::get_opt_values() const
{
    const AAPiArray<AAPiVariant>&   values = m_tmpConfig->get_opt_values( m_index );
    QStringList                     res;

    for (uint i = 0; i < values.size(); i++) {
        res.append( formatValue( values[i] ) );
    }
    return res;
}

QStringList QAAPiConfigurationView::get_opt_labels() const
{
    const AAPiArray<AAPiVariant>&   values = m_tmpConfig->get_opt_values( m_index );
    const AAPiArray<AAPiString>&    labels = m_tmpConfig->get_opt_labels( m_index );
    QStringList                     res;

    for (uint i = 0; i < values.size(); i++) {
        res.append( labels[i].length() != 0 ? (const char *)labels[i] : formatValue( values[i] ) );
    }
    return res;
}

QString QAAPiConfigurationView::formatValue(const AAPiVariant& value) const
{
    QString formatted;
    switch (m_tmpConfig->get_type( m_index )) {
    case AAPiVariantType::BYTE:
    case AAPiVariantType::UINT:
    case AAPiVariantType::INT:
    case AAPiVariantType::TEXT:
        formatted = value.toString();
        break;

    case AAPiVariantType::FLOAT:
        formatted.setNum( value.toFloat(), 'f', m_tmpConfig->get_precision( m_index ) );
        break;

    case AAPiVariantType::UNKNOWN:
        break;
    }
    return formatted;
}

bool QAAPiConfigurationView::is_dirty_config() const
{
    for (uint i = 0; i < m_config->get_total_params(); i++) {
        if (m_config->get_value( i ) != m_tmpConfig->get_value( i )) {
            return true;
        }
    }
    return false;
}

QString QAAPiConfigurationView::get_value() const
{
    return formatValue( m_tmpConfig->get_value( m_index ));
}

void QAAPiConfigurationView::handleMovePrevParam()
{
    int i = this->m_index;

    while (--i >= 0 && !m_tmpConfig->is_valid( i ));

    m_index = std::max( i, 0 );
}

void QAAPiConfigurationView::handleMoveNextParam()
{
    int total = (int) m_tmpConfig->get_total_params();
    int i = this->m_index;

    while (++i < total && !m_tmpConfig->is_valid( i ));

    m_index = std::min( i, total-1 );
}

void QAAPiConfigurationView::handleMoveFirstParam()
{
    int total = (int) m_tmpConfig->get_total_params();
    int i = -1;

    while (++i < total && !m_tmpConfig->is_valid( i ));

    m_index = std::min( i, total-1 );
}

void QAAPiConfigurationView::handleSetParamValue(QString new_val)
{
    /* parameter has been changed */
    m_tmpConfig->set_value( m_index, new_val.toLatin1().constData() );
}

void QAAPiConfigurationView::handleSetParamOption(int opt)
{
    AAPiParameter param_id = AAPiConfig::get_id( m_index );
    AAPiVariant old_val = m_tmpConfig->get_value( param_id );
    AAPiVariant new_val = m_tmpConfig->get_opt_values( m_index )[ opt ];

    if ( old_val != new_val ) {
        // parameter has been changed
        m_tmpConfig->set_value( param_id, new_val );

        if (param_id == AAPiParameter::SHOW_ADVANCED) {
            m_index = 0;
            emit raiseNumParamsChanged();
        }
    }
}

void QAAPiConfigurationView::handleUseDefaults()
{
    // TODO: set all default values
}

void QAAPiConfigurationView::handleAcceptChanges()
{
    *m_config = *m_tmpConfig;
    m_config->flush();
}

void QAAPiConfigurationView::handleDiscardChanges()
{
    *m_tmpConfig = *m_config;
}

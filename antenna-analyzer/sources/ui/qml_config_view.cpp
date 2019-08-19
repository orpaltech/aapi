/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

#include "qml_config_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlConfigView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlConfigView::QAAPIQmlConfigView( AAPIConfig *config, QObject *parent )
    : QAAPIQmlView( config, nullptr, nullptr, parent )
    , m_tmpConfig( AAPIConfig::create() )
    , m_index( 0 )
    , m_numParams( 0 )
{
}

QAAPIQmlConfigView::~QAAPIQmlConfigView()
{
    AAPI_DISPOSE(m_tmpConfig);
}

int QAAPIQmlConfigView::load_view()
{
    // Copy configuration into temp 
    *m_tmpConfig = *m_config;
    m_index = 0;
    m_numParams = m_tmpConfig->get_num_params();
    emit numParamsChanged();
    return 0;
}

void QAAPIQmlConfigView::destroy_view()
{
    if( is_dirty_config() )
    {
        accept_changes();
    }

    m_numParams = 0;
}

QAAPIQmlConfigView::ConfigParamType QAAPIQmlConfigView::get_type() const
{
    return static_cast< ConfigParamType > ( AAPIConfig::get_type( m_index ) );
}

uint32_t QAAPIQmlConfigView::get_num_params() const
{
    return m_numParams;
}

uint32_t QAAPIQmlConfigView::get_num_options() const
{
    return m_tmpConfig->get_num_opt( m_index );
}

QString QAAPIQmlConfigView::get_description() const
{
    return m_tmpConfig->get_description( m_index );
}

QStringList QAAPIQmlConfigView::get_opt_values() const
{
    QStringList res;
    const QVariant* values = m_tmpConfig->get_opt_values( m_index );

    for( uint i = 0; i < m_tmpConfig->get_num_opt( m_index ); i++ )
    {
        res.append( format_value( values[i] ) );
    }
    return res;
}

QStringList QAAPIQmlConfigView::get_opt_labels() const
{
    QStringList res;
    const QString* labels = m_tmpConfig->get_opt_labels( m_index );
    const QVariant* values = m_tmpConfig->get_opt_values( m_index );
    unsigned i;

    for( i = 0; i < m_tmpConfig->get_num_opt( m_index ); i++ )
    {
        res.append( labels ? labels[ i ] : format_value( values[ i ] ) );
    }
    return res;
}

QString QAAPIQmlConfigView::format_value(QVariant value) const
{
    QString formatted;
    switch( m_tmpConfig->get_type( m_index ) )
    {
    case AA_PT_BYTE:
    case AA_PT_UINT:
    case AA_PT_INT:
    case AA_PT_TEXT:
        formatted = value.toString();
        break;

    case AA_PT_FLOAT:
        formatted.setNum( value.toFloat(), 'f', m_tmpConfig->get_precision( m_index ) );
        break;
    }
    return formatted;
}

bool QAAPIQmlConfigView::is_dirty_config() const
{
    for( int i = 0; i < m_config->get_total_params(); i++ )
    {
        if( m_config->get_value( i ) != m_tmpConfig->get_value( i ) )
        {
            return true;
        }
    }
    return false;
}

QString QAAPIQmlConfigView::get_value() const
{
    return format_value( m_tmpConfig->get_value( m_index ));
}

void QAAPIQmlConfigView::move_prev_param()
{
    int i = this->m_index;
    while (--i >= 0 && !m_tmpConfig->is_valid( i ));
    this->m_index = std::max( i, 0 );
}

void QAAPIQmlConfigView::move_next_param()
{
    int total = (int) m_tmpConfig->get_total_params();
    int i = this->m_index;
    while (++i < total && !m_tmpConfig->is_valid( i ));
    this->m_index = std::min( i, total-1 );
}

void QAAPIQmlConfigView::move_first_param()
{
    int total = (int) m_tmpConfig->get_total_params();
    int i = -1;
    while (++i < total && !m_tmpConfig->is_valid( i ));
    this->m_index = std::min( i, total-1 );
}

void QAAPIQmlConfigView::set_param_value(QString new_val)
{
    /* parameter has been changed */
    m_tmpConfig->set_value( m_index, new_val );
}

void QAAPIQmlConfigView::set_param_option(int opt)
{
    enum AAPIParamId param_id = AAPIConfig::get_id( m_index );
    QVariant old_val = m_tmpConfig->get_value( param_id );
    QVariant new_val = m_tmpConfig->get_opt_values( m_index )[ opt ];

    if( old_val != new_val )
    {
        // parameter has been changed 
        m_tmpConfig->set_value(param_id, new_val);

        if( param_id == AA_PARAM_SHOW_ADVANCED )
        {
            m_index = 0;
            m_numParams = m_tmpConfig->get_num_params();
            emit numParamsChanged();
        }
    }
}

void QAAPIQmlConfigView::accept_changes()
{
    *m_config = *m_tmpConfig;
    m_config->flush();
}

void QAAPIQmlConfigView::discard_changes()
{
    *m_tmpConfig = *m_config;
}

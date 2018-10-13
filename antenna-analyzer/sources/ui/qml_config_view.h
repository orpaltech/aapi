/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-PI software.
 *
 * 	ORPAL-AA-PI is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-PI is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-PI. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QML_CONFIGVIEW_H
#define QML_CONFIGVIEW_H

#include "qml_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// QAAPIQmlConfigView
///////////////////////////////////////////////////////////////////////////////

class QAAPIQmlConfigView :
        public QAAPIQmlView
{
public:
    enum ConfigParamType {
        PT_BYTE = AA_PT_BYTE,
        PT_UINT = AA_PT_UINT,
        PT_INT = AA_PT_INT,
        PT_FLOAT = AA_PT_FLOAT,
        PT_TEXT = AA_PT_TEXT
    };

    Q_OBJECT

    /* Enumerations */
    Q_ENUM(ConfigParamType)

    /* Properties */
    Q_PROPERTY(uint num_params READ get_num_params NOTIFY numParamsChanged)
    Q_PROPERTY(uint param_index READ get_param_index)
    Q_PROPERTY(uint param_num_options READ get_num_options NOTIFY paramNumOptionsChanged)
    Q_PROPERTY(QStringList param_opt_values READ get_opt_values)
    Q_PROPERTY(QStringList param_opt_labels READ get_opt_labels)
    Q_PROPERTY(QString param_description READ get_description)
    Q_PROPERTY(ConfigParamType param_type READ get_type)
    Q_PROPERTY(QString param_value READ get_value)
    Q_PROPERTY(bool is_dirty_config READ is_dirty_config)

public:
    explicit QAAPIQmlConfigView(AAPIConfig *config,
                                QObject *parent = Q_NULLPTR);

    /* Property accessors */
    uint32_t get_num_params() const;
    int get_param_index() const { return m_index;}
    uint32_t get_num_options() const;
    QStringList get_opt_values() const;
    QStringList get_opt_labels() const;
    QString get_description() const;
    QString get_value() const;
    bool is_dirty_config() const;
    enum ConfigParamType get_type() const;

private:
    virtual int load_view();
    virtual void destroy_view();

    QString format_value(QVariant value) const;

private:
    aapi_ptr<AAPIConfig> m_tmpConfig;
    int         m_index;
    uint32_t    m_numParams;

signals:
    void numParamsChanged();
    void paramNumOptionsChanged();

public slots:
    void move_prev_param();
    void move_next_param();
    void move_first_param();
    void set_param_value(QString new_val);
    void set_param_option(int new_opt);
    void accept_changes();
    void discard_changes();
};

#endif // QML_CONFIGVIEW_H

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

#ifndef UI_AAPI_CONFIGURATION_VIEW_H
#define UI_AAPI_CONFIGURATION_VIEW_H

#include "ui/aapi_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiConfigurationView
///////////////////////////////////////////////////////////////////////////////

class QAAPiConfigurationView : public QAAPiViewBackend
{
public:
    enum ConfigParamType {
        PT_BYTE = AAPI_VT_BYTE,
        PT_UINT = AAPI_VT_UINT,
        PT_INT = AAPI_VT_INT,
        PT_FLOAT = AAPI_VT_FLOAT,
        PT_TEXT = AAPI_VT_TEXT
    };

    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(ConfigurationViewBackend)

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
    explicit QAAPiConfigurationView(AAPiConfig *config, QObject *parent = Q_NULLPTR);
    ~QAAPiConfigurationView();

    /* Property accessors */
    uint32_t get_num_params() const;
    int get_param_index() const { return m_index;}
    uint32_t get_num_options() const;
    QStringList get_opt_values() const;
    QStringList get_opt_labels() const;
    QString get_description() const;
    QString get_value() const;
    bool is_dirty_config() const;
    ConfigParamType get_type() const;

private:
    virtual int load_view();
    virtual void destroy_view();

    QString formatValue(AAPiVariant value) const;

private:
    AAPiConfig  *m_tmpConfig;
    int         m_index;

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

#endif // UI_AAPI_CONFIGURATION_VIEW_H

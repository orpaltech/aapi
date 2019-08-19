/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef QML_OSLCAL_VIEW_H
#define QML_OSLCAL_VIEW_H

#include "ui/qml_view_backend.h"
#include "analyzer/analyzer_calibrator.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlOSLCalView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPIQmlOSLCalView class
///
class QAAPIQmlOSLCalView : public QAAPIQmlView
{
public:
    enum ScanType {
        SCAN_SHORT  = CALIB_FILE_SHORT_SCANNED,
        SCAN_LOAD   = CALIB_FILE_LOAD_SCANNED,
        SCAN_OPEN   = CALIB_FILE_OPEN_SCANNED,
    };

    Q_OBJECT

    /* Enumerations */
    Q_ENUM(ScanType)

    /* Properties */
    Q_PROPERTY(int osl_file READ get_osl_file WRITE set_osl_file NOTIFY oslFileChanged)
    Q_PROPERTY(uint file_num_options READ get_file_num_options)
    Q_PROPERTY(QList<int> file_opt_values READ get_file_opt_values)
    Q_PROPERTY(QStringList file_opt_labels READ get_file_opt_labels)
    Q_PROPERTY(bool file_exists READ get_file_exists NOTIFY oslFileExistsChanged)
    Q_PROPERTY(uint r_short READ get_r_short WRITE set_r_short NOTIFY oslRShortChanged)
    Q_PROPERTY(uint r_load READ get_r_load WRITE set_r_load NOTIFY oslRLoadChanged)
    Q_PROPERTY(uint r_open READ get_r_open WRITE set_r_open NOTIFY oslROpenChanged)

public:
    explicit QAAPIQmlOSLCalView(AAPIConfig *config, AAPISignalProcessor *dsp,
                                AAPIGenerator *gen, AAPICalibrator *cal,
                                QObject *parent = Q_NULLPTR);
    ~QAAPIQmlOSLCalView();

private:
    virtual int load_view();
    virtual void destroy_view();

    /* Properties */
    int get_osl_file() const { return m_osl_file; }
    void set_osl_file(int osl_file);

    uint32_t get_file_num_options() const;
    QList<int> get_file_opt_values() const;
    QStringList get_file_opt_labels() const;

    bool get_file_exists() const;

    uint32_t get_r_short() const { return m_config->get_osl_r_short(); }
    uint32_t get_r_load() const { return m_config->get_osl_r_load(); }
    uint32_t get_r_open() const { return m_config->get_osl_r_open(); }

    void set_r_short(uint32_t val) {}
    void set_r_load(uint32_t val) {}
    void set_r_open(uint32_t val) {}

// Measurement callback
    virtual int on_measure_finished(AAPIMeasure *measure);

private:
    AAPICalibrator  *m_calibrator;
    ScanType    m_scan_type;
    int         m_osl_file;
    uint32_t    m_scan_index;
    bool        m_scan_cancelled;

signals:
    void oslFileChanged();
    void oslRShortChanged();
    void oslRLoadChanged();
    void oslROpenChanged();
    void oslFileExistsChanged();
    void scanProgress(enum ScanType type, int step, int total, float r, float x);
    void scanNoSignal(enum ScanType type);

public slots:
    int start_scan(enum ScanType type);
    void cancel_scan();
};

#endif // QML_OSLCAL_VIEW_H

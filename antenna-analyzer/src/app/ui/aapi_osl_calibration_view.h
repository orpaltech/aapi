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

#ifndef UI_AAPI_OSL_CAL_VIEW_H
#define UI_AAPI_OSL_CAL_VIEW_H

#include "aapi_view_backend.h"
#include "analyzer/aapi_calibrator.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiOSLCalibrationView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiOSLCalibrationView class
///
class QAAPiOSLCalibrationView : public QAAPiViewBackend
{
public:
    enum ScanType {
        SCAN_SHORT  = AAPI_CAL_FILE_SHORT_SCANNED,
        SCAN_LOAD   = AAPI_CAL_FILE_LOAD_SCANNED,
        SCAN_OPEN   = AAPI_CAL_FILE_OPEN_SCANNED,
    };

    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(OSLCalibrationViewBackend)

    /* Enumerations */
    Q_ENUM(ScanType)

    /* Properties */
    Q_PROPERTY(int osl_file READ getOslFile WRITE setOslFile NOTIFY oslFileChanged)
    Q_PROPERTY(uint num_osl_files READ getNumOslFiles CONSTANT)
    Q_PROPERTY(QIntList file_opt_values READ getFileOptionValues CONSTANT)
    Q_PROPERTY(QStringList file_opt_labels READ getFileOptionLabels CONSTANT)
    Q_PROPERTY(bool osl_file_exists READ getOslFileExists CONSTANT)
    Q_PROPERTY(uint r_short READ getRShort CONSTANT)
    Q_PROPERTY(uint r_load READ getRLoad CONSTANT)
    Q_PROPERTY(uint r_open READ getROpen CONSTANT)

public:
    explicit QAAPiOSLCalibrationView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                     AAPiGenerator *gen, AAPiCalibrator *cal,
                                     QObject *parent = Q_NULLPTR);
    ~QAAPiOSLCalibrationView();

private:
    virtual int load_view();
    virtual void destroy_view();

    /* ---------- Properties ----------- */
    int getOslFile() const;
    void setOslFile(int osl_file);

    uint32_t getNumOslFiles() const;
    QIntList getFileOptionValues() const;
    QStringList getFileOptionLabels() const;

    bool getOslFileExists() const;

    uint32_t getRShort() const { return m_config->get_osl_r_short(); }
    uint32_t getRLoad() const { return m_config->get_osl_r_load(); }
    uint32_t getROpen() const { return m_config->get_osl_r_open(); }

    void setRShort(uint32_t val) {}
    void setRLoad(uint32_t val) {}
    void setROpen(uint32_t val) {}

// Measurement callback
    virtual int onViewMeasureFinished(AAPiMeasureTask *measure);

private:
    AAPiCalibrator  *m_calibrator;
    ScanType        m_scanType;
    uint32_t        m_scanIndex;
    bool            m_scanCancel;

signals:
    void oslFileChanged(int oslFile);
    void oslFileExistsChanged();
    void numOslFilesChanged();
    void scanProgress(enum ScanType type, int step, int total, double r, double x);
    void scanNoSignal(enum ScanType type);

public slots:
    int start_scan(enum ScanType type);
    void cancel_scan();
};

#endif // UI_AAPI_OSL_CAL_VIEW_H

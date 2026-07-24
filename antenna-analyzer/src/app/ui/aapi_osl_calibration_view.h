/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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
#include "aapi_messages.h"

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
        SCAN_OPEN   = AAPiCalibrator::FILE_STATUS_SCAN_OPEN,
        SCAN_SHORT  = AAPiCalibrator::FILE_STATUS_SCAN_SHORT,
        SCAN_LOAD   = AAPiCalibrator::FILE_STATUS_SCAN_LOAD
    };

    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(OSLCalibrationViewBackend)

    /* Enumerations */
    Q_ENUM(ScanType)

    /* Properties */
    Q_PROPERTY(int osl_file READ getFile WRITE setFile NOTIFY oslFileChanged)
    Q_PROPERTY(uint num_osl_files READ getNumFiles CONSTANT)
    Q_PROPERTY(QIntList file_opt_values READ getFileOptionValues CONSTANT)
    Q_PROPERTY(QStringList file_opt_labels READ getFileOptionLabels CONSTANT)
    Q_PROPERTY(bool osl_file_exists READ getFileExists NOTIFY oslFileExistsChanged)
    Q_PROPERTY(uint r_short READ getRShort CONSTANT)
    Q_PROPERTY(uint r_load READ getRLoad CONSTANT)
    Q_PROPERTY(uint r_open READ getROpen CONSTANT)

public:
    explicit QAAPiOSLCalibrationView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                     AAPiGenerator *gen, AAPiCalibrator *cal,
                                     QAAPiMessages *msgs, QObject *parent = Q_NULLPTR);
    ~QAAPiOSLCalibrationView();

private:
// QAAPiViewBackend
    AAPiError onViewLoad() override;
    void onViewDestroy() override;
    AAPiError onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure) override;
    void onViewMeasureError(AAPiError error) override;

    /* ---------- Properties ----------- */
    int getFile() const;
    void setFile(int osl_file);

    uint32_t getNumFiles() const;
    QIntList getFileOptionValues() const;
    QStringList getFileOptionLabels() const;

    bool getFileExists() const;

    uint32_t getRShort() const { return m_config->get_osl_r_short(); }
    uint32_t getRLoad() const { return m_config->get_osl_r_load(); }
    uint32_t getROpen() const { return m_config->get_osl_r_open(); }

private:
    AAPiPtr<AAPiCalibrator> m_calibrator;
    ScanType        m_scanType;
    uint32_t        m_scanIndex;
    volatile bool   m_scanCancelled;

Q_SIGNALS:
    void oslFileChanged(int osl_file);
    void oslFileExistsChanged();
    void numOslFilesChanged();
    void scanProgress(enum ScanType scan_type, int step, int total, quint32 freq, double rx_re, double rx_im);
    void scanNoSignal(enum ScanType scan_type);
    void scanError(QString message);

public Q_SLOTS:
    bool handleStartScan(enum ScanType scan_type);
    void handleCancelScan();
};

#endif // UI_AAPI_OSL_CAL_VIEW_H

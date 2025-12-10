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

#ifndef AAPI_SIGNAL_PROCESS_H
#define AAPI_SIGNAL_PROCESS_H

#include <complex.h>
#include "audio/audio_reader.h"
#include "analyzer/aapi_config.h"
#include "utils/simple_array.h"
#include "utils/aapi_complex.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////

enum AAPiSignalChannel {
    DSP_V_CHANNEL,
    DSP_I_CHANNEL,
    NUM_DSP_CHANNELS
};

enum AAPiSignalProcessError {
    AAPI_DSP_E_UNSPECIFIED              = (AAPI_DSP_ERROR_START - 0),
    AAPI_DSP_E_ADC_DEVICE_NOT_FOUND     = (AAPI_DSP_ERROR_START - 1),   /* The selected audio device was not found. */
    AAPI_DSP_E_ADC_UNSUPPORT_FORMAT     = (AAPI_DSP_ERROR_START - 2),   /* The specified audio format is not suported by the device.*/
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiSignalProcessEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiSignalProcessEvents class
///
class AAPiSignalProcessEvents
{
    volatile bool m_enabled;
public:
    AAPiSignalProcessEvents() : m_enabled(false) {}
    virtual ~AAPiSignalProcessEvents() {}

    void enableSignalProcessing(bool enable = true) { m_enabled = enable; }

    bool isProcessingSignal() const { return m_enabled; }

    virtual void onSignalProcessRaw(double **buffers, uint32_t num_buffers, uint32_t buf_size) { }
    virtual void onSignalProcessFFT(double **buffers, uint32_t num_buffers, uint32_t buf_size) { }
    virtual void onSignalProcessMags(AAPiComplex *mags, uint32_t num_mags) { }
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiSignalProcessor
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiSignalProcessor class
///
class AAPiSignalProcessor : public AAPiObject,
                            public AAPiAudioReaderEvents
{
    DECLARE_AAPI_OBJECT_WITH_CONFIG(AAPiSignalProcessor)

protected:
    AAPiSignalProcessor();
    ~AAPiSignalProcessor();

public:
    int start();
    void stop();

    void addCallback(AAPiSignalProcessEvents *cb);

    static void setBlackman(double *wnd, uint32_t nsamples);
    static double mag2db(double magnitude);

private:
// AAPiAudioReaderEvents
    virtual void onAudioReaderData(char **buffers,
                                   uint32_t num_buffers,
                                   uint32_t buf_size);

    /* Caclulates magnitude for a bin considering +/- 2 bins from maximum */
    AAPiComplex calcMagnitude(int channel);

    /* Process audio buffer for a channel */
    void processAudioBuffer(int channel, char *buffer, uint32_t buf_size);

    void releaseBuffers();

private:
    AAPiAudioReader         *m_reader;
    AAPiArray<AAPiSignalProcessEvents*>  m_callbacks;
    double                  *m_fft_mags[NUM_DSP_CHANNELS];
    AAPiComplex             m_fft_xmag[NUM_DSP_CHANNELS];
    double                  *m_raw_inp[NUM_DSP_CHANNELS];
    double                  *m_fft_inp[NUM_DSP_CHANNELS];
    AAPiComplex             *m_fft_out;
    double                  *m_fft_wnd;
    void                    *m_plan;
};

} //namespace aapi

#endif /* AAPI_SIGNAL_PROCESS_H */

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
#include "aapi_object_with_config.h"
#include "audio/audio_reader.h"
#include "utils/simple_array.h"
#include "utils/aapi_dsp_defs.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////

enum AAPiSignalChannel {
    DSP_V_CHANNEL,
    DSP_I_CHANNEL,
    N_DSP_CHANNELS
};

enum AAPiSignalProcessError {
    AAPI_DSP_E_UNSPECIFIED              = (AAPI_DSP_ERROR_START - 0),
    AAPI_DSP_E_ADC_DEVICE_NOT_FOUND     = (AAPI_DSP_ERROR_START - 1),   /* The selected audio device was not found. */
    AAPI_DSP_E_ADC_UNSUPPORT_FORMAT     = (AAPI_DSP_ERROR_START - 2),   /* The specified audio format is not suported by the device.*/
    AAPI_DSP_E_ADC_BUFFER_OVERRUN       = (AAPI_DSP_ERROR_START - 3),   /* Audio buffer overrun: Consumer thread fell behind hardware reader. */
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiSignalProcessEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiSignalProcessEvents class
///
class AAPiSignalProcessEvents
{
    // This provides true, thread-safe memory sequencing across your audio loops.
    std::atomic<bool> m_processing;

public:
    AAPiSignalProcessEvents() : m_processing(false) {}
    virtual ~AAPiSignalProcessEvents() = default;

    // Changes the active flag state atomically using safe memory barriers
    void signal_process_enable(bool enable = true) {
        m_processing.store(enable, std::memory_order_release);
    }

    // Evaluates the current state safely across your real-time processing threads
    bool is_signal_processing() const {
        return m_processing.load(std::memory_order_acquire);
    }

    virtual void onSignalProcessError(AAPiError error) { }
    virtual void onSignalProcessRaw(AAPiReal **buffers, uint32_t num_buffers, uint32_t buf_size) { }
    virtual void onSignalProcessFFT(AAPiReal **buffers, uint32_t num_buffers, uint32_t buf_size) { }
    virtual void onSignalProcessMags(AAPiComplex *mags, uint32_t num_mags) { }
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiSignalProcessor
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiSignalProcessor class
///
class AAPiSignalProcessor : public AAPiObject, public AAPiAudioReaderEvents
{
    DECLARE_AAPI_OBJECT_WITH_CONFIG(AAPiSignalProcessor)

protected:
    AAPiSignalProcessor();
    ~AAPiSignalProcessor();

public:
    AAPiError start();
    void stop();

    uint32_t getBufferSize() const;

    void addCallback(AAPiSignalProcessEvents *cb);

    static AAPiReal setBlackman(AAPiReal *wnd, uint32_t nsamples);
    static AAPiReal mag2db(AAPiReal magnitude);

private:
// AAPiAudioReaderEvents
    void onAudioReaderData(char **buffers, uint32_t num_buffers, uint32_t buf_size) override;
    void onAudioBufferOverrun() override;

    // Caclulates magnitude for a bin considering +/- 2 bins from maximum
    AAPiComplex calcMagnitude(int channel);

    // Process audio buffer for a channel
    void processAudioBuffer(int channel, char *buffer, uint32_t buf_size);
    AAPiReal readSample(int index, char *buffer);

    void releaseBuffers();

private:
    AAPiArray<AAPiSignalProcessEvents*> m_callbacks;
    AAPiPtr<AAPiAudioReader>            m_reader;

    AAPiReal        *m_fft_mags[N_DSP_CHANNELS];
    AAPiComplex     m_fft_xmag[N_DSP_CHANNELS];
    AAPiReal        *m_raw_inp[N_DSP_CHANNELS];
    AAPiReal        *m_fft_inp[N_DSP_CHANNELS];
    AAPiComplex     *m_fft_out;
    AAPiReal        *m_fft_wnd;
    AAPiReal        m_fft_wnd_gain;
    void*           m_plan;
};

} //namespace aapi

#endif /* AAPI_SIGNAL_PROCESS_H */

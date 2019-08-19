/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef AAPI_DSP_H
#define AAPI_DSP_H

#include <complex.h>
#include "audio/audio_reader.h"
#include "analyzer/analyzer_config.h"
#include "analyzer/simple_array.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////

enum AAPISignalProcessorChannel {
    DSP_V_CHANNEL,
    DSP_I_CHANNEL,
    NUM_DSP_CHANNELS
};

enum AAPISignalProcessorError {
    AAPI_DSP_E_AUDIO_DEVICE_NOT_FOUND    = AAPI_DSP_ERROR_START,
    AAPI_DSP_E_AUDIO_UNSUPPORT_FORMAT    = AAPI_DSP_ERROR_START - 1,
};

///////////////////////////////////////////////////////////////////////////////
// class AAPISignalProcessorEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPISignalProcessorEvents class
///
class AAPISignalProcessorEvents
{
    volatile bool m_enabled;
public:
    AAPISignalProcessorEvents()
    {
        m_enabled = false;
    }
    virtual ~AAPISignalProcessorEvents() { }

    void enable_signal_processing(bool enable = true)
    {
        m_enabled = enable;
    }

    bool is_signal_processing() const
    {
        return m_enabled;
    }

    virtual void dsp_raw_frame(float **buffers, uint32_t num_buffers, uint32_t buf_size) { }
    virtual void dsp_fft_frame(float **buffers, uint32_t num_buffers, uint32_t buf_size) { }
    virtual void dsp_magnitudes(std::complex< float > *mags, uint32_t num_mags) { }
};

///////////////////////////////////////////////////////////////////////////////
// class AAPISignalProcessor
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPISignalProcessor class
///
class AAPISignalProcessor : public AAPIObject,
                            public AAPIAudioReaderEvents
{
    DECLARE_AAPI_OBJECT(AAPISignalProcessor)

    static AAPISignalProcessor *create(AAPIConfig *config, bool add_ref = true);
protected:
    AAPISignalProcessor();
    ~AAPISignalProcessor();

public:
    int start();
    void stop();

    void add_callback(AAPISignalProcessorEvents *cb);

    static void set_blackman(float *wnd, uint32_t nsamples);
    static float mag2db(float magnitude);

private:
// AAPIAudioReaderEvents
    virtual void audio_reader_data(char **buffers,
                                   uint32_t num_buffers,
                                   uint32_t buf_size);

    /* Caclulates magnitude for a bin considering
     * +/- 2 bins from maximum */
    std::complex<float> calc_magnitude(int channel);

    /* Process audio buffer for a channel */
    uint32_t process_buffer(int channel, char *buffer, uint32_t buf_size);

    void free_buffers();

private:
    AAPIConfig          *m_config;
    AAPIAudioReader     *m_reader;
    SimpleArray<AAPISignalProcessorEvents>  m_callbacks;
    std::complex<float> fft_xmag[NUM_DSP_CHANNELS];
    float               *fft_inp[NUM_DSP_CHANNELS];
    float               *fft_out;
    float               *fft_mags[NUM_DSP_CHANNELS];
    float               *m_wnd;
    void                *m_plan;
};

} //namespace aapi

#endif /* AAPI_DSP_H */

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

#ifndef AAPI_ALSA_READER_H
#define AAPI_ALSA_READER_H

#include "audio/audio_reader.h"

namespace aapi
{
class ALSASource;
class ALSADevice;

///////////////////////////////////////////////////////////////////////////////
// class AAPIALSAReader
///////////////////////////////////////////////////////////////////////////////

class AAPIALSAReader : public AAPIAudioReader
{
    DECLARE_AAPI_OBJECT(AAPIALSAReader)
protected:
    AAPIALSAReader();
    ~AAPIALSAReader();

public:
    virtual uint32_t get_num_devices();
    virtual const char *get_device_id(int index);
    virtual const char *get_device_name(int index);

    virtual bool is_format_supported(int index,
                                     AAPIAudioChannels channels,
                                     AAPIAudioSampleRate sample_rate,
                                     AAPIAudioSampleSize sample_size);

    virtual int open(const char *device_id,
                     AAPIAudioChannels channels,
                     AAPIAudioSampleRate sample_rate,
                     AAPIAudioSampleSize sample_size,
                     uint32_t nsamples);
    virtual void close();

    virtual int start(AAPIAudioReaderEvents *callback);
    virtual void stop();

private:
    static void load_devices(ALSADevice **devices, uint32_t *num_devices);

private:
    ALSASource  *m_source;
    ALSADevice  *m_devices;
    uint32_t    m_numDevices;
};

} //namespace aapi

#endif // AAPI_ALSA_READER_H

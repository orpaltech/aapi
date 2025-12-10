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

#ifndef AAPI_ALSA_READER_H
#define AAPI_ALSA_READER_H

#include "audio/audio_reader.h"

namespace aapi
{

class AlsaSource;
class AlsaDevice;

///////////////////////////////////////////////////////////////////////////////
// class AAPIALSAReader
///////////////////////////////////////////////////////////////////////////////

class AAPiAlsaReader : public AAPiAudioReader
{
    DECLARE_AAPI_OBJECT(AAPiAlsaReader)

protected:
    AAPiAlsaReader();
    ~AAPiAlsaReader();

public:
    virtual uint32_t get_num_devices();
    virtual const char *get_device_id(int dev_index);
    virtual const char *get_device_name(int dev_index);

    virtual bool is_format_supported(int dev_index, AAPiAudioChannels channels,
                                     AAPiAudioSampleRate sample_rate,
                                     AAPiAudioSampleSize sample_size);

    virtual int open(const char *dev_id, AAPiAudioChannels channels,
                     AAPiAudioSampleRate sample_rate,
                     AAPiAudioSampleSize sample_size,
                     uint32_t num_samples);
    virtual void close();

    virtual int start(AAPiAudioReaderEvents *callback);
    virtual void stop();

private:
    static void load_devices(AlsaDevice **devices, uint32_t *num_devices);

private:
    AlsaSource  *m_source;
    AlsaDevice  *m_devices;
    uint32_t    m_numDevices;
};

} //namespace aapi

#endif // AAPI_ALSA_READER_H

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

#ifndef AAPI_AUDIO_READER_H
#define AAPI_AUDIO_READER_H

#include "aapi_object.h"
#include "aapi_error.h"
#include "utils/simple_string.h"

namespace aapi
{

enum AAPiAudioError {
    AAPI_AUDIO_E_UNSPECIFIED        = (AAPI_AUDIO_ERROR_START - 0),
    AAPI_AUDIO_E_INVALID_PARAM      = (AAPI_AUDIO_ERROR_START - 1),
    AAPI_AUDIO_E_OPEN_DEVICE        = (AAPI_AUDIO_ERROR_START - 2),
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiAudioReaderEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiAudioReaderEvents class
///
class AAPiAudioReaderEvents
{
public:
    virtual ~AAPiAudioReaderEvents() = default;

    virtual void onAudioReaderData(char **buffers, uint32_t num_buffers, uint32_t buf_size) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiAudioReader
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiAudioReader class
///
class AAPiAudioReader : public AAPiObject
{
public:
    static AAPiAudioReader *create(bool addRef = true);

    enum class Channels : unsigned char {
        Mono    = 1U,
        Stereo  = 2U,
    };

    enum class SampleSize : unsigned char {
        _16 = 16U,
        _24 = 24U,
        _32 = 32U
    };

    enum class SampleRate : unsigned int {
        _48K    = 48000U,
        _96K    = 96000U,
    };

protected:
    AAPiAudioReader();
    ~AAPiAudioReader();

public:
    virtual unsigned int get_num_devices() = 0;
    virtual AAPiString get_device_id(int index) = 0;
    virtual AAPiString get_device_name(int index) = 0;

    virtual bool is_format_supported(int index, Channels channels,
                                     SampleRate sample_rate,
                                     SampleSize sample_size) = 0;

    virtual AAPiError open(const AAPiString& dev_id, Channels channels,
                           SampleRate sample_rate, SampleSize sample_size,
                           uint32_t period_samples) = 0;
    virtual void close() { }

    virtual uint32_t get_buffer_size_chan() const = 0;

    virtual AAPiError start(AAPiAudioReaderEvents *callback) = 0;
    virtual void stop() = 0;
};

using AAPiAudioChannels = AAPiAudioReader::Channels;
using AAPiAudioSampleRate = AAPiAudioReader::SampleRate;
using AAPiAudioSampleSize = AAPiAudioReader::SampleSize;

} //namespace aapi

#endif // AAPI_AUDIO_READER_H

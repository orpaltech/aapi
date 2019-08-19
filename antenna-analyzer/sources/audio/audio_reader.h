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

#ifndef AAPI_AUDIO_READER_H
#define AAPI_AUDIO_READER_H

#include "analyzer_object.h"
#include "error_codes.h"

namespace aapi
{
enum AAPIAudioChannels {
    AUDIO_CHANNELS_1    = 1,
    AUDIO_CHANNELS_2    = 2
};

enum AAPIAudioSampleSize {
    AUDIO_SSIZE_16  = 16,
    AUDIO_SSIZE_24  = 24,
    AUDIO_SSIZE_32  = 32,
};

enum AAPIAudioSampleRate {
    AUDIO_SRATE_11K     = 11025,
    AUDIO_SRATE_22K     = 22050,
    AUDIO_SRATE_44K     = 44100,
    AUDIO_SRATE_48K     = 48000,
    AUDIO_SRATE_96K     = 96000,
};

enum AAPIAudioError {
    AAPI_AUDIO_E_OPEN_DEVICE         = AUDIO_ERROR_START,
    AAPI_AUDIO_E_INVALID_PARAM       = AUDIO_ERROR_START - 1,
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIAudioReaderEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIAudioReaderEvents class
///
class AAPIAudioReaderEvents
{
public:
    virtual ~AAPIAudioReaderEvents() {}

    virtual void audio_reader_data(char **buffers, uint32_t num_buffers, uint32_t buf_size) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIAudioReader
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIAudioReader class
///
class AAPIAudioReader : public AAPIObject
{
public:
    static AAPIAudioReader *create(bool add_ref = true);

protected:
    AAPIAudioReader();
    ~AAPIAudioReader();

public:
    virtual unsigned int get_num_devices() = 0;
    virtual const char *get_device_id(int index) = 0;
    virtual const char *get_device_name(int index) = 0;

    virtual bool is_format_supported(int index,
                                     enum AAPIAudioChannels channels,
                                     enum AAPIAudioSampleRate sample_rate,
                                     enum AAPIAudioSampleSize sample_size) = 0;

    virtual int open(const char *device_id,
                     AAPIAudioChannels channels,
                     AAPIAudioSampleRate sample_rate,
                     AAPIAudioSampleSize sample_size,
                     uint32_t nsamples) = 0;
    virtual void close() { }

    virtual int start(AAPIAudioReaderEvents *callback) = 0;
    virtual void stop() = 0;
};

} //namespace aapi

#endif // AAPI_AUDIO_READER_H

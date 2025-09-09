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

#include "audio_reader.h"
#include "audio/impl/alsa_reader.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPIAudioReader implementation
///////////////////////////////////////////////////////////////////////////////

AAPiAudioReader *AAPiAudioReader::create(bool addRef)
{
    // create an ALSA reader impl 
    return AAPiAlsaReader::create(addRef);
}

AAPiAudioReader::AAPiAudioReader()
{
}

AAPiAudioReader::~AAPiAudioReader()
{
    close();
}

} //namespace aapi

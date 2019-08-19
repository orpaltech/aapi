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

#include "audio_reader.h"
#include "audio/impl/alsa_reader.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPIAudioReader implementation
///////////////////////////////////////////////////////////////////////////////

AAPIAudioReader *AAPIAudioReader::create(bool add_ref)
{
    // create an ALSA reader impl 
    return AAPIALSAReader::create(add_ref);
}

AAPIAudioReader::AAPIAudioReader()
{
}

AAPIAudioReader::~AAPIAudioReader()
{
    close();
}

} //namespace aapi

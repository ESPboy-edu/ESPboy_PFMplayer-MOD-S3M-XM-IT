//============================================================================
// PMF Player
//
// Copyright (c) 2019, Profoundic Technologies, Inc.
// All rights reserved.
//----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Profoundic Technologies nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL PROFOUNDIC TECHNOLOGIES BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//============================================================================

#include "pmf_player.h"
#if defined(ARDUINO_ARCH_ESP8266)
#include "pmf_data.h"

#include "sigma_delta.h"
#include "timer.h"
//---------------------------------------------------------------------------

//===========================================================================
// audio buffer
//===========================================================================
static pmf_audio_buffer<int16_t, 1024> s_audio_buffer;
//---------------------------------------------------------------------------


//===========================================================================
// pmf_player
//===========================================================================

uint8_t masterVolume = 255;

IRAM_ATTR void timerHandler( void ){
    sigmaDeltaWrite(0, (s_audio_buffer.read_sample<uint16_t, 8>() * masterVolume ) >> 8 );
}

uint32_t pmf_player::get_sampling_freq(uint32_t sampling_freq_) const
{
  return (1000*1000) / ( (1000*1000)/sampling_freq_ ) ;
}
//----

void setupTimer ( int rate, int_handler_t handler ) ;
void timerHandler( void ) ;

void pmf_player::start_playback(uint32_t sampling_freq_)
{
    sigmaDeltaSetup(0, sampling_freq_ );
    sigmaDeltaAttachPin(D3);
    sigmaDeltaEnable();

    s_audio_buffer.reset();

    setupTimer ( (1000*1000)/sampling_freq_, (int_handler_t)timerHandler );
}

void pmf_player::stop_playback()
{
  /*todo*/
}
//----

void pmf_player::mix_buffer(pmf_mixer_buffer &buf_, unsigned num_samples_)
{
  mix_buffer_impl<int16_t, false, 8>(buf_, num_samples_);
}
//----

pmf_mixer_buffer pmf_player::get_mixer_buffer()
{
  return s_audio_buffer.get_mixer_buffer();
}
//---------------------------------------------------------------------------

//===========================================================================
#endif // ARDUINO_ARCH_ESP8266
/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Audio/Sound.hpp"

#ifdef ANDROID
#include "Android/Main.hpp"
#include "Android/SoundUtil.hpp"
#include "Android/Context.hpp"
#endif

#if defined(WIN32) && !defined(GNAV)
#include "ResourceLoader.hpp"
#include <windows.h>
#include <mmsystem.h>
#endif

#if !defined(ANDROID) && !(defined(WIN32) && !defined(GNAV))
#include "Audio/raw_play.hpp"
#include "LocalPath.hpp"
#include "Util/StringUtil.hpp"
#include "Util/StaticString.hxx"
#include <windef.h>

const char *beep_id_str = "IDR_";
const char *wav_str = "WAV_";
#endif

bool
PlayResource(const TCHAR *resource_name)
{
#ifdef ANDROID

  return SoundUtil::Play(Java::GetEnv(), context->Get(), resource_name);

#elif defined(WIN32) && !defined(GNAV)

  if (_tcsstr(resource_name, TEXT(".wav")))
    return sndPlaySound(resource_name, SND_ASYNC | SND_NODEFAULT);

  ResourceLoader::Data data = ResourceLoader::Load(resource_name, _T("WAVE"));
  return !data.IsNull() &&
         sndPlaySound((LPCTSTR)data.data,
                      SND_MEMORY | SND_ASYNC | SND_NODEFAULT);

#else
  StaticString<MAX_PATH> raw_file_name;
  InitialiseDataPath();
  LocalPath(raw_file_name.buffer(), _T("sound/"));

  if (strncmp(resource_name, beep_id_str, strlen(beep_id_str)) == 0) {
    resource_name += strlen(beep_id_str);
    if (strncmp(resource_name, wav_str, strlen(wav_str)) == 0)
      resource_name += strlen(wav_str);
  }
  StaticString<64> lower_resource_name;
  CopyASCIILower(lower_resource_name.buffer(), resource_name);

  raw_file_name.append(lower_resource_name.c_str());
  raw_file_name.append(".raw");

  const pid_t pid = fork();
  if (gcc_unlikely(pid < 0))
    return false;

  if (pid == 0) {
      RawPlayback *raw_playback = new RawPlayback();
      raw_playback->playback_file(raw_file_name);
      delete raw_playback;
      _exit(0);
  }
  return true;
#endif
}

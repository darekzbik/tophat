/*
  Copyright_License {

  Top Hat Soaring Glide Computer - http://www.tophatsoaring.org/
  Copyright (C) 2000-2016 The Top Hat Soaring Project
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

#include "Logger/LoggerImpl.hpp"
#include "Logger/Settings.hpp"
#include "LogFile.hpp"
#include "LocalPath.hpp"
#include "Device/Declaration.hpp"
#include "NMEA/Info.hpp"
#include "Simulator.hpp"
#include "OS/FileUtil.hpp"
#include "Formatter/IGCFilenameFormatter.hpp"
#include "Interface.hpp"
#include "Util/CharUtil.hpp"
#include "Util/StringAPI.hxx"
#include "IGCFileCleanup.hpp"
#include "IGC/IGCWriter.hpp"

#include <tchar.h>
#include <algorithm>

const struct LoggerImpl::PreTakeoffBuffer &
LoggerImpl::PreTakeoffBuffer::operator=(const NMEAInfo &src)
{
  location = src.location_available
    ? src.location
    : GeoPoint::Invalid();

  if (src.pressure_altitude_available) {
    pressure_altitude = src.pressure_altitude;
    pressure_altitude_available = true;
  } else if (src.baro_altitude_available) {
    pressure_altitude = src.baro_altitude;
    pressure_altitude_available = true;
  } else
    pressure_altitude_available = false;

  altitude_gps = src.gps_altitude;
  gps_altitude_available = src.gps_altitude_available;

  date_time_utc = src.date_time_utc;
  time = src.time;

  fix_quality = src.gps.fix_quality;

  satellites_used_available = src.gps.satellites_used_available;
  if (satellites_used_available)
    satellites_used = src.gps.satellites_used;

  hdop = src.gps.hdop;
  real = src.gps.real;

  satellite_ids_available = src.gps.satellite_ids_available;
  if (satellite_ids_available)
    std::copy_n(src.gps.satellite_ids, GPSState::MAXSATELLITES, satellite_ids);

  return *this;
}

LoggerImpl::LoggerImpl()
  :writer(nullptr)
{
  filename[0] = 0;
}

LoggerImpl::~LoggerImpl()
{
  delete writer;
}

void
LoggerImpl::StopLogger(const NMEAInfo &gps_info)
{
  // Logger can't be switched off if already off -> cancel
  if (writer == nullptr)
    return;

  writer->Flush();

  if (!simulator)
    writer->Sign();

  LogFormat(_T("Logger stopped: %s"), filename);

  // Logger off
  delete writer;
  writer = nullptr;

  // Make space for logger file, if unsuccessful -> cancel
  if (gps_info.gps.real && gps_info.date_time_utc.IsDatePlausible())
    IGCFileCleanup(gps_info.date_time_utc.year);

  pre_takeoff_buffer.clear();
}

void
LoggerImpl::LogPointToBuffer(const NMEAInfo &gps_info)
{
  assert(gps_info.alive);
  assert(gps_info.time_available);

  PreTakeoffBuffer item;
  item = gps_info;
  pre_takeoff_buffer.push(item);
}

void
LoggerImpl::LogEvent(const NMEAInfo &gps_info, const char *event)
{
  if (gps_info.location_available && !gps_info.gps.real)
    simulator = true;

  if (writer != nullptr)
    writer->LogEvent(gps_info, event);
}

void
LoggerImpl::LogPoint(const NMEAInfo &gps_info)
{
  if (!gps_info.alive || !gps_info.time_available)
    return;

  if (writer == nullptr) {
    LogPointToBuffer(gps_info);
    return;
  }

  while (!pre_takeoff_buffer.empty()) {
    const struct PreTakeoffBuffer &src = pre_takeoff_buffer.shift();
    if (!simulator && !src.real)
      /* ignore buffered "unreal" fixes if we're logging a real
         flight; should never happen, but who knows */
      continue;

    NMEAInfo tmp_info;
    tmp_info.Reset();

    // NOTE: clock is only used to set the validity of valid objects to true
    //       for which "1" is sufficient. This kludge needs to be rewritten.
    tmp_info.clock = fixed(1);

    tmp_info.alive.Update(tmp_info.clock);

    if (src.location.IsValid()) {
      tmp_info.location = src.location;
      tmp_info.location_available.Update(tmp_info.clock);
    }

    if (src.gps_altitude_available) {
      tmp_info.gps_altitude = src.altitude_gps;
      tmp_info.gps_altitude_available.Update(tmp_info.clock);
    }

    if (src.pressure_altitude_available) {
      tmp_info.pressure_altitude = src.pressure_altitude;
      tmp_info.pressure_altitude_available.Update(tmp_info.clock);
    }

    tmp_info.date_time_utc = src.date_time_utc;
    tmp_info.time = src.time;
    tmp_info.time_available.Update(tmp_info.clock);

    tmp_info.gps.fix_quality = src.fix_quality;

    if (src.satellites_used_available) {
      tmp_info.gps.satellites_used_available.Update(tmp_info.clock);
      tmp_info.gps.satellites_used = src.satellites_used;
    }

    tmp_info.gps.hdop = src.hdop;
    tmp_info.gps.real = src.real;

    if (src.satellite_ids_available) {
      tmp_info.gps.satellite_ids_available.Update(tmp_info.clock);
      for (unsigned i = 0; i < GPSState::MAXSATELLITES; i++)
        tmp_info.gps.satellite_ids[i] = src.satellite_ids[i];
    }

    WritePoint(tmp_info);
  }

  WritePoint(gps_info);
}

void
LoggerImpl::WritePoint(const NMEAInfo &gps_info)
{
  assert(gps_info.alive);
  assert(gps_info.time_available);

  if (gps_info.location_available && !gps_info.gps.real)
    simulator = true;

  if (!simulator && frecord.Update(gps_info.gps, gps_info.time,
                                   !gps_info.location_available)) {
    if (gps_info.gps.satellite_ids_available)
      writer->LogFRecord(gps_info.date_time_utc, gps_info.gps.satellite_ids);
    else
      writer->LogEmptyFRecord(gps_info.date_time_utc);
  }

  writer->LogPoint(gps_info);
}

bool
LoggerImpl::StartLogger(const NMEAInfo &gps_info,
                        const LoggerSettings &settings,
                        const char *logger_id)
{
  assert(logger_id != nullptr);
  assert(strlen(logger_id) == 3);

  /* finish the previous IGC file */
  StopLogger(gps_info);

  assert(writer == nullptr);

  LocalPath(filename, _T("logs"));
  Directory::Create(filename);

  const BrokenDate today = gps_info.date_time_utc.IsDatePlausible()
    ? (const BrokenDate &)gps_info.date_time_utc
    : BrokenDate::TodayUTC();

  StaticString<64> name;
  for (int i = 1; i < 99; i++) {
    FormatIGCFilenameLong(name.buffer(), today, "XCS", logger_id, i);

    LocalPath(filename, _T("logs"), name);
    if (!File::Exists(filename))
      break;  // file not exist, we'll use this name
  }

  frecord.Reset();
  writer = new IGCWriter(filename);
  if (!writer->IsOpen()) {
    LogFormat(_T("Failed to create file %s"), filename);
    delete writer;
    writer = nullptr;
    return false;
  }

  LogFormat(_T("Logger Started: %s"), filename);
  return true;
}

void
LoggerImpl::LoggerNote(const TCHAR *text)
{
  if (writer != nullptr)
    writer->LoggerNote(text);
}

static const TCHAR *
GetGPSDeviceName()
{
  if (is_simulator())
    return _T("Simulator");

  const DeviceConfig &device = CommonInterface::GetSystemSettings().devices[0];
  if (device.UsesDriver())
    return device.driver_name;

  if (device.IsAndroidInternalGPS())
    return _T("Internal GPS (Android)");

  return _T("Unknown");
}

// TODO: fix scope so only gui things can start it
void
LoggerImpl::StartLogger(const NMEAInfo &gps_info,
                        const LoggerSettings &settings,
                        const TCHAR *asset_number, const Declaration &decl)
{
  if (StringLength(settings.GetLoggerID()) > 0)
    asset_number = settings.GetLoggerID();

  // chars must be legal in file names
  char logger_id[4];
  unsigned asset_length = _tcslen(asset_number);
  for (unsigned i = 0; i < 3; i++)
    logger_id[i] = i < asset_length && IsAlphaNumericASCII(asset_number[i]) ?
                   asset_number[i] : _T('A');
  logger_id[3] = _T('\0');

  if (!StartLogger(gps_info, settings, logger_id))
    return;

  simulator = gps_info.location_available && !gps_info.gps.real;
  writer->WriteHeader(gps_info.date_time_utc, decl.pilot_name,
                      decl.aircraft_type, decl.aircraft_registration,
                      decl.competition_id,
                      logger_id, GetGPSDeviceName(), simulator);

  if (decl.Size()) {
    BrokenDateTime FirstDateTime = !pre_takeoff_buffer.empty()
      ? pre_takeoff_buffer.peek().date_time_utc
      : gps_info.date_time_utc;
    writer->StartDeclaration(FirstDateTime, decl.Size());

    for (unsigned i = 0; i< decl.Size(); ++i)
      writer->AddDeclaration(decl.GetLocation(i), decl.GetName(i));

    writer->EndDeclaration();
  }
}

void
LoggerImpl::ClearBuffer()
{
  pre_takeoff_buffer.clear();
}

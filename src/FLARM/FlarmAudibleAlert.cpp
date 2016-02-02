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

#include "FlarmAudibleAlert.hpp"
#include "NMEA/MoreData.hpp"
#include "Audio/Sound.hpp"
#include "Blackboard/LiveBlackboard.hpp"
#include "Event/Idle.hpp"
#include "FLARM/Status.hpp"
#include "Util/StaticString.hxx"

#include "LogFile.hpp" //debug

FlarmAudibleAlert::FlarmAudibleAlert(LiveBlackboard &_blackboard)
  :blackboard(_blackboard)
{
  Reset();
  blackboard.AddListener(*this);
}

FlarmAudibleAlert::~FlarmAudibleAlert() {
  blackboard.RemoveListener(*this);
}

void
FlarmAudibleAlert::Reset()
{
  traffic_oclock = -1;
  alarm_level = FlarmTraffic::AlarmType::NONE;
  traffic_above_below = TrafficAboveBelow::TRAFFIC_UNKNOWN;
  last_alert_time.Update();
}

/// returns 1-12 oclock
static int
CalculatTrafficClock(fixed relative_bearing_degrees)
{
  const Angle relative_bearing(Angle::Degrees(relative_bearing_degrees));
  int clock_mod_12 = (int)((relative_bearing.AsBearing().Degrees() + fixed(15)) / fixed(30)) % 12;
  return clock_mod_12 == 0 ? 12 : clock_mod_12;
}

static FlarmAudibleAlert::TrafficAboveBelow
CalculateAboveBelow(fixed relative_altitude, bool altitude_valid,
                    fixed distance, bool distance_valid)
{
  if (!altitude_valid || !distance_valid) {
    return FlarmAudibleAlert::TrafficAboveBelow::TRAFFIC_UNKNOWN;
  }
  const fixed rise_over_run = relative_altitude / distance;
  const fixed threshold = fixed(0.15);

  if (rise_over_run > threshold)
    return FlarmAudibleAlert::TrafficAboveBelow::TRAFFIC_ABOVE;
  else if (rise_over_run < -threshold)
    return FlarmAudibleAlert::TrafficAboveBelow::TRAFFIC_BELOW;
  else
    return FlarmAudibleAlert::TrafficAboveBelow::TRAFFIC_LEVEL;
}

void
FlarmAudibleAlert::PlayAlarm()
{
  ResetUserIdle();
  StaticString<64> resource;

  switch (traffic_oclock) {
  case (0):
    resource = _T("IDR_WAV_TRAFFIC");
    break;
  case (1):
    resource = _T("IDR_WAV_ONE_OCLOCK");
    break;
  case (2):
    resource = _T("IDR_WAV_TWO_OCLOCK");
    break;
  case (3):
    resource = _T("IDR_WAV_THREE_OCLOCK");
    break;
  case (4):
    resource = _T("IDR_WAV_FOUR_OCLOCK");
    break;
  case (5):
    resource = _T("IDR_WAV_FIVE_OCLOCK");
    break;
  case (6):
    resource = _T("IDR_WAV_SIX_OCLOCK");
    break;
  case (7):
    resource = _T("IDR_WAV_SEVEN_OCLOCK");
    break;
  case (8):
    resource = _T("IDR_WAV_EIGHT_OCLOCK");
    break;
  case (9):
    resource = _T("IDR_WAV_NINE_OCLOCK");
    break;
  case (10):
    resource = _T("IDR_WAV_TEN_OCLOCK");
    break;
  case (11):
    resource = _T("IDR_WAV_ELEVEN_OCLOCK");
    break;
  case (12):
    resource = _T("IDR_WAV_TWELVE_OCLOCK");
    break;
  }
#ifdef DOABOVEBELOW
  if (traffic_above_below == TrafficAboveBelow::TRAFFIC_BELOW)
    resource.append(_T("_BELOW"));
  else if (traffic_above_below == TrafficAboveBelow::TRAFFIC_ABOVE)
    resource.append(_T("_ABOVE"));
#endif
  PlayResource(resource.c_str());
}

bool
FlarmAudibleAlert::IsAlertAudible(FlarmTraffic::AlarmType type)
{
  return (unsigned)type >= (unsigned)FlarmTraffic::AlarmType::LOW &&
      (unsigned)type <= (unsigned)FlarmTraffic::AlarmType::URGENT;
}

void
FlarmAudibleAlert::Update(const FlarmStatus& status)
{
  if (!status.priority_intruder.relative_altitude_valid ||
      !status.priority_intruder.relative_bearing_degrees_valid) {
    return;
  }

  const FlarmStatus::PriorityIntruder intruder = status.priority_intruder;
  const FlarmTraffic::AlarmType new_alarm_level = status.alarm_level;
  int new_clock = CalculatTrafficClock(status.priority_intruder.relative_bearing_degrees);
  TrafficAboveBelow new_above_below =
      CalculateAboveBelow(intruder.relative_altitude,
                          intruder.relative_altitude_valid,
                          intruder.distance, intruder.distance_valid);

  bool play = false;

  // alarm has changed
  if (IsAlertAudible(new_alarm_level) && (
      traffic_oclock != new_clock ||
      traffic_above_below != new_above_below))
    play = true;

  // new alarm
  if (!IsAlertAudible(alarm_level) && IsAlertAudible(new_alarm_level))
    play = true;

  // coming straight at you, play every 5 seconds max
  if (IsAlertAudible(new_alarm_level) &&
      (unsigned)new_alarm_level > (unsigned)alarm_level &&
      last_alert_time.Elapsed() > 6000)
    play = true;

  if (play) {
    traffic_oclock = new_clock;
    traffic_above_below = new_above_below;
    alarm_level = new_alarm_level;
    last_alert_time.Update();
    LogFormat("FlarmAudibleAlert::Update alert:%i above_below:%u rel_alt:%.0f dist:%.0f rise/run:%.3f",
              (int)alarm_level, (unsigned)new_above_below, (double)intruder.relative_altitude,
              (double)intruder.distance,
              (double)intruder.relative_altitude / (double)intruder.distance);
    PlayAlarm();
  }
}

void
FlarmAudibleAlert::OnGPSUpdate(const MoreData &basic)
{
  const FlarmStatus& status = basic.flarm.status;
  if (status.priority_intruder.alarm_type ==
      FlarmStatus::PriorityIntruder::AlarmTypePFLAU::AIRCRAFT &&
      status.gps == FlarmStatus::GPSStatus::GPS_3D) {
      // TODO: fix this enum.
      // Should be 0 = no GPS reception 1 = 3d-fix on ground, i.e. not airborne 2 = 3d-fix when airborne
    Update(status);
  } else
    Reset();
}


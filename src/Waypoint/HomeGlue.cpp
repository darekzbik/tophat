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

#include "WaypointGlue.hpp"
#include "Profile/Map.hpp"
#include "Profile/ProfileKeys.hpp"
#include "Blackboard/DeviceBlackboard.hpp"
#include "LogFile.hpp"
#include "Terrain/RasterTerrain.hpp"
#include "Waypoint/Waypoints.hpp"
#include "LastUsed.hpp"
#include "Util/tstring.hpp"
#include "Util/ConvertString.hpp"

#include <stdlib.h>

const Waypoint *
WaypointGlue::FindHomeId(Waypoints &waypoints,
                         PlacesOfInterestSettings &settings)
{
  if (settings.home_waypoint < 0)
    return nullptr;

  const Waypoint *wp = waypoints.LookupId(settings.home_waypoint);
  if (wp == nullptr) {
    settings.home_waypoint = -1;
    return nullptr;
  }

  settings.home_location = wp->location;
  settings.home_location_available = true;
  settings.home_elevation = wp->elevation;
  settings.home_elevation_available = true;
  waypoints.SetHome(wp->id);
  return wp;
}

const Waypoint *
WaypointGlue::FindHomeLocation(Waypoints &waypoints,
                               PlacesOfInterestSettings &settings)
{
  if (!settings.home_location_available) {
    settings.home_elevation_available = false;
    return nullptr;
  }

  const Waypoint *wp = waypoints.LookupLocation(settings.home_location,
                                                fixed(100));
  if (wp == nullptr) {
    settings.home_location_available = false;
    settings.home_elevation_available = false;
    return nullptr;
  }

  settings.home_elevation = wp->elevation;
  settings.home_elevation_available = true;

  settings.home_waypoint = wp->id;
  waypoints.SetHome(wp->id);
  return wp;
}

const Waypoint *
WaypointGlue::FindFlaggedHome(Waypoints &waypoints,
                              PlacesOfInterestSettings &settings)
{
  const Waypoint *wp = waypoints.FindHome();
  if (wp == nullptr)
    return nullptr;

  settings.SetHome(*wp);
  return wp;
}

/**
 * if the wp location in poi_settings is valid, and the name is stored
 * in the profile, and we're flying, then create a new waypoint with this
 * information and append it to the waypoint database
 *
 * @param ProfileMap
 * @param waypoints the database
 * @param poi_settings info about last home waypoint
 * @param flying.  true if state of aircraft is flying
 * @return pointer to waypoint in waypoint database or nullptr if not added
 */
static const Waypoint*
CreateHomeAndAppend(ProfileMap &profile, Waypoints &waypoints,
                    PlacesOfInterestSettings &poi_settings,
                    bool flying)
{
  // if not flying, the don't save home if wp file is changed - recalc based on
  // new wp file and/or new terrain
  // but if flying, then retain home even when waypoint file changes
  if (!poi_settings.home_location.IsValid() ||
      !flying)
    return nullptr;
//  TCHAR blank[] = _T("");
  tstring home_waypoint_name;
  UTF8ToWideConverter text2(profile.Get(ProfileKeys::HomeWaypointName, ""));
  if (text2.IsValid())
    home_waypoint_name = text2;

  if (home_waypoint_name.length() == 0)
    return nullptr;

  Waypoint wp_temp(poi_settings.home_location);
  wp_temp.name = home_waypoint_name;
  if (poi_settings.home_elevation_available)
    wp_temp.elevation = poi_settings.home_elevation;

  const Waypoint wp_new = waypoints.Append(std::move(wp_temp));
  poi_settings.home_waypoint = wp_new.id;
  poi_settings.home_location_available = true;
  waypoints.SetHome(wp_new.id);

  return waypoints.GetHome();
}

void
WaypointGlue::SetHome(ProfileMap &profile,
                      Waypoints &way_points, const RasterTerrain *terrain,
                      PlacesOfInterestSettings &poi_settings,
                      TeamCodeSettings &team_code_settings,
                      DeviceBlackboard *device_blackboard,
                      const bool reset)
{
  if (reset)
    poi_settings.home_waypoint = -1;

  // check invalid home waypoint or forced reset due to file change
  const Waypoint *wp = FindHomeId(way_points, poi_settings);
  if (wp == nullptr) {
    /* fall back to HomeLocation, try to find it in the waypoint
       database */
    wp = FindHomeLocation(way_points, poi_settings);
    // create a new waypoint if home exists but can't be found in the wp file
    if (wp == nullptr)
      wp = CreateHomeAndAppend(profile, way_points, poi_settings,
                               device_blackboard->Calculated().flight.flying);

    if (wp == nullptr)
      // search for home in waypoint list, if we don't have a home
      wp = FindFlaggedHome(way_points, poi_settings);
  }

  if (wp != nullptr)
    LastUsedWaypoints::Add(*wp);

  // check invalid task ref waypoint or forced reset due to file change
  if (reset || way_points.IsEmpty() ||
      !way_points.LookupId(team_code_settings.team_code_reference_waypoint))
    // set team code reference waypoint if we don't have one
    team_code_settings.team_code_reference_waypoint = poi_settings.home_waypoint;

  if (poi_settings.home_location_available) {
    profile.SetGeoPoint(ProfileKeys::HomeLocation, poi_settings.home_location);
    const Waypoint *wp = way_points.LookupId(poi_settings.home_waypoint);
    tstring name;
    if (wp != nullptr)
      name = wp->name.c_str();
    profile.Set(ProfileKeys::HomeWaypointName, name.c_str());
  }

  if (device_blackboard != nullptr) {
    if (wp != nullptr) {
      // OK, passed all checks now
      LogFormat("Start at home waypoint");
      device_blackboard->SetStartupLocation(wp->location, wp->elevation);
    } else if (terrain != nullptr) {
      // no home at all, so set it from center of terrain if available
      GeoPoint loc = terrain->GetTerrainCenter();
      LogFormat("Start at terrain center");
      device_blackboard->SetStartupLocation(loc,
                                            fixed(terrain->GetTerrainHeightOr0(loc)));
    }
  }
}

void
WaypointGlue::SaveHome(ProfileMap &profile,
                       const PlacesOfInterestSettings &poi_settings,
                       const TeamCodeSettings &team_code_settings)
{
  profile.Set(ProfileKeys::HomeWaypoint, poi_settings.home_waypoint);
  if (poi_settings.home_location_available) {
    profile.SetGeoPoint(ProfileKeys::HomeLocation, poi_settings.home_location);
  }

  profile.Set(ProfileKeys::HomeElevationAvailable, poi_settings.home_elevation_available);
  if (poi_settings.home_elevation_available)
    profile.Set(ProfileKeys::HomeElevation, poi_settings.home_elevation);

  profile.Set(ProfileKeys::TeamcodeRefWaypoint,
              team_code_settings.team_code_reference_waypoint);
}

void
WaypointGlue::SaveATCReference(ProfileMap &profile,
                               const PlacesOfInterestSettings &poi_settings)
{
  if (poi_settings.atc_reference.IsValid())
    profile.SetGeoPoint(ProfileKeys::ATCReference, poi_settings.atc_reference);
}

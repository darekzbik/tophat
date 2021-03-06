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

#ifndef XCSOAR_GEO_WGS84_HPP
#define XCSOAR_GEO_WGS84_HPP

#include "Math/fixed.hpp"

namespace WGS84 {
  static constexpr double EQUATOR_RADIUS = 6378137;
  static constexpr double FLATTENING = 1 / 298.257223563;
  static constexpr double POLE_RADIUS = EQUATOR_RADIUS * (1 - FLATTENING);

  namespace Fixed {
    static constexpr fixed EQUATOR_RADIUS = fixed(WGS84::EQUATOR_RADIUS);
    static constexpr fixed POLE_RADIUS = fixed(WGS84::POLE_RADIUS);
    static constexpr fixed FLATTENING = fixed(WGS84::FLATTENING);
  }
}

#endif

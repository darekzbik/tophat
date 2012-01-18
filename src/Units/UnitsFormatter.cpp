/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
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

#include "Units/UnitsFormatter.hpp"
#include "Units/Units.hpp"
#include "Units/Descriptor.hpp"
#include "Math/Angle.hpp"
#include "Atmosphere/Pressure.hpp"
#include "DateTime.hpp"
#include "Util/StringUtil.hpp"

#include <stdio.h>
#include <stdlib.h>

static void
FormatInteger(TCHAR *buffer, size_t size,
              const fixed value, const Unit unit, bool include_unit,
              bool include_sign)
{
  const fixed uvalue = Units::ToUserUnit(value, unit);
  const int ivalue = iround(uvalue);

  if (include_unit)
    _sntprintf(buffer, size, include_sign ? _T("%+d %s") : _T("%d %s"), ivalue,
               Units::unit_descriptors[(unsigned)unit].name);
  else
    _sntprintf(buffer, size, include_sign ? _T("%+d") : _T("%d"), ivalue);
}

void
Units::FormatAltitude(TCHAR *buffer, size_t size, fixed value, Unit unit,
                      bool include_unit)
{
  FormatInteger(buffer, size, value, unit, include_unit, false);
}

void
Units::FormatUserAltitude(fixed value, TCHAR *buffer, size_t size,
                          bool include_unit)
{
  FormatAltitude(buffer, size, value, current.altitude_unit, include_unit);
}

gcc_const
static Unit
GetAlternateUnit(Unit unit)
{
  switch (unit) {
  case Unit::METER:
    return Unit::FEET;

  case Unit::FEET:
    return Unit::METER;

  default:
    return unit;
  }
}

void
Units::FormatAlternateUserAltitude(fixed value, TCHAR *buffer, size_t size,
                                   bool include_unit)
{
  FormatAltitude(buffer, size, value, GetAlternateUnit(current.altitude_unit),
                 include_unit);
}

void
Units::FormatRelativeAltitude(TCHAR *buffer, size_t size, fixed value,
                              Unit unit, bool include_unit)
{
  FormatInteger(buffer, size, value, unit, include_unit, true);
}

void
Units::FormatUserArrival(fixed value, TCHAR *buffer, size_t size,
                         bool include_unit)
{
  FormatRelativeAltitude(buffer, size, value, current.altitude_unit,
                         include_unit);
}

void
Units::FormatUserDistance(fixed _value, TCHAR *buffer, size_t size,
                          bool include_unit)
{
  int prec;
  fixed value;

  const UnitDescriptor *descriptor =
      &unit_descriptors[(unsigned)current.distance_unit];

  value = _value * descriptor->factor_to_user; // + descriptor->ToUserOffset;

  if (value >= fixed(100))
    prec = 0;
  else if (value > fixed_ten)
    prec = 1;
  else if (!include_unit)
    prec = 2;
  else {
    prec = 2;
    if (current.distance_unit == Unit::KILOMETER) {
      prec = 0;
      descriptor = &unit_descriptors[(unsigned)Unit::METER];
      value = _value * descriptor->factor_to_user;
    }
    if (current.distance_unit == Unit::NAUTICAL_MILES ||
        current.distance_unit == Unit::STATUTE_MILES) {
      descriptor = &unit_descriptors[(unsigned)Unit::FEET];
      value = _value * descriptor->factor_to_user;
      if (value < fixed(1000)) {
        prec = 0;
      } else {
        prec = 1;
        descriptor = &unit_descriptors[(unsigned)current.distance_unit];
        value = _value * descriptor->factor_to_user;
      }
    }
  }

  if (include_unit)
    _sntprintf(buffer, size, _T("%.*f %s"), prec, (double)value, descriptor->name);
  else
    _sntprintf(buffer, size, _T("%.*f"), prec, (double)value);
}

void
Units::FormatUserMapScale(fixed _value, TCHAR *buffer,
                          size_t size, bool include_unit)
{
  int prec;
  fixed value;

  const UnitDescriptor *descriptor =
      &unit_descriptors[(unsigned)current.distance_unit];

  value = _value * descriptor->factor_to_user; // + descriptor->ToUserOffset;

  if (value >= fixed(9.999))
    prec = 0;
  else if ((current.distance_unit == Unit::KILOMETER && value >= fixed(0.999)) ||
           (current.distance_unit != Unit::KILOMETER && value >= fixed(0.160)))
    prec = 1;
  else if (!include_unit)
    prec = 2;
  else {
    prec = 2;
    if (current.distance_unit == Unit::KILOMETER) {
      prec = 0;
      descriptor = &unit_descriptors[(unsigned)Unit::METER];
      value = _value * descriptor->factor_to_user;
    }
    if (current.distance_unit == Unit::NAUTICAL_MILES ||
        current.distance_unit == Unit::STATUTE_MILES) {
      prec = 0;
      descriptor = &unit_descriptors[(unsigned)Unit::FEET];
      value = _value * descriptor->factor_to_user;
    }
  }

  if (include_unit)
    _sntprintf(buffer, size, _T("%.*f %s"), prec, (double)value, descriptor->name);
  else
    _sntprintf(buffer, size, _T("%.*f"), prec, (double)value);
}

void
Units::FormatSpeed(TCHAR *buffer, size_t size,
            fixed value, const Unit unit, bool include_unit, bool precision)
{
  value = Units::ToUserUnit(value, unit);

  const int prec = precision && value < fixed(100);
  if (include_unit)
    _sntprintf(buffer, size, _T("%.*f %s"),
               prec, (double)value, Units::unit_descriptors[(unsigned)unit].name);
  else
    _sntprintf(buffer, size, _T("%.*f"),
               prec, (double)value);
}

void
Units::FormatUserSpeed(fixed value, TCHAR *buffer, size_t size,
                       bool include_unit, bool precision)
{
  FormatSpeed(buffer, size, value, current.speed_unit,
              include_unit, precision);
}

void
Units::FormatUserWindSpeed(fixed value, TCHAR *buffer, size_t size,
                           bool include_unit, bool precision)
{
  FormatSpeed(buffer, size, value, current.wind_speed_unit,
              include_unit, precision);
}

void
Units::FormatUserTaskSpeed(fixed value, TCHAR *buffer, size_t size,
                           bool include_unit, bool precision)
{
  FormatSpeed(buffer, size, value, current.task_speed_unit,
              include_unit, precision);
}

void
Units::FormatUserVSpeed(fixed value, TCHAR *buffer, size_t size,
                        bool include_unit)
{
  const UnitDescriptor *descriptor =
      &unit_descriptors[(unsigned)current.vertical_speed_unit];

  value = value * descriptor->factor_to_user;

  if (include_unit)
    _sntprintf(buffer, size, _T("%+.1f %s"), (double)value, descriptor->name);
  else
    _sntprintf(buffer, size, _T("%+.1f"), (double)value);
}

void
Units::FormatUserTemperature(fixed value, TCHAR *buffer, size_t size,
                             bool include_unit)
{
  value = ToUserTemperature(value);

  if (include_unit)
    _sntprintf(buffer, size, _T("%.0f %s"), (double)value,
               GetTemperatureName());
  else
    _sntprintf(buffer, size, _T("%.0f"), (double)value);
}

void
Units::FormatUserPressure(AtmosphericPressure pressure, TCHAR *buffer,
                          size_t size, bool include_unit)
{
  TCHAR buffer2[16];
  const UnitDescriptor *descriptor =
      &unit_descriptors[(unsigned)current.pressure_unit];

  fixed _pressure = pressure.GetHectoPascal() * descriptor->factor_to_user;

  if (include_unit) {
    TCHAR format[8];
    _tcscpy(format, GetFormatUserPressure());
    _tcscat(format, _T(" %s") );
    _stprintf(buffer2, format, (double)_pressure, descriptor->name);
  } else
    _stprintf(buffer2, GetFormatUserPressure(),  (double)_pressure);

  if (_tcslen(buffer2) < size - 1)
    _tcscpy(buffer, buffer2);
  else
    CopyString(buffer, buffer2, size);
}

const TCHAR*
Units::GetFormatUserPressure()
{
  if (current.pressure_unit == Unit::INCH_MERCURY)
    return _T("%.2f");
  else
    return _T("%.f");
}

fixed
Units::PressureStep()
{
  if (current.pressure_unit == Unit::INCH_MERCURY)
    return fixed(0.01);
  else
    return fixed_one;
}

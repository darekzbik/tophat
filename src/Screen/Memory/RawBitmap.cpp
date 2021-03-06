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

#include "../RawBitmap.hpp"
#include "Canvas.hpp"

#include <assert.h>

/**
 * Returns minimum width that is greater then the given width and
 * that is acceptable as image width (not all numbers are acceptable)
 */
static inline unsigned
CorrectedWidth(unsigned nWidth)
{
  return ((nWidth + 3) / 4) * 4;
}

RawBitmap::RawBitmap(unsigned nWidth, unsigned nHeight)
  :width(nWidth), height(nHeight),
   corrected_width(CorrectedWidth(nWidth)),
   buffer(new BGRColor[corrected_width * height])
{
}

RawBitmap::~RawBitmap()
{
  delete[] buffer;
}

void
RawBitmap::StretchTo(unsigned width, unsigned height,
                     Canvas &dest_canvas,
                     unsigned dest_width, unsigned dest_height) const
{
  ConstImageBuffer<ActivePixelTraits> src(ActivePixelTraits::const_pointer_type(buffer),
                                          corrected_width * sizeof(*buffer),
                                          width, height);

  dest_canvas.Stretch(0, 0, dest_width, dest_height,
                      src, 0, 0, width, height);
}

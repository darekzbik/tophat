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

#include "Thread.hpp"
#include "TopographyStore.hpp"
#include "Thread/Util.hpp"

TopographyThread::TopographyThread(TopographyStore &_store,
                                   std::function<void()> &&_callback)
  :StandbyThread("Topography"),
   store(_store),
   callback(_callback),
   last_bounds(GeoBounds::Invalid()) {}

TopographyThread::~TopographyThread()
{
}

void
TopographyThread::Trigger(const WindowProjection &_projection)
{
  assert(_projection.IsValid());

  const GeoBounds new_bounds = _projection.GetScreenBounds();
  if (last_bounds.IsValid() && last_bounds.IsInside(new_bounds)) {
    /* still inside cache bounds - now check if we crossed a scale
       threshold for at least one file, which would mean we have to
       update a file which was not updated for the current cache
       bounds */
    if (negative(scale_threshold) ||
        _projection.GetMapScale() >= scale_threshold)
      /* the cache is still fresh */
      return;
  }

  last_bounds = new_bounds.Scale(fixed(1.1));
  scale_threshold = store.GetNextScaleThreshold(_projection.GetMapScale());

  {
    const ScopeLock protect(mutex);
    next_projection = _projection;
    StandbyThread::Trigger();
  }
}

void
TopographyThread::Tick()
{
  // TODO: call only once
  SetIdlePriority();

  bool again = true;
  while (next_projection.IsValid() && again && !IsStopped()) {
    const WindowProjection projection = next_projection;

    mutex.Unlock();
    again = store.ScanVisibility(projection, 1) > 0;
    mutex.Lock();
  }

  /* notify the client that we have updated the topography cache */
  if (callback) {
    mutex.Unlock();
    callback();
    mutex.Lock();
  }
}

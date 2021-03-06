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

#ifndef XCSOAR_SCREEN_BRUSH_HPP
#define XCSOAR_SCREEN_BRUSH_HPP

#include "Screen/Color.hpp"
#include "Screen/Features.hpp"
#include "Debug.hpp"
#include "Compiler.h"

#include <assert.h>

#ifdef USE_GDI
class Bitmap;
#endif

/**
 * A Brush is used for drawing filled circles, rectangles and so on
 */
class Brush
{
protected:
#ifndef USE_GDI
  Color color;
#else
  HBRUSH brush;
#endif

public:
#ifndef USE_GDI
  constexpr Brush():color(Color::Transparent()) {}

  constexpr
  explicit Brush(const Color _color):color(_color)  {}
#else
  /** Base Constructor of the Brush class */
  Brush():brush(nullptr) {}

  /**
   * Constructor (creates a Brush object of the given Color
   * @param c Color of the Brush
   */
  explicit Brush(const Color c):brush(nullptr) {
    Create(c);
  }

  /** Destructor */
  ~Brush() {
    Destroy();
  }

  Brush(const Brush &other) = delete;
  Brush &operator=(const Brush &other) = delete;
#endif

public:
  /**
   * Sets the Color of the Brush
   * @param c The new Color
   */
  void Create(const Color c);

#ifdef HAVE_HATCHED_BRUSH

  /**
   * Creates a bitmap-based Brush
   * @param bitmap The bitmap the new Brush will be based on
   */
  void Create(const Bitmap &bitmap);

#endif

  /**
   * Resets the Brush to nullptr
   */
  void Destroy();

  /**
   * Returns whether the Brush is defined (!= nullptr)
   * @return True if the Brush is defined, False otherwise
   */
  bool
  IsDefined() const
  {
#ifndef USE_GDI
    return !color.IsTransparent();
#else
    return brush != nullptr;
#endif
  }

#ifndef USE_GDI
  constexpr bool IsHollow() const {
    return color.IsTransparent();
  }

  const Color GetColor() const { return color; }
#else
  /**
   * Returns the native HBRUSH object
   * @return The native HBRUSH object
   */
  HBRUSH Native() const {
    return brush;
  }
#endif

#ifdef ENABLE_OPENGL
  /**
   * Configures this brush in the OpenGL context.
   */
  void Bind() const {
    color.Bind();
  }

#ifdef USE_GLSL
  void BindUniform(GLint location) const {
    color.Uniform(location);
  }
#endif
#endif /* OPENGL */
};

#ifndef USE_GDI

inline void
Brush::Create(const Color c)
{
  assert(IsScreenInitialized());

  color = c;
}

inline void
Brush::Destroy()
{
  assert(!IsDefined() || IsScreenInitialized());

  color = Color::Transparent();
}

#endif

#endif

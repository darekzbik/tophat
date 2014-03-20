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

#ifndef XCSOAR_SCREENS_BUTTON_WIDGET_HPP
#define XCSOAR_SCREENS_BUTTON_WIDGET_HPP

#include "OverlayButtonWidget.hpp"
#include "MapOverlayButton.hpp"
#include "Form/Button.hpp"
#include "Form/ActionListener.hpp"
#include "Look/ButtonLook.hpp"
#include "InfoBoxes/InfoBoxLayout.hpp"

#include <tchar.h>

struct IconLook;
class ContainerWindow;
struct PixelRect;

class ScreensButtonWidget : public OverlayButtonWidget {
public:

protected:
  MapOverlayButton::Screens::ButtonPosition button_position;

public:
  ScreensButtonWidget()
    :OverlayButtonWidget(),
     button_position(MapOverlayButton::Screens::ButtonPosition::Bottom)
  {}

  /**
   * Shows or hides the widgets based on these parameters
   * @rc. the rc of the map
   * @is_panning.  is the map in panning mode
   * @is_main_window_widget. is the mainWindow's widget non-NULL
   * @is_map. is the map non-NULL
   */
  virtual void UpdateVisibility(const PixelRect &rc, bool is_panning,
                                bool is_main_window_widget, bool is_map);
  virtual void Prepare(ContainerWindow &parent, const PixelRect &rc);
  virtual void Move(const PixelRect &rc);

  /**
   * The OnAction is derived from ActionListener
   */
  virtual void OnAction(int id);

  /**
   * updates the text on the button
   */
  void UpdateText();

  /**
   * return position of S button based on screen layout
   */
  MapOverlayButton::Screens::ButtonPosition GetButtonPosition() const {
    return button_position;
  }
};

#endif

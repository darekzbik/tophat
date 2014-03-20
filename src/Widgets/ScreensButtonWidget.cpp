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

#include "ScreensButtonWidget.hpp"
#include "Widgets/MapOverlayButton.hpp"
#include "Form/SymbolButton.hpp"
#include "UIGlobals.hpp"
#include "Look/DialogLook.hpp"
#include "Look/IconLook.hpp"
#include "Screen/Bitmap.hpp"
#include "Screen/Layout.hpp"
#include "Interface.hpp"
#include "MainWindow.hpp"
#include "Look/Look.hpp"
#include "Screen/Canvas.hpp"
#include "Interface.hpp"
#include "Input/InputEvents.hpp"
#include "InfoBoxes/InfoBoxSettings.hpp"
#include "PageSettings.hpp"
#include "PageState.hpp"
#include "PageActions.hpp"
#include "UIState.hpp"

void
ScreensButtonWidget::UpdateVisibility(const PixelRect &rc,
                                       bool is_panning,
                                       bool is_main_window_widget,
                                       bool is_map)
{
  if (is_main_window_widget || (!is_panning &&
      CommonInterface::GetUISettings().screens_button_location ==
          UISettings::ScreensButtonLocation::MAP &&
          CommonInterface::SetUISettings().pages.n_pages > 1)) {
    Show(rc);
  } else
    Hide();
}

void
ScreensButtonWidget::Prepare(ContainerWindow &parent,
                              const PixelRect &rc)
{
  OverlayButtonWidget::Prepare(parent, rc);
  UpdateText();
}

void
ScreensButtonWidget::Move(const PixelRect &rc_map)
{
  const UISettings &ui_settings = CommonInterface::GetUISettings();
  const PixelRect rc_main = UIGlobals::GetMainWindow().GetClientRect();

  PixelRect rc;

  button_position = MapOverlayButton::Screens::GetButtonPosition(
      ui_settings.info_boxes.geometry, Layout::landscape);

  switch (button_position) {
  case  MapOverlayButton::Screens::ButtonPosition::Left:
    rc.left = 0;
    rc.right = rc.left + GetWidth();
    rc.bottom = rc_main.GetCenter().y;
    rc.top = rc.bottom - GetHeight();
  break;

  case  MapOverlayButton::Screens::ButtonPosition::Right:
    rc.right = rc_main.right;
    rc.left = rc.right - GetWidth();
    rc.bottom = rc_main.GetCenter().y;
    rc.top = rc.bottom - GetHeight();
  break;

  case  MapOverlayButton::Screens::ButtonPosition::Bottom:
    rc.left = rc_main.GetCenter().x - GetWidth() / 2;
    rc.right = rc.left + GetWidth();
    rc.bottom = rc_main.bottom;
    rc.top = rc.bottom - GetHeight();
  break;
}
  WindowWidget::Move(rc);
  GetWindow().Move(rc);
}

void
ScreensButtonWidget::UpdateText()
{

  const PagesState &state = CommonInterface::GetUIState().pages;
  TCHAR line_two[50];
  const PageLayout *pl =
    &CommonInterface::SetUISettings().pages.pages[state.current_index];
  const InfoBoxSettings &info_box_settings =
    CommonInterface::GetUISettings().info_boxes;
  assert(pl != NULL);

  pl->MakeInfoBoxSetTitle(info_box_settings, line_two);
  SetLineTwoText(line_two);

  SetText(_T("S"));
}

void
ScreensButtonWidget::OnAction(int id)
{
  InputEvents::HideMenu();
  InputEvents::eventScreenModes(_T("next"));
  UpdateText();
}

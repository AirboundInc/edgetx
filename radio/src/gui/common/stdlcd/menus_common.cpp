/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "edgetx.h"

MenuHandlerFunc menuHandlers[5];
event_t menuEvent = 0;
uint8_t menuVerticalPositions[4];
uint8_t menuVerticalOffsets[4];
uint8_t menuLevel = 0;

void popMenu()
{
  killEvents(KEY_EXIT);
  if (menuLevel == 0) return;

  menuLevel = menuLevel - 1;
  menuEvent = EVT_ENTRY_UP;
  // TODO ? AUDIO_KEY_PRESS();
  TRACE("popMenu(%d)", menuLevel);
}

void abortPopMenu()
{
  menuLevel = menuLevel + 1;
  menuEvent = 0;
  TRACE("popMenu(%d) aborted", menuLevel);
}

#if defined(MODEL_LOCK)
void menuCommonCalibOptions(event_t event);

// Refuse every Model / Radio tab and the stick direction options
static bool isLockedMenu(MenuHandlerFunc newMenu)
{
  bool locked = (newMenu == menuCommonCalibOptions);
  for (const auto& tab : menuTabModel) locked |= (tab.menuFunc == newMenu);
  for (const auto& tab : menuTabGeneral) locked |= (tab.menuFunc == newMenu);
  if (locked) POPUP_WARNING("Locked");
  return locked;
}
#endif

void chainMenu(MenuHandlerFunc newMenu)
{
  killAllEvents();
#if defined(MODEL_LOCK)
  if (isLockedMenu(newMenu)) return;
#endif
  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
  // TODO ? AUDIO_KEY_PRESS();
  TRACE("chainMenu(%d, %p)", menuLevel, newMenu);
}

void pushMenu(MenuHandlerFunc newMenu)
{
  killAllEvents();
  if (menuLevel == DIM(menuHandlers) - 1) return;
#if defined(MODEL_LOCK)
  if (isLockedMenu(newMenu)) return;
#endif

  if (menuLevel == 0) {
    if (newMenu == menuTabGeneral[0].menuFunc)
      menuVerticalPositions[0] = 1;
    if (newMenu == menuModelSelect)
      menuVerticalPositions[0] = 0;
#if defined(MODEL_LOCK)
    // name edits on the locked screen are saved to the model
    if (newMenu == menuModelLocked)
      menuVerticalPositions[0] = 0;
#endif
  }
  else {
    menuVerticalPositions[menuLevel] = menuVerticalPosition;
  }
  menuVerticalOffsets[menuLevel] = menuVerticalOffset;

  menuLevel++;

  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
  // ? AUDIO_KEY_PRESS();
  TRACE("pushMenu(%d, %p)", menuLevel, newMenu);
}

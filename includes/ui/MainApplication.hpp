// Sigpatch Updater
// Copyright (c) 2020 HarukoNX
// 
// This is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <pu/Plutonium>
#include <string>
#include <switch.h>
#include <ui/PatchLayout.hpp>
#include <ui/UpdateLayout.hpp>

namespace ui
{
  class MainApplication : public pu::ui::Application
  {
    public:
      using Application::Application;
      PU_SMART_CTOR(MainApplication)

      void OnLoad() override;

      void LockHomeButton();
      void SetAppletType(AppletType appletType);
      void SetAppPath(std::string appPath);
      void ShowPatch();
      void UnlockHomeButton();

    private:
      PatchLayout::Ref patchLayout = nullptr;
      UpdateLayout::Ref updateLayout = nullptr;
      AppletType appletType;
      std::string appPath;
      bool homeButtonLocked = false;

  };
}

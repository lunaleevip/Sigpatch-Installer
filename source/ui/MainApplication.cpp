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

#include <ui/MainApplication.hpp>

namespace ui
{
  void MainApplication::OnLoad()
  {
    this->LockHomeButton();

    this->updateLayout = UpdateLayout::New();
    this->updateLayout->SetAppPath(this->appPath);

    this->patchLayout = PatchLayout::New();

    this->LoadLayout(this->updateLayout);
  }

  void MainApplication::LockHomeButton()
  {
    if((this->appletType != AppletType_Application && this->appletType != AppletType_SystemApplication) || this->homeButtonLocked)
    {
      return;
    }

    appletBeginBlockingHomeButtonShortAndLongPressed(0);

    homeButtonLocked = true;
  }

  void MainApplication::SetAppletType(AppletType appletType)
  {
    this->appletType = appletType;
  }

  void MainApplication::SetAppPath(std::string appPath)
  {
    this->appPath = appPath;
  }

  void MainApplication::ShowPatch()
  {
    this->LoadLayout(this->patchLayout);
  }

  void MainApplication::UnlockHomeButton()
  {
    if((this->appletType != AppletType_Application && this->appletType != AppletType_SystemApplication) || !this->homeButtonLocked)
    {
      return;
    }

    appletEndBlockingHomeButtonShortAndLongPressed();

    homeButtonLocked = false;
  }
}

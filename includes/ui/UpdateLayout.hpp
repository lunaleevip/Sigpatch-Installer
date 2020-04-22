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
#include <vector>

namespace ui
{
  class UpdateLayout : public pu::ui::Layout
  {
    public:
      UpdateLayout();
      PU_SMART_CTOR(UpdateLayout)

      void Thread();
      void ProgressUpdate(double progress);
      bool ParseAppUpdateData(std::vector<char> data);
      void SetAppPath(std::string appPath);
      void UpdateLoadingText(std::string text);

    private:
      pu::ui::elm::Rectangle::Ref titleRect;
      pu::ui::elm::TextBlock::Ref titleText;
      pu::ui::elm::ProgressBar::Ref progressBar;
      pu::ui::elm::TextBlock::Ref loadingText;
      std::string appPath;
    
    private:
      bool hekateChecked = false;

    private:
      bool internetChecked = false;
      bool hasInternet = false;

    private:
      bool checkingForUpdate = false;
      bool checkedForUpdate = false;
      bool downloadingUpdate = false;
      bool downloadedUpdate = false;
      bool requiresUpdate = false;
      std::string updateUrl = "";

  };
}

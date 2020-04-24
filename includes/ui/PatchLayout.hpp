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

namespace ui
{
  class PatchLayout : public pu::ui::Layout
  {
    public:
      PatchLayout();
      PU_SMART_CTOR(PatchLayout)

      void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos);
      void Thread();
      std::string ParseNextLink(std::string link);
      bool ParsePatchData(std::vector<char> data, std::tuple<u32, u32, u32> atmosphereVersion, std::tuple<u32, u32, u32> firmwareVersion);
      void ProgressUpdate(double progress);
      void UpdateLoadingText(std::string text);

    private:
      pu::ui::elm::ProgressBar::Ref progressBar;
      pu::ui::elm::TextBlock::Ref loadingText;
    
    private:
      bool isThreadDone = false;
      std::string loaderUrl = "";
      std::string patchesUrl = "";

  };
}

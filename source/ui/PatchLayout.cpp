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
#include <ui/PatchLayout.hpp>
#include <util/Version.hpp>

extern ui::MainApplication::Ref global_app;

namespace ui
{
  PatchLayout::PatchLayout() : pu::ui::Layout()
  {
    this->SetBackgroundColor(pu::ui::Color::FromHex("#212121FF"));

    this->titleRect = pu::ui::elm::Rectangle::New(0, 0, 1280, 84, pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->titleRect);

    this->titleText = pu::ui::elm::TextBlock::New(24, 0, "Sigpatch Updater", 30);
    this->titleText->SetY((84 - this->titleText->GetTextHeight()) / 2);
    this->Add(this->titleText);

    this->loadingText = pu::ui::elm::TextBlock::New(0, 0, "Checking for signature patches...", 24);
    this->loadingText->SetColor(pu::ui::Color::FromHex("#FAFAFAFF"));
    this->loadingText->SetX((1280 - this->loadingText->GetTextWidth()) / 2);
    this->loadingText->SetY((606 - this->loadingText->GetTextHeight()) / 2 + 114);
    this->Add(this->loadingText);

    this->progressBar = pu::ui::elm::ProgressBar::New(320, (606 - this->loadingText->GetTextHeight()) / 2 + 84, 640, 6, 100);
    this->progressBar->SetProgressColor(pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->progressBar);

    this->AddThread(std::bind(&PatchLayout::Thread, this));
    this->SetOnInput(std::bind(&PatchLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  }

  // TEMP: So you can exit the application.
  void PatchLayout::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos)
  {
    if(Down & KEY_B)
    {
      global_app->Close();
    }
  }

  void PatchLayout::Thread()
  {
    if(!this->checkingForPatches)
    {
      this->checkingForPatches = true;
      auto atmosphereVersion = util::Version::GetAtmosphereVersion();
      auto firmwareVersion = util::Version::GetFirmwareVersion();

      // TODO: Loop through all releases to find an appropriate patch for the system.
    }
    else if(this->checkedForPatches)
    {
      // TODO: Install patches and go to HekateLayout.
    }
  }

  void PatchLayout::ProgressUpdate(double progress)
  {
    this->progressBar->SetProgress(progress * 100);
    global_app->CallForRender();
  }
}

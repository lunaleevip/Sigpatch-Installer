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

#include <jansson.h>
#include <ui/MainApplication.hpp>
#include <ui/MainLayout.hpp>
#include <util/File.hpp>
#include <util/Web.hpp>
#include <vector>

extern ui::MainApplication::Ref global_app;

namespace ui
{
  MainLayout::MainLayout() : pu::ui::Layout()
  {
    this->SetBackgroundColor(pu::ui::Color::FromHex("#212121FF"));

    this->titleRect = pu::ui::elm::Rectangle::New(0, 0, 1280, 84, pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->titleRect);

    this->titleText = pu::ui::elm::TextBlock::New(24, 0, "Sigpatch Updater", 30);
    this->titleText->SetY((84 - this->titleText->GetTextHeight()) / 2);
    this->Add(this->titleText);

    this->updateText = pu::ui::elm::TextBlock::New(0, 0, "Checking for updates...", 24);
    this->updateText->SetColor(pu::ui::Color::FromHex("#FAFAFAFF"));
    this->updateText->SetX((1280 - this->updateText->GetTextWidth()) / 2);
    this->updateText->SetY((606 - this->updateText->GetTextHeight()) / 2 + 114);
    this->Add(this->updateText);

    this->updateProgressBar = pu::ui::elm::ProgressBar::New(320, (606 - this->updateText->GetTextHeight()) / 2 + 84, 640, 6, 1);
    this->updateProgressBar->SetProgressColor(pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->updateProgressBar);

    this->AddThread(std::bind(&MainLayout::MainThread, this));
  }

  void MainLayout::MainThread()
  {
    if(!this->hekateChecked && !util::File::Exists("sdmc:/bootloader/hekate_ipl.ini"))
    {
      this->hekateChecked = true;
      global_app->CreateShowDialog("Hekate Required", "Make sure you have Hekate installed.", { "Ok" }, true);
      global_app->Close();
      return;
    }
    else if(!this->hekateChecked)
    {
      this->hekateChecked = true;
      this->internetChecked = true;
      if(!util::Web::IsConnected())
      {
        global_app->CreateShowDialog("Internet Required", "Make sure you are connected to the Internet and use a service like 90DNS to block traffic to Nintendo.", { "Ok" }, true);
        global_app->Close();
        return;
      }
      else
      {
        this->hasInternet = true;
        return;
      }
    }
    else if(this->hasInternet && !this->checkingForUpdate && !this->checkedForUpdate)
    {
      this->checkingForUpdate = true;

      auto data = util::Web::Get(
        "https://api.github.com/repos/HarukoNX/sigpatch-updater/releases",
        std::bind(&MainLayout::ProgressUpdate, this, std::placeholders::_1));
      if(data.empty())
      {
        this->checkingForUpdate = false;
        this->checkedForUpdate = true;
        return;
      }
      else
      {
        std::string json(data.begin(), data.end());
        auto root = json_loads(json.c_str(), 0, NULL);
        if(!root || !json_is_array(root) || json_array_size(root) < 1)
        {
          if(root)
          {
            json_decref(root);
          }
          this->checkingForUpdate = false;
          this->checkedForUpdate = true;
          return;
        }

        auto release = json_array_get(root, 0);
        if(!release || !json_is_object(release))
        {
          json_decref(root);
          this->checkingForUpdate = false;
          this->checkedForUpdate = true;
          return;
        }

        auto tagName = json_object_get(release, "tag_name");
        if(!tagName || !json_is_string(tagName))
        {
          json_decref(root);
          this->checkingForUpdate = false;
          this->checkedForUpdate = true;
          return;
        }

        std::string latest(json_string_value(tagName));

        // TODO
        json_decref(root);
        this->checkingForUpdate = false;
        this->checkedForUpdate = true;
        return;
      }
    }
    else if(this->checkedForUpdate && !this->checkingForPatches && !this->checkedForPatches)
    {
      this->checkingForPatches = true;

      this->updateText->SetText("Checking for available signature patches...");
      this->updateText->SetX((1280 - this->updateText->GetTextWidth()) / 2);
      this->updateProgressBar->SetProgress(0);

      auto data = util::Web::Get(
        "https://api.github.com/repos/HarukoNX/Atmosphere/releases",
        std::bind(&MainLayout::ProgressUpdate, this, std::placeholders::_1));
    }
  }

  void MainLayout::ProgressUpdate(double progress) {
    this->updateProgressBar->SetProgress(progress);
  }
}

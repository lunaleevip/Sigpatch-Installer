// Sigpatch Installer
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
#include <ui/UpdateLayout.hpp>
#include <util/File.hpp>
#include <util/Version.hpp>
#include <util/Web.hpp>
#include <vector>

extern ui::MainApplication::Ref global_app;

namespace ui
{
  UpdateLayout::UpdateLayout() : pu::ui::Layout()
  {
    this->SetBackgroundColor(pu::ui::Color::FromHex("#212121FF"));

    this->loadingText = pu::ui::elm::TextBlock::New(0, 0, "", 24);
    this->loadingText->SetColor(pu::ui::Color::FromHex("#FAFAFAFF"));
    this->Add(this->loadingText);

    this->progressBar = pu::ui::elm::ProgressBar::New(320, 0, 640, 6, 100);
    this->progressBar->SetProgressColor(pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->progressBar);
    
    this->UpdateLoadingText("Checking for updates...");

    this->AddThread(std::bind(&UpdateLayout::Thread, this));
  }

  void UpdateLayout::Thread()
  {
    if(this->isThreadDone)
    {
      return;
    }
    this->isThreadDone = true;

    if(!util::File::Exists("sdmc:/bootloader/hekate_ipl.ini"))
    {
      global_app->CreateShowDialog("Hekate Required", "Make sure you have Hekate installed.", { "Ok" }, true);
      global_app->Close();
      return;
    }

    if(!util::Web::IsConnected())
    {
      global_app->CreateShowDialog("Internet Required", "Make sure you are connected to the Internet and use a service like 90DNS to block traffic to Nintendo.", { "Ok" }, true);
      global_app->Close();
      return;
    }

    auto response = util::Web::Get(
      "https://api.github.com/repos/HarukoNX/Sigpatch-Installer/releases",
      std::bind(&UpdateLayout::ProgressUpdate, this, std::placeholders::_1));

    if(this->ParseAppUpdateData(response.RawBody))
    {
      this->UpdateLoadingText("Downloading update...");

      auto response = util::Web::Get(this->updateUrl, std::bind(&UpdateLayout::ProgressUpdate, this, std::placeholders::_1));
      util::File::Write(this->appPath, response.RawBody);
      envSetNextLoad(this->appPath.c_str(), this->appPath.c_str());
      global_app->Close();
    }
    else
    {
      global_app->ShowPatch();
    }
  }

  void UpdateLayout::ProgressUpdate(double progress)
  {
    this->progressBar->SetProgress(progress * 100);
    global_app->CallForRender();
  }

  bool UpdateLayout::ParseAppUpdateData(std::vector<char> data)
  {
    if(data.empty())
    {
      return false;
    }
    
    std::string json(data.begin(), data.end());
    auto root = json_loads(json.c_str(), 0, NULL);
    if(!root || !json_is_array(root) || json_array_size(root) < 1)
    {
      if(root)
      {
        json_decref(root);
      }
      return false;
    }

    auto release = json_array_get(root, 0);
    if(!release || !json_is_object(release))
    {
      json_decref(root);
      return false;
    }

    auto tagName = json_object_get(release, "tag_name");
    if(!tagName || !json_is_string(tagName))
    {
      json_decref(root);
      return false;
    }

    auto latest = util::Version::ParseVersion(std::string(json_string_value(tagName)));
    if(!util::Version::IsNewer(latest, util::Version::GetAppVersion()))
    {
      json_decref(root);
      return false;
    }

    auto assets = json_object_get(release, "assets");
    if(!assets || !json_is_array(assets) || json_array_size(assets) < 1)
    {
      json_decref(root);
      return false;
    }

    for(size_t i = 0; i < json_array_size(assets); i++)
    {
      auto asset = json_array_get(assets, i);
      if(!asset || !json_is_object(asset))
      {
        continue;
      }

      auto name = json_object_get(asset, "name");
      if(!name || !json_is_string(name))
      {
        continue;
      }

      std::string assetName(json_string_value(name));
      if(assetName.compare(assetName.length() - 4, 4, ".nro") != 0)
      {
        continue;
      }

      auto browserDownloadUrl = json_object_get(asset, "browser_download_url");
      if(!browserDownloadUrl || !json_is_string(browserDownloadUrl))
      {
        continue;
      }

      this->updateUrl = std::string(json_string_value(browserDownloadUrl));
      json_decref(root);
      return true;
    }

    json_decref(root);
    return false;
  }

  void UpdateLayout::SetAppPath(std::string appPath)
  {
    this->appPath = appPath;
  }

  void UpdateLayout::UpdateLoadingText(std::string text)
  {
    this->loadingText->SetText(text);
    this->loadingText->SetX((1280 - this->loadingText->GetTextWidth()) / 2);
    this->loadingText->SetY((606 - this->loadingText->GetTextHeight()) / 2 + 114);
    this->progressBar->SetY((606 - this->loadingText->GetTextHeight()) / 2 + 84);
    this->progressBar->SetProgress(0);
  }
}

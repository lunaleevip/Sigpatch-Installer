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
#include <ui/PatchLayout.hpp>
#include <util/File.hpp>
#include <util/Version.hpp>
#include <util/Web.hpp>
#include <sstream>

extern ui::MainApplication::Ref global_app;

namespace ui
{
  PatchLayout::PatchLayout() : pu::ui::Layout()
  {
    this->SetBackgroundColor(pu::ui::Color::FromHex("#212121FF"));

    this->loadingText = pu::ui::elm::TextBlock::New(0, 0, "", 24);
    this->loadingText->SetColor(pu::ui::Color::FromHex("#FAFAFAFF"));
    this->Add(this->loadingText);

    this->progressBar = pu::ui::elm::ProgressBar::New(320, 0, 640, 6, 100);
    this->progressBar->SetProgressColor(pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->progressBar);

    this->UpdateLoadingText("Checking for signature patches...");

    this->AddThread(std::bind(&PatchLayout::Thread, this));
  }

  void PatchLayout::Thread()
  {
    if(this->isThreadDone)
    {
      return;
    }
    this->isThreadDone = true;

    auto atmosphereVersion = util::Version::GetAtmosphereVersion();
    auto firmwareVersion = util::Version::GetFirmwareVersion();
    std::string url = "https://api.github.com/repos/HarukoNX/Atmosphere/releases";

    while(!url.empty())
    {
      auto response = util::Web::Get(
        url,
        std::bind(&PatchLayout::ProgressUpdate, this, std::placeholders::_1));

      if(this->ParsePatchData(response.RawBody, atmosphereVersion, firmwareVersion))
      {
        break;
      }

      if(response.Headers.count("link") == 0)
      {
        break;
      }

      url = ParseNextLink(response.Headers["link"]);

      this->progressBar->SetProgress(0);
      global_app->CallForRender();
    }

    if(this->patchesUrl.empty() && this->loaderUrl.empty())
    {
      global_app->CreateShowDialog("No Patches Available", "No patches could be found for the version of Atmosphere and/or firmware version that you are running.\nIf a new version of Atmosphere or firmware was just released, then please give me some time to\nrelease the patches. Check the GBATemp thread and try again at a later date.\n", { "Ok" }, true);
      global_app->Close();
      return;
    }

    this->UpdateLoadingText("Downloading FS patches...");
    this->ProgressUpdate(0);
    
    auto response = util::Web::Get(this->patchesUrl, std::bind(&PatchLayout::ProgressUpdate, this, std::placeholders::_1));
    util::File::Write("sdmc:/bootloader/patches.ini", response.RawBody);

    if(std::get<0>(firmwareVersion) >= 10)
    {
      this->UpdateLoadingText("Downloading ACID patches...");
      this->ProgressUpdate(0);

      if(!util::File::Exists("sdmc:/atmosphere/kips"))
      {
        util::File::MakeFolder("sdmc:/atmosphere/kips");
      }
      
      auto response = util::Web::Get(this->loaderUrl, std::bind(&PatchLayout::ProgressUpdate, this, std::placeholders::_1));
      util::File::Write("sdmc:/atmosphere/kips/loader.kip", response.RawBody);
    }

    global_app->ShowHekate();
  }

  std::string PatchLayout::ParseNextLink(std::string links)
  {
    std::istringstream ss(links);
    std::string link;
    while(std::getline(ss, link, ','))
    {
      if(link.size() == 0)
      {
        continue;
      }

      auto pos = link.find(";");
      if(pos == std::string::npos)
      {
        continue;
      }

      auto relationship = link.substr(pos + 7, link.size() - pos - 8);
      if(relationship == "next")
      {
        return link.substr(1, pos - 2);
      }
    }

    return "";
  }

  bool PatchLayout::ParsePatchData(std::vector<char> data, std::tuple<u32, u32, u32> atmosphereVersion, std::tuple<u32, u32, u32> firmwareVersion)
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

    // Firmwares below 10.0.0 don't require patching of loader service.
    if(std::get<0>(firmwareVersion) < 10)
    {
      auto release = json_array_get(root, 0);
      if(!release || !json_is_object(release))
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
        if(assetName != "patches.ini")
        {
          continue;
        }

        auto browserDownloadUrl = json_object_get(asset, "browser_download_url");
        if(!browserDownloadUrl || !json_is_string(browserDownloadUrl))
        {
          continue;
        }

        this->patchesUrl = std::string(json_string_value(browserDownloadUrl));
        json_decref(root);
        return true;
      }

      this->patchesUrl = "";
      this->loaderUrl = "";
      return false;
    }

    for(size_t x = 0; x < json_array_size(root); x++)
    {
      auto release = json_array_get(root, x);
      if(!release || !json_is_object(release))
      {
        continue;
      }

      auto body = json_object_get(release, "body");
      if(!body || !json_is_string(body))
      {
        continue;
      }

      std::string releaseBody(json_string_value(body));
      auto firmwareInfo = releaseBody.substr(releaseBody.rfind('\n') + 1);
      auto pos = firmwareInfo.find(":");
      if(pos == std::string::npos)
      {
        continue;
      }

      auto compatibleAtmosphereVersion = util::Version::ParseVersion(firmwareInfo.substr(0, pos));
      auto maxFirmwareVersion = util::Version::ParseVersion(firmwareInfo.substr(pos + 1, firmwareInfo.size() - pos - 1));

      if(
        util::Version::IsNewer(atmosphereVersion, compatibleAtmosphereVersion)
        || util::Version::IsNewer(firmwareVersion, maxFirmwareVersion))
      {
        this->patchesUrl = "";
        this->loaderUrl = "";
        return true;
      }
      
      if (
        util::Version::IsEqual(atmosphereVersion, compatibleAtmosphereVersion)
        && (
          util::Version::IsEqual(firmwareVersion, maxFirmwareVersion)
          || util::Version::IsNewer(maxFirmwareVersion, firmwareVersion)))
      {
        auto assets = json_object_get(release, "assets");
        if(!assets || !json_is_array(assets) || json_array_size(assets) < 1)
        {
          json_decref(root);
          return false;
        }

        for(size_t y = 0; y < json_array_size(assets); y++)
        {
          auto asset = json_array_get(assets, y);
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
          if(assetName != "patches.ini" && assetName != "loader.kip")
          {
            continue;
          }

          auto browserDownloadUrl = json_object_get(asset, "browser_download_url");
          if(!browserDownloadUrl || !json_is_string(browserDownloadUrl))
          {
            continue;
          }

          if(assetName != "patches.ini")
          {
            this->patchesUrl = std::string(json_string_value(browserDownloadUrl));
          }
          else
          {
            this->loaderUrl = std::string(json_string_value(browserDownloadUrl));
          }
        }

        json_decref(root);
        return true;
      }
    }

    return false; 
  }

  void PatchLayout::ProgressUpdate(double progress)
  {
    this->progressBar->SetProgress(progress * 100);
    global_app->CallForRender();
  }

  void PatchLayout::UpdateLoadingText(std::string text)
  {
    this->loadingText->SetText(text);
    this->loadingText->SetX((1280 - this->loadingText->GetTextWidth()) / 2);
    this->loadingText->SetY((606 - this->loadingText->GetTextHeight()) / 2 + 114);
    this->progressBar->SetY((606 - this->loadingText->GetTextHeight()) / 2 + 84);
    this->progressBar->SetProgress(0);
  }
}

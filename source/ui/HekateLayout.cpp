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

#include <ui/MainApplication.hpp>
#include <ui/HekateLayout.hpp>
#include <util/File.hpp>
#include <util/Reboot.hpp>
#include <util/Version.hpp>

extern ui::MainApplication::Ref global_app;

namespace ui
{
  HekateLayout::~HekateLayout()
  {
    if(this->config != nullptr)
    {
      delete this->config;
    }
  }

  HekateLayout::HekateLayout() : pu::ui::Layout()
  {
    this->GetHekateConfig();

    this->SetBackgroundColor(pu::ui::Color::FromHex("#212121FF"));

    this->instructionsRect = pu::ui::elm::Rectangle::New(0, 84, 1280, 84, pu::ui::Color::FromHex("#F44336FF"));
    this->Add(this->instructionsRect);

    this->instructionsTitle = pu::ui::elm::TextBlock::New(
      24,
      102,
      "Setting up Hekate",
      24);
    this->instructionsTitle->SetColor(pu::ui::Color::FromHex("#FFFFFFFF"));
    this->Add(this->instructionsTitle);

    this->instructionsSubtitle = pu::ui::elm::TextBlock::New(
      24,
      102 + this->instructionsTitle->GetTextHeight(),
      "Select which boot options you would like to enable signature patches, and then press + to save.",
      18);
    this->instructionsSubtitle->SetColor(pu::ui::Color::FromHex("#FFFFFFFF"));
    this->Add(this->instructionsSubtitle);

    this->menu = pu::ui::elm::Menu::New(0, 168, 1280, pu::ui::Color::FromHex("#212121FF"), 92, 6);
    this->menu->SetOnFocusColor(pu::ui::Color::FromHex("#ff7961FF"));
    this->Add(this->menu);

    for(auto const& section : this->config->sections)
    {
      if(
        section->type != simpleIniParser::IniSectionType::Section
        || section->findFirstOption("stock") != nullptr
        || section->value == "config")
      {
        continue;
      }

      auto menuItem = pu::ui::elm::MenuItem::New(section->value);
      menuItem->SetIcon("romfs:/Off.png");
      menuItem->SetColor(pu::ui::Color::FromHex("#FAFAFAFF"));
      menuItem->AddOnClick(std::bind(&HekateLayout::OnItemClicked, this));
      this->itemsSelected.push_back(false);
      this->menu->AddItem(menuItem);
    }


    this->SetOnInput(std::bind(&HekateLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  }

  void HekateLayout::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos)
  {
    if(Down & KEY_PLUS)
    {
      if(std::find(this->itemsSelected.begin(), this->itemsSelected.end(), true) == this->itemsSelected.end())
      {
        global_app->CreateShowDialog("One Boot Option Required", "Please select at least one boot option to enable the signature patches.", { "Ok" }, true);
        return;
      }

      int i = 0;
      auto firmwareVersion = util::Version::GetFirmwareVersion();
      for(auto const& section : this->config->sections)
      {
        if(
          section->type != simpleIniParser::IniSectionType::Section
          || section->findFirstOption("stock") != nullptr
          || section->value == "config")
        {
          continue;
        }

        if(!this->itemsSelected[i])
        {
          i++;
          continue;
        }

        section->options.push_back(new simpleIniParser::IniOption(simpleIniParser::IniOptionType::Option, "kip1patch", "nosigchk"));
        if(std::get<0>(firmwareVersion) >= 10)
        {
          section->options.push_back(new simpleIniParser::IniOption(simpleIniParser::IniOptionType::Option, "kip1", "atmosphere/kips/*"));
        }
      }

      this->config->writeToFile("sdmc:/bootloader/hekate_ipl.ini");

      if(!util::Reboot::RebootToPayload("sdmc:/bootloader/update.bin"))
      {
        global_app->UnlockHomeButton();
        global_app->Close();
      }
    }
  }

  void HekateLayout::OnItemClicked()
  {
    auto index = this->menu->GetSelectedIndex();
    auto menuItem = this->menu->GetItems()[index];

    menuItem->SetIcon((this->itemsSelected[index]) ? "romfs:/Off.png" : "romfs:/On.png");
    this->itemsSelected[index] = !this->itemsSelected[index];
    this->menu->ReloadItemRenders();
  }

  void HekateLayout::GetHekateConfig()
  {
    auto currentConfig = simpleIniParser::Ini::parseFile("sdmc:/bootloader/hekate_ipl.ini");
    this->config = new simpleIniParser::Ini();
    this->config->magic = currentConfig->magic;

    for(auto const& option : currentConfig->options)
    {
      this->config->options.push_back(
        new simpleIniParser::IniOption(
          option->type,
          option->key,
          option->value
        )
      );
    }

    for(auto const& section : currentConfig->sections)
    {
      auto newSection = new simpleIniParser::IniSection(section->type, section->value);
      
      for(auto const& option : section->options)
      {
        if(option->key == "kip1patch" || option->key == "kip1")
        {
          continue;
        }

        newSection->options.push_back(
          new simpleIniParser::IniOption(
            option->type,
            option->key,
            option->value
          )
        );
      }

      this->config->sections.push_back(newSection);
    }

    delete currentConfig;
  }

}

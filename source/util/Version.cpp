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

#include <util/Version.hpp>

namespace util
{
  std::tuple<u32, u32, u32> Version::GetAppVersion()
  {
    return std::make_tuple(
      APP_VERSION_MAJOR,
      APP_VERSION_MINOR,
      APP_VERSION_PATCH);
  }

  std::tuple<u32, u32, u32> Version::GetFirmwareVersion()
  {
    SetSysFirmwareVersion fw;

    Result rc = setsysInitialize();
    if(R_SUCCEEDED(rc))
    {
      setsysGetFirmwareVersion(&fw);
      setsysExit();
    }

    return std::make_tuple(
      fw.major,
      fw.minor,
      fw.micro);
  }

  std::tuple<u32, u32, u32> Version::GetAtmosphereVersion()
  {
    u64 exosphere_cfg = 0;

    Result rc = splInitialize();
    if(R_SUCCEEDED(rc))
    {
      splGetConfig((SplConfigItem) 65000, &exosphere_cfg);
      splExit();
    }

    return std::make_tuple(
      (u32)((exosphere_cfg >> 0x20) & 0xFF),
      (u32)((exosphere_cfg >> 0x18) & 0xFF),
      (u32)((exosphere_cfg >> 0x10) & 0xFF));
  }

  std::tuple<u32, u32, u32> Version::ParseVersion(std::string version)
  {
    u32 major = 0;
    u32 minor = 0;
    u32 patch = 0;
    u8 section = 0;

    std::string number = "";
    for(char& c : version)
    {
      if(c >= '0' && c <= '9')
      {
          number += c;
      }
      else if(c == '.')
      {
        if(section == 0)
        {
          major = std::stoi(number);
        }
        else if(section == 1)
        {
          minor = std::stoi(number);
        }
        else if(section == 2)
        {
          patch = std::stoi(number);
          break;
        }

        number = "";
        section++;
      }
      else
      {
        number = "";
      }
    }

    if(section == 2 && !number.empty())
    {
      patch = std::stoi(number);
    }

    return std::make_tuple(
      major,
      minor,
      patch);
  }

  bool Version::IsNewer(std::tuple<u32, u32, u32> a, std::tuple<u32, u32, u32> b)
  {
    if(std::get<0>(a) != std::get<0>(b))
    {
      return std::get<0>(a) > std::get<0>(b);
    }

    if(std::get<1>(a) != std::get<1>(b))
    {
      return std::get<1>(a) > std::get<1>(b);
    }

    return std::get<2>(a) > std::get<2>(b);
  }

  bool Version::IsEqual(std::tuple<u32, u32, u32> a, std::tuple<u32, u32, u32> b)
  {
    return (
      std::get<0>(a) == std::get<0>(b) &&
      std::get<1>(a) == std::get<1>(b) &&
      std::get<2>(a) == std::get<2>(b)
    );
  }
}

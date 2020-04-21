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
  std::tuple<u8, u8, u8> Version::GetAppVersion()
  {
    return std::make_tuple(
      APP_VERSION_MAJOR,
      APP_VERSION_MINOR,
      APP_VERSION_PATCH);
  }

  std::tuple<u8, u8, u8> Version::GetFirmwareVersion()
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
}

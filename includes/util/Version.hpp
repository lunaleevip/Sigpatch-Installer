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

#include <switch.h>
#include <tuple>

namespace util
{
  class Version
  {
    public:
      static std::tuple<u8, u8, u8> GetAppVersion();
      static std::tuple<u8, u8, u8> GetFirmwareVersion();
      static std::tuple<u32, u32, u32> GetAtmosphereVersion();

  };
}

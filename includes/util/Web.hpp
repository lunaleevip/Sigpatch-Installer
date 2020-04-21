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

#include <curl/curl.h>
#include <functional>
#include <string>
#include <vector>

namespace util
{
  class Web
  {
    public:
      static bool IsConnected();
      static std::vector<char> Get(std::string url, std::function<void(double)> progress);
      static size_t Write(const char *in, size_t size, size_t num, std::vector<char> *buffer);
      static size_t TransferInfo(std::function<void(double)> *progress, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
  };
}

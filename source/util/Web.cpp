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

#include <util/Web.hpp>
#include <switch.h>

namespace util
{
  bool Web::IsConnected()
  {
    Result rc = nifmInitialize(NifmServiceType_User);
    if(R_FAILED(rc))
    {
      return false;
    }

    NifmInternetConnectionType type;
    u32 str = 0;
    NifmInternetConnectionStatus status;

    rc = nifmGetInternetConnectionStatus(&type, &str, &status);
    nifmExit();
    if(R_FAILED(rc))
    {
      return false;
    }

    return (
      (type == NifmInternetConnectionType_Ethernet && status == NifmInternetConnectionStatus_Connected)
      || (type == NifmInternetConnectionType_WiFi && str > 0)
    );
  }

  std::vector<char> Web::Get(std::string url, std::function<void(double)> progress)
  {
    std::vector<char> buffer;

    CURL * c = curl_easy_init();
    if(!c)
    {
      buffer.clear();
      return buffer;
    }

    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    curl_easy_setopt(c, CURLOPT_USERAGENT, "Sigpatch-Updater");
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, Write);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, (void *) &buffer);
    curl_easy_setopt(c, CURLOPT_XFERINFOFUNCTION, TransferInfo);
    curl_easy_setopt(c, CURLOPT_XFERINFODATA, (void *) &progress);

    CURLcode res = curl_easy_perform(c);
    curl_easy_cleanup(c);

    if(res != CURLE_OK)
    {
      buffer.clear();
    }

    return buffer;
  }

  size_t Web::Write(const char *in, size_t size, size_t num, std::vector<char> *buffer)
  {
    size_t i = 0;
    while (i < size * num) {
      buffer->push_back(*in);
      ++in;
      i++;
    }

    return i;
  }

  size_t Web::TransferInfo(std::function<void(double)> *progress, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    auto completed = (double) dlnow / (double) dltotal;
    (*progress)(completed);
    return 0;
  }
}

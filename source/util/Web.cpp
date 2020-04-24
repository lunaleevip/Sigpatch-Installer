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

  data::Response Web::Get(std::string url, std::function<void(double)> progress)
  {
    data::Response response;

    CURL * c = curl_easy_init();
    if(!c)
    {
      response.Clear();
      return response;
    }

    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, Header);
    curl_easy_setopt(c, CURLOPT_HEADERDATA, (void *) &response.RawHeader);
    curl_easy_setopt(c, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    curl_easy_setopt(c, CURLOPT_USERAGENT, "Sigpatch-Installer");
    curl_easy_setopt(c, CURLOPT_WRITEDATA, (void *) &response.RawBody);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, Write);
    curl_easy_setopt(c, CURLOPT_XFERINFODATA, (void *) &progress);
    curl_easy_setopt(c, CURLOPT_XFERINFOFUNCTION, TransferInfo);

    CURLcode res = curl_easy_perform(c);

    if(res != CURLE_OK)
    {
      response.Clear();
      return response;
    }

    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &response.StatusCode);
    curl_easy_cleanup(c);

    response.ParseHeaders();

    return response;
  }

  size_t Web::Header(const char *in, size_t size, size_t num, std::vector<char> *buffer)
  {
    size_t i = 0;
    while(i < size * num)
    {
      buffer->push_back(*in);
      ++in;
      i++;
    }

    return i;
  }

  size_t Web::TransferInfo(std::function<void(double)> *progress, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
  {
    auto completed = (double) dlnow / (double) dltotal;
    (*progress)(completed);
    return 0;
  }

  size_t Web::Write(const char *in, size_t size, size_t num, std::vector<char> *buffer)
  {
    size_t i = 0;
    while(i < size * num)
    {
      buffer->push_back(*in);
      ++in;
      i++;
    }

    return i;
  }
}

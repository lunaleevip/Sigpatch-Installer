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

#include <data/Response.hpp>
#include <sstream>

namespace data
{
  void Response::Clear()
  {
    this->Headers.clear();
    this->RawBody.clear();
    this->RawHeader.clear();
    this->StatusCode = 0;
  }

  void Response::ParseHeaders()
  {
    if(this->RawHeader.size() == 0)
    {
      return;
    }

    std::istringstream ss(std::string(this->RawHeader.begin(), this->RawHeader.end()));
    std::string header;
    while(std::getline(ss, header))
    {
      if(header.size() == 0)
      {
        continue;
      }

      auto pos = header.find(":");
      if(pos == std::string::npos)
      {
        continue;
      }

      this->Headers.insert(
        std::pair<std::string, std::string>(
          header.substr(0, pos),
          header.substr(pos + 2, header.size() - pos + 2)
        )
      );
    }
  }
}

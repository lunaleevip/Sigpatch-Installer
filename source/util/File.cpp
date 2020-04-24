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

#include <fstream>
#include <sys/stat.h>
#include <util/File.hpp>

namespace util
{
  bool File::Exists(std::string path)
  {
    struct stat buf;
    return (stat(path.c_str(), &buf) == 0);
  }


  void File::MakeFolder(std::string path)
  {
    mkdir(path.c_str(), 0775);
  }

  void File::Write(std::string path, std::vector<char> data)
  {
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if(!file.is_open())
    {
      return;
    }

    file.write((char *)&data[0], data.size());

    file.flush();
    file.close();
  }
}

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

#include <cstring>
#include <util/File.hpp>
#include <util/Reboot.hpp>
#include <switch.h>

#define IRAM_PAYLOAD_MAX_SIZE 0x2F000
#define IRAM_PAYLOAD_BASE 0x40010000

static __attribute__((aligned(0x1000))) u8 g_ff_page[0x1000];
static __attribute__((aligned(0x1000))) u8 g_work_page[0x1000];

namespace util
{
  bool Reboot::RebootToPayload(std::string path)
  {
    auto payload = util::File::Read("sdmc:/bootloader/update.bin");
    if(payload.size() == 0)
    {
      return false;
    }

    Result rc = splInitialize();
    if(R_FAILED(rc))
    {
      return false;
    }

    Reboot::ClearIram();

    for(size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += 0x1000)
    {
      Reboot::CopyToIram(IRAM_PAYLOAD_BASE + i, &payload[i], 0x1000);
    }

    splSetConfig((SplConfigItem)65001, 2);
    splExit();

    return true;
  }

  void Reboot::CopyToIram(uintptr_t iram_addr, void *buf, size_t size)
  {
    memcpy(g_work_page, buf, size);
    
    SecmonArgs args = {0};
    args.X[0] = 0xF0000201;             /* smcAmsIramCopy */
    args.X[1] = (uintptr_t)g_work_page; /* DRAM Address */
    args.X[2] = iram_addr;              /* IRAM Address */
    args.X[3] = size;                   /* Copy size */
    args.X[4] = 1;
    svcCallSecureMonitor(&args);
    
    memcpy(buf, g_work_page, size);
  }

  void Reboot::ClearIram()
  {
    memset(g_ff_page, 0xFF, sizeof(g_ff_page));
    for(size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += sizeof(g_ff_page))
    {
      Reboot::CopyToIram(IRAM_PAYLOAD_BASE + i, g_ff_page, sizeof(g_ff_page));
    }
  }
}

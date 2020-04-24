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

extern ui::MainApplication::Ref global_app;

namespace ui
{
  HekateLayout::HekateLayout() : pu::ui::Layout()
  {
    this->SetBackgroundColor(pu::ui::Color::FromHex("#212121FF"));
    this->SetOnInput(std::bind(&HekateLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  }

  void HekateLayout::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos)
  {
    if(Down & KEY_PLUS)
    {
      global_app->Close();
    }
  }
}

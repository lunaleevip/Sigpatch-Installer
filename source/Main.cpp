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

#include <curl/curl.h>
#include <ui/MainApplication.hpp>

ui::MainApplication::Ref global_app;

extern "C" void userAppInit()
{
  socketInitializeDefault();
  curl_global_init(CURL_GLOBAL_ALL);
  nxlinkStdio();
}

extern "C" void userAppExit()
{
  socketExit();
}

int main()
{
  auto renderer = pu::ui::render::Renderer::New(
    SDL_INIT_EVERYTHING,
    pu::ui::render::RendererInitOptions::RendererNoSound,
    pu::ui::render::RendererHardwareFlags);

  global_app = ui::MainApplication::New(renderer);
  global_app->Prepare();
  global_app->Show();

  return 0;
}
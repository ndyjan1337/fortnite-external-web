#include <Windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <d3d9.h>
#include <d3d11.h>
#include <dwmapi.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "render/framework/imgui.h"
#include "render/framework/imgui_impl_win32.h"
#include "render/framework/imgui_impl_dx11.h"

#include "other/fonts/burbank.hpp"

#include "other/globals.hpp"

#include "other/hexarray.hpp"

#include "other/skcrypt.hpp"

#include "request/request.hpp"

#include "fortnite/update.hpp"

#include "fortnite/ue4.hpp"

#include "other/mouse/mouse.hpp"

#include "fortnite/fortnite.hpp"

#include "render/render.hpp"
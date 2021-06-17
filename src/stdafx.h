#pragma once
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7
#define NOMINMAX

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <thread>
#include <vector>

namespace Gdiplus
{
	using std::min;
	using std::max;
};

#include "resource.h"
#include "../columns_ui-sdk/ui_extension.h"
#include <foobar2000/helpers/foobar2000+atl.h>
#include <foobar2000/helpers/atl-misc.h>
#include <libPPUI/gdiplus_helpers.h>
#include <json.hpp>

using FontNameArray = std::array<wchar_t, LF_FACESIZE>;
using ImageBuffer = std::vector<uint8_t>;
using PathArray = std::array<wchar_t, MAX_PATH>;
using Strings = std::vector<std::string>;
using WStrings = std::vector<std::wstring>;
using json = nlohmann::json;
using pfc::string8;

#include "foo_jscript_panel.h"
#include "guids.h"
#include "Misc.h"
#include "ThreadPool.h"
#include "Timer.h"
#include "ScriptInterface.h"
#include "CallbackID.h"
#include "CallbackData.h"
#include "Component.h"
#include "CustomSort.h"
#include "FileHelper.h"
#include "Image.h"
#include "ProcessLocationsNotify.h"

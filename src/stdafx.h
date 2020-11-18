#pragma once
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7
#define NOMINMAX

#include <algorithm>
#include <array>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
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

using pfc::string8;
using pfc::stringp;
using namespace pfc::stringcvt;
using Strings = std::vector<std::string>;
using json = nlohmann::json;

#include "foo_jscript_panel.h"
#include "guids.h"
#include "ThreadPool.h"
#include "ScriptInterface.h"
#include "CallbackID.h"
#include "CallbackData.h"
#include "Text.h"
#include "File.h"
#include "Image.h"
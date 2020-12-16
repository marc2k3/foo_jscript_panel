#pragma once

namespace jsp
{
	static constexpr const char* component_name = "JScript Panel";
	static constexpr const char* component_version = "2.4.3";
	static constexpr const char* component_dll_name = "foo_jscript_panel.dll";
	static constexpr const char* component_info = "Copyright (C) 2015-2020 marc2003\nBased on WSH Panel Mod by T.P. Wang\nThanks for the contributions by TheQwertiest and kbuffington\n\nBuild: " __TIME__ ", " __DATE__ "\nColumns UI SDK Version: " UI_EXTENSION_VERSION;
	static constexpr size_t uwm_refreshbk = WM_USER + 1;
	static constexpr size_t uwm_timer = WM_USER + 2;
	static constexpr size_t uwm_unload = WM_USER + 3;
	static constexpr size_t version = 2430;
}

#define CRLF "\r\n"
#define LF "\n"

#define GET_PTR(X, Y) \
	X->get__ptr(reinterpret_cast<void**>(&Y)); \
	if (!Y) return E_INVALIDARG;

#define FIND_IF(blah, func) std::find_if(std::begin(blah), std::end(blah), func)

template <typename T>
static std::vector<T> split_string_t(const T& text, const T& delims)
{
	std::vector<T> out;
	size_t pos = 0;
	while (true)
	{
		size_t old_pos = pos;
		pos = text.find(delims, old_pos);
		out.emplace_back(text.substr(old_pos, pos - old_pos));
		if (pos == T::npos) break;
		pos += delims.length();
	}
	return out;
}

static std::vector<std::string> split_string(const std::string& text, const std::string& delims) { return split_string_t<std::string>(text, delims); }
static std::vector<std::wstring> split_string(const std::wstring& text, const std::wstring& delims) { return split_string_t<std::wstring>(text, delims); }

template <typename T>
static bool ensure_gdiplus_object(const std::unique_ptr<T>& obj) { return obj && obj->GetLastStatus() == Gdiplus::Ok; }

template <typename T>
static uint32_t to_uint(T num) { return static_cast<uint32_t>(num); }

template <typename T>
static COLORREF to_colorref(T argb) { return Gdiplus::Color(to_uint(argb)).ToCOLORREF(); }

template <typename T>
static VARIANT_BOOL to_variant_bool(T b) { return b ? VARIANT_TRUE : VARIANT_FALSE; }

static string8 from_wide(const std::wstring& str)
{
	std::string ret;
	ret.resize(WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), nullptr, 0, nullptr, nullptr));
	WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), ret.data(), ret.size(), nullptr, nullptr);
	return ret.c_str();
}

static std::wstring to_wide(stringp str)
{
	std::wstring ret;
	ret.resize(MultiByteToWideChar(CP_UTF8, 0, str, str.length(), nullptr, 0));
	MultiByteToWideChar(CP_UTF8, 0, str, str.length(), ret.data(), ret.size());
	return ret;
}

static bool to_bool(VARIANT_BOOL vb) { return vb != VARIANT_FALSE; }
static BSTR to_bstr(stringp str) { return SysAllocString(to_wide(str).data()); }
static int to_int(size_t num) { return num == SIZE_MAX ? -1 : static_cast<int>(num); }

static int to_argb(COLORREF colour)
{
	Gdiplus::Color c;
	c.SetFromCOLORREF(colour);
	return static_cast<int>(Gdiplus::Color::MakeARGB(UINT8_MAX, c.GetRed(), c.GetGreen(), c.GetBlue()));
}

static HFONT create_font(const std::wstring& name, float pxSize, int style)
{
	return CreateFont(
		-static_cast<int>(pxSize),
		0,
		0,
		0,
		(style & Gdiplus::FontStyleBold) ? FW_BOLD : FW_NORMAL,
		(style & Gdiplus::FontStyleItalic) ? TRUE : FALSE,
		(style & Gdiplus::FontStyleUnderline) ? TRUE : FALSE,
		(style & Gdiplus::FontStyleStrikeout) ? TRUE : FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		name.data());
}

#pragma once
#define CRLF "\r\n"
#define LF "\n"

class jstring {
public:
	jstring(const char* s) : m_ptr(s) {}
	jstring(const pfc::string& s) : m_ptr(s.get_ptr()) {}
	jstring(const pfc::string_base& s) : m_ptr(s) {}
	jstring(const std::string& s) : m_ptr(s.c_str()) {}

	operator const char* () const { return m_ptr; }
	operator pfc::string8() const { return m_ptr; }
	operator std::string() const { return m_ptr; }

	const char* get_ptr() const { return m_ptr; }
	size_t length() const { return strlen(m_ptr); }

private:
	const char* const m_ptr;
};

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

static Strings split_string(const std::string& text, const std::string& delims) { return split_string_t<std::string>(text, delims); }
static WStrings split_string(const std::wstring& text, const std::wstring& delims) { return split_string_t<std::wstring>(text, delims); }

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
	ret.resize(WideCharToMultiByte(CP_UTF8, 0, str.data(), str.length(), nullptr, 0, nullptr, nullptr));
	WideCharToMultiByte(CP_UTF8, 0, str.data(), str.length(), ret.data(), ret.length(), nullptr, nullptr);
	return ret.c_str();
}

static std::wstring to_wide(jstring str)
{
	std::wstring ret;
	ret.resize(MultiByteToWideChar(CP_UTF8, 0, str, str.length(), nullptr, 0));
	MultiByteToWideChar(CP_UTF8, 0, str, str.length(), ret.data(), ret.length());
	return ret;
}

static bool to_bool(VARIANT_BOOL vb) { return vb != VARIANT_FALSE; }
static BSTR to_bstr(jstring str) { return SysAllocString(to_wide(str).data()); }
static int to_int(size_t num) { return num == SIZE_MAX ? -1 : static_cast<int>(num); }
static uint64_t hash_guid(const GUID& g) { return hasher_md5::get()->process_single_string(pfc::print_guid(g)).xorHalve(); }

static int to_argb(COLORREF colour)
{
	Gdiplus::Color c;
	c.SetFromCOLORREF(colour);
	return static_cast<int>(Gdiplus::Color::MakeARGB(UINT8_MAX, c.GetRed(), c.GetGreen(), c.GetBlue()));
}

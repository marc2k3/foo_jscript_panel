#pragma once

class EstimateLineWrap
{
public:
	struct WrappedItem
	{
		std::wstring text;
		uint32_t width = 0;
	};

	using WrappedItems = std::vector<WrappedItem>;

	EstimateLineWrap(HDC hdc, uint32_t width) : m_hdc(hdc), m_width(width) {}

	void wrap(const std::wstring& text, WrappedItems& out)
	{
		std::wstring delims = PFC_WIDESTRING(CRLF);
		if (text.find(delims) == std::wstring::npos) delims = PFC_WIDESTRING(LF);

		for (const std::wstring& str : split_string(text, delims))
		{
			wrap_recur(str, out);
		}
	}

private:
	bool is_wrap_char(wchar_t current, wchar_t next)
	{
		if (iswpunct(current)) return false;
		if (iswspace(current) || next == '\0') return true;

		const int current_alpha_num = iswalnum(current);
		if (current_alpha_num && iswpunct(next)) return false;
		return current_alpha_num == 0 || iswalnum(next) == 0;
	}

	uint32_t get_text_width(const std::wstring& text, uint32_t length)
	{
		SIZE size;
		GetTextExtentPoint32(m_hdc, text.data(), static_cast<int>(length), &size);
		return to_uint(size.cx);
	}

	void wrap_recur(const std::wstring& text, WrappedItems& out)
	{
		const uint32_t text_width = get_text_width(text, text.length());

		if (text_width <= m_width)
		{
			WrappedItem item = { text, text_width };
			out.emplace_back(item);
		}
		else
		{
			uint32_t text_length = text.length() * m_width / text_width;

			if (get_text_width(text, text_length) < m_width)
			{
				while (get_text_width(text, std::min(text.length(), text_length + 1)) <= m_width)
				{
					++text_length;
				}
			}
			else
			{
				while (get_text_width(text, text_length) > m_width && text_length > 1)
				{
					--text_length;
				}
			}

			const uint32_t fallback_length = std::max<uint32_t>(text_length, 1);

			while (text_length > 0 && !is_wrap_char(text.at(text_length - 1), text.at(text_length)))
			{
				--text_length;
			}

			if (text_length == 0) text_length = fallback_length;

			WrappedItem item = { text.substr(0, text_length), get_text_width(text, text_length) };
			out.emplace_back(item);

			if (text_length < text.length())
			{
				wrap_recur(text.substr(text_length), out);
			}
		}
	}

	HDC m_hdc;
	uint32_t m_width;
};

#pragma once

namespace FontHelpers
{
	static FontStrings get_fonts()
	{
		static FontStrings fonts;
		if (fonts.empty())
		{
			Gdiplus::InstalledFontCollection font_collection;
			const int count = font_collection.GetFamilyCount();
			std::vector<Gdiplus::FontFamily> families(count);

			int found = 0;
			if (font_collection.GetFamilies(count, families.data(), &found) == Gdiplus::Ok)
			{
				for (const auto& family : families)
				{
					FontString family_name;
					family.GetFamilyName(family_name.data());
					fonts.emplace_back(family_name);
				}
			}
		}
		return fonts;
	}

	static FontString get_name(const std::unique_ptr<Gdiplus::Font>& font)
	{
		FontString name;
		Gdiplus::FontFamily fontFamily;
		font->GetFamily(&fontFamily);
		fontFamily.GetFamilyName(name.data());
		return name;
	}

	static bool check_name(const std::wstring& name)
	{
		FontStrings fonts = get_fonts();

		const auto& it = std::ranges::find_if(fonts, [name](const FontString& font)
			{
				return _wcsicmp(font.data(), name.data()) == 0;
			});
		return it != fonts.end();
	}

	static HFONT create(const std::wstring& name, float pxSize, int style)
	{
		std::wstring name_checked = check_name(name) ? name : L"Segoe UI";

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
			name_checked.data());
	}

	static HFONT create(const LOGFONT& lf)
	{
		if (check_name(lf.lfFaceName))
		{
			return CreateFontIndirectW(&lf);
		}
		return create(L"Segoe UI", 12, 0);
	}
}

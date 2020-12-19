#pragma once

namespace CustomSort
{
	struct Item
	{
		std::wstring text;
		size_t index = 0;
	};

	using Items = std::vector<Item>;
	using Order = std::vector<size_t>;

	template <int direction>
	static bool custom_sort_compare(const Item& a, const Item& b)
	{
		const int ret = direction * StrCmpLogicalW(a.text.data(), b.text.data());
		if (ret == 0) return a.index < b.index;
		return ret < 0;
	}

	static Order custom_sort(Items& items, int direction = 1)
	{
		Order order(items.size());
		std::ranges::sort(items, direction > 0 ? custom_sort_compare<1> : custom_sort_compare<-1>);
		std::ranges::transform(items, order.begin(), [](const Item& item) { return item.index; });
		return order;
	}

	static std::wstring make_sort_string(stringp in)
	{
		std::wstring out = L" ";
		out.append(to_wide(in));
		return out;
	}
}

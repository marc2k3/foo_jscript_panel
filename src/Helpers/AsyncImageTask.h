#pragma once

class AsyncImageTask : public SimpleThreadTask
{
public:
	AsyncImageTask(CWindow hwnd, const std::wstring& path) : m_hwnd(hwnd), m_path(path), m_cookie(reinterpret_cast<UINT_PTR>(this)) {}

	void run() override
	{
		IGdiBitmap* bitmap = ImageHelper(m_path).load();
		AsyncImageData data(m_cookie, bitmap, SysAllocString(m_path.data()));
		m_hwnd.SendMessage(to_uint(CallbackID::on_load_image_done), reinterpret_cast<WPARAM>(&data));
	}

	CWindow m_hwnd;
	size_t m_cookie = 0;
	std::wstring m_path;
};

#pragma once

class AsyncImageTask : public SimpleThreadTask
{
public:
	AsyncImageTask(CWindow hwnd, const std::wstring& path, uint32_t cookie) : m_hwnd(hwnd), m_path(path), m_cookie(cookie) {}

	void run() override
	{
		IGdiBitmap* bitmap = ImageHelpers::load(m_path);
		AsyncImageData data(m_cookie, bitmap, SysAllocString(m_path.data()));
		m_hwnd.SendMessage(to_uint(CallbackID::on_load_image_done), reinterpret_cast<WPARAM>(&data));
	}

private:
	CWindow m_hwnd;
	std::wstring m_path;
	uint32_t m_cookie = 0;
};

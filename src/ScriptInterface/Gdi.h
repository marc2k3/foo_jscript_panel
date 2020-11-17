#pragma once

class Gdi : public JSDispatchImpl<IGdi>
{
protected:
	Gdi();
	~Gdi();

public:
	STDMETHODIMP CreateImage(int w, int h, IGdiBitmap** pp) override;
	STDMETHODIMP Font(BSTR name, float pxSize, int style, IGdiFont** pp) override;
	STDMETHODIMP Image(BSTR path, IGdiBitmap** pp) override;
	STDMETHODIMP LoadImageAsync(UINT window_id, BSTR path, UINT* p) override;
};

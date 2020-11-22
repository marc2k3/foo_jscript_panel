#pragma once

class Gdi : public JSDispatchImpl<IGdi>
{
protected:
	Gdi();
	~Gdi();

public:
	STDMETHODIMP CreateImage(int w, int h, IGdiBitmap** out) override;
	STDMETHODIMP Font(BSTR name, float pxSize, int style, IGdiFont** out) override;
	STDMETHODIMP Image(BSTR path, IGdiBitmap** out) override;
	STDMETHODIMP LoadImageAsync(UINT window_id, BSTR path, UINT* out) override;
};

#include "stdafx.h"
#include "FileInfo.h"

FileInfo::FileInfo(metadb_info_container::ptr info) : m_info(info) {}
FileInfo::~FileInfo() {}

STDMETHODIMP FileInfo::get__ptr(void** out)
{
	if (!out) return E_POINTER;

	*out = m_info.get_ptr();
	return S_OK;
}

STDMETHODIMP FileInfo::InfoFind(BSTR name, int* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	*out = to_int(m_info->info().info_find(string_utf8_from_wide(name)));
	return S_OK;
}

STDMETHODIMP FileInfo::InfoName(UINT idx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	if (idx < m_info->info().info_get_count())
	{
		*out = to_bstr(m_info->info().info_enum_name(idx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::InfoValue(UINT idx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	if (idx < m_info->info().info_get_count())
	{
		*out = to_bstr(m_info->info().info_enum_value(idx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::MetaFind(BSTR name, int* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	*out = to_int(m_info->info().meta_find(string_utf8_from_wide(name)));
	return S_OK;
}

STDMETHODIMP FileInfo::MetaName(UINT idx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	if (idx < m_info->info().meta_get_count())
	{
		*out = to_bstr(m_info->info().meta_enum_name(idx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::MetaValue(UINT idx, UINT vidx, BSTR* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	if (idx < m_info->info().meta_get_count() && vidx < m_info->info().meta_enum_value_count(idx))
	{
		*out = to_bstr(m_info->info().meta_enum_value(idx, vidx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::MetaValueCount(UINT idx, UINT* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	if (idx < m_info->info().meta_get_count())
	{
		*out = m_info->info().meta_enum_value_count(idx);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::get_InfoCount(UINT* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	*out = m_info->info().info_get_count();
	return S_OK;
}

STDMETHODIMP FileInfo::get_MetaCount(UINT* out)
{
	if (m_info.is_empty() || !out) return E_POINTER;

	*out = m_info->info().meta_get_count();
	return S_OK;
}

void FileInfo::FinalRelease()
{
	m_info.release();
}

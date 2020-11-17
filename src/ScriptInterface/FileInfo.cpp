#include "stdafx.h"
#include "FileInfo.h"

FileInfo::FileInfo(metadb_info_container::ptr info) : m_info(info) {}
FileInfo::~FileInfo() {}

STDMETHODIMP FileInfo::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = m_info.get_ptr();
	return S_OK;
}

STDMETHODIMP FileInfo::InfoFind(BSTR name, int* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	*p = to_int(m_info->info().info_find(string_utf8_from_wide(name)));
	return S_OK;
}

STDMETHODIMP FileInfo::InfoName(UINT idx, BSTR* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	if (idx < m_info->info().info_get_count())
	{
		*p = to_bstr(m_info->info().info_enum_name(idx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::InfoValue(UINT idx, BSTR* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	if (idx < m_info->info().info_get_count())
	{
		*p = to_bstr(m_info->info().info_enum_value(idx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::MetaFind(BSTR name, int* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	*p = to_int(m_info->info().meta_find(string_utf8_from_wide(name)));
	return S_OK;
}

STDMETHODIMP FileInfo::MetaName(UINT idx, BSTR* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	if (idx < m_info->info().meta_get_count())
	{
		*p = to_bstr(m_info->info().meta_enum_name(idx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::MetaValue(UINT idx, UINT vidx, BSTR* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	if (idx < m_info->info().meta_get_count() && vidx < m_info->info().meta_enum_value_count(idx))
	{
		*p = to_bstr(m_info->info().meta_enum_value(idx, vidx));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::MetaValueCount(UINT idx, UINT* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	if (idx < m_info->info().meta_get_count())
	{
		*p = m_info->info().meta_enum_value_count(idx);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FileInfo::get_InfoCount(UINT* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	*p = m_info->info().info_get_count();
	return S_OK;
}

STDMETHODIMP FileInfo::get_MetaCount(UINT* p)
{
	if (m_info.is_empty() || !p) return E_POINTER;

	*p = m_info->info().meta_get_count();
	return S_OK;
}

void FileInfo::FinalRelease()
{
	m_info.release();
}

#include "stdafx.h"
#include "TitleFormat.h"

TitleFormat::TitleFormat(stringp pattern)
{
	titleformat_compiler::get()->compile_safe(m_obj, pattern);
}

TitleFormat::~TitleFormat() {}

STDMETHODIMP TitleFormat::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = m_obj.get_ptr();
	return S_OK;
}

STDMETHODIMP TitleFormat::Eval(BSTR* p)
{
	if (m_obj.is_empty() || !p) return E_POINTER;

	string8 str;
	playback_control::get()->playback_format_title(nullptr, str, m_obj, nullptr, playback_control::display_level_all);
	*p = to_bstr(str);
	return S_OK;
}

STDMETHODIMP TitleFormat::EvalWithMetadb(IMetadbHandle* handle, BSTR* p)
{
	if (m_obj.is_empty() || !p) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	string8 str;
	ptr->format_title(nullptr, str, m_obj, nullptr);
	*p = to_bstr(str);
	return S_OK;
}

STDMETHODIMP TitleFormat::EvalWithMetadbs(IMetadbHandleList* handles, VARIANT* p)
{
	if (m_obj.is_empty() || !p) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	const size_t count = handles_ptr->get_count();
	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		string8 str;
		handles_ptr->get_item(i)->format_title(nullptr, str, m_obj, nullptr);
		if (!writer.put_item(i, str)) return E_OUTOFMEMORY;
	}

	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

void TitleFormat::FinalRelease()
{
	m_obj.release();
}

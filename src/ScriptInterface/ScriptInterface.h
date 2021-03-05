#pragma once
#define GET_PTR(X, Y) \
	X->get__ptr(reinterpret_cast<void**>(&Y)); \
	if (!Y) return E_INVALIDARG;

#include <ActivScp.h>
#include <ComDef.h>
using VariantArgs = std::vector<_variant_t>;

#include "ComArray.h"
#include "ComTools.h"
#include "ScriptInterface_h.h"

#include "ContextMenuManager.h"
#include "DropAction.h"
#include "FileInfo.h"
#include "GdiBitmap.h"
#include "GdiFont.h"
#include "GdiGraphics.h"
#include "GdiRawBitmap.h"
#include "MainMenuManager.h"
#include "MeasureStringInfo.h"
#include "MenuObj.h"
#include "MetadbHandle.h"
#include "MetadbHandleList.h"
#include "PlaybackQueueItem.h"
#include "PlayingItemLocation.h"
#include "Profiler.h"
#include "ThemeManager.h"
#include "TitleFormat.h"
#include "Tooltip.h"
#include "UiSelectionHolder.h"

#include "Console.h"
#include "Fb.h"
#include "Gdi.h"
#include "Plman.h"
#include "Utils.h"
#include "Window.h"

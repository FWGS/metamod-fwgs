#pragma once
#include "precompiled.h"

// Original DLL routines, functions returning "void".
#define META_DLLAPI_HANDLE_void(FN_TYPE, pfnName, pfn_args) \
	SETUP_API_CALLS_void(FN_TYPE, pfnName, g_dllapi_info); \
	CALL_PLUGIN_API_void(P_PRE, pfnName, pfn_args, m_dllapi_table); \
	CALL_GAME_API_void(pfnName, pfn_args, dllapi_table); \
	CALL_PLUGIN_API_void(P_POST, pfnName, pfn_args, m_dllapi_post_table);

// Original DLL routines, functions returning an actual value.
#define META_DLLAPI_HANDLE(ret_t, ret_init, FN_TYPE, pfnName, pfn_args) \
	SETUP_API_CALLS(ret_t, ret_init, FN_TYPE, pfnName, g_dllapi_info); \
	CALL_PLUGIN_API(P_PRE, ret_init, pfnName, pfn_args, MRES_SUPERCEDE, m_dllapi_table); \
	CALL_GAME_API(pfnName, pfn_args, dllapi_table); \
	CALL_PLUGIN_API(P_POST, ret_init, pfnName, pfn_args, MRES_OVERRIDE, m_dllapi_post_table);


// The "new" api routines (just 3 right now), functions returning "void".
#define META_NEWAPI_HANDLE_void(FN_TYPE, pfnName, pfn_args) \
	SETUP_API_CALLS_void(FN_TYPE, pfnName, g_newapi_info); \
	CALL_PLUGIN_API_void(P_PRE, pfnName, pfn_args, m_newapi_table); \
	CALL_GAME_API_void(pfnName, pfn_args, newapi_table); \
	CALL_PLUGIN_API_void(P_POST, pfnName, pfn_args, m_newapi_post_table);

// The "new" api routines (just 3 right now), functions returning an actual value.
#define META_NEWAPI_HANDLE(ret_t, ret_init, FN_TYPE, pfnName, pfn_args) \
	SETUP_API_CALLS(ret_t, ret_init, FN_TYPE, pfnName, g_newapi_info); \
	CALL_PLUGIN_API(P_PRE, ret_init, pfnName, pfn_args, MRES_SUPERCEDE, m_newapi_table); \
	CALL_GAME_API(pfnName, pfn_args, newapi_table); \
	CALL_PLUGIN_API(P_POST, ret_init, pfnName, pfn_args, MRES_OVERRIDE, m_newapi_post_table);

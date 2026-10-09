#pragma once

// Real-code implementation of the metamod api-call chain, replacing the old
// SETUP_API_CALLS / CALL_PLUGIN_API / CALL_GAME_API / CALL_ENGINE_API /
// RETURN_API macro set (formerly at the end of metamod.h).
//
// For each hooked api call the chain is:
//   1. call the function in every running plugin's pre table;
//   2. call the "real" function (gamedll, or the engine);
//   3. call the function in every running plugin's post table;
//   4. pick the final return value per plugin result flags (MRES_*).
// Plugins report their result via gpMetaGlobals (RETURN_META macros in the
// public plugin headers) -- that protocol and META_INTERFACE_VERSION are
// unchanged.
//
// Why: the old macros expanded to ~150 lines of inline code inside every one
// of the ~200 hook stubs, which made them impossible to debug (a breakpoint
// inside the macro body hit all stubs at once, every chain stage's locals
// lived in one unreadable frame), forced GCC-only optimize("O0") workarounds
// for the varargs hooks and encouraged copy-pasted manual macro expansions
// (mm_RegUserMsg, mm_AlertMessage). Here the whole chain is ordinary function
// code: stepping, breakpoints, locals and stack frames work in any debugger,
// and hook stubs are single real calls.
//
// Behavior matches the old macros exactly:
//  - g_CALL_API_count bookkeeping incl. backup/restore of g_metaGlobals
//    around nested calls (metamod-bot-plugin fix by hullu);
//  - same log messages, wording and loglevels;
//  - MRES precedence, supercede skip, override return for value hooks;
//  - silent pass when NEW_DLL_FUNCTIONS routines are missing;
//  - stale gpMetaGlobals.orig_ret / override_ret left unset by void hooks,
//    exactly like the old SETUP_API_CALLS_void.

#include <type_traits>
#include <utility>

#include "precompiled.h"

// Everything api_call() needs to know about one hooked slot.
template <typename Table>
struct api_target
{
	const api_info_t& info;              // trace name + loglevel (api_info.h)
	const char* owner;                   // gamedll file or "engine", for logs
	Table* game_table;                   // table of the "real" routine (may be null)
	bool complain_missing;               // META_ERROR when the real routine is null

	Table* MPlugin::* plugin_pre;        // MPlugin member holding the pre table
	Table* MPlugin::* plugin_post;       // MPlugin member holding the post table
};

// The api-call families only differ in which tables they use.
inline api_target<DLL_FUNCTIONS> dllapi_target(const api_info_t& info)
{
	return api_target<DLL_FUNCTIONS> {
		info, g_GameDLL.file, g_GameDLL.funcs.dllapi_table, true,
		&MPlugin::m_dllapi_table, &MPlugin::m_dllapi_post_table
	};
}

inline api_target<NEW_DLL_FUNCTIONS> newapi_target(const api_info_t& info)
{
	// don't complain for NULL routines in NEW_DLL_FUNCTIONS (old behaviour)
	return api_target<NEW_DLL_FUNCTIONS> {
		info, g_GameDLL.file, g_GameDLL.funcs.newapi_table, false,
		&MPlugin::m_newapi_table, &MPlugin::m_newapi_post_table
	};
}

inline api_target<enginefuncs_t> engineapi_target(const api_info_t& info)
{
	return api_target<enginefuncs_t> {
		info, "engine", g_engine.funcs, true,
		&MPlugin::m_engine_table, &MPlugin::m_engine_post_table
	};
}

// RAII for the g_CALL_API_count bookkeeping:
//  - construction: counter++, and back up g_metaGlobals on nested calls;
//  - begin_real_call()/end_real_call(): counter down/up around the call of
//    the "real" routine, as CALL_GAME_API / CALL_ENGINE_API did (inner hooks
//    triggered by the real routine must see the outer nesting level);
//  - destruction: counter-- and restore.
class api_call_frame
{
public:
	api_call_frame()
	{
		if (g_CALL_API_count++ > 0)
			m_backup = g_metaGlobals;
	}

	~api_call_frame()
	{
		if (--g_CALL_API_count > 0)
			g_metaGlobals = m_backup;
	}

	void begin_real_call() { --g_CALL_API_count; }
	void end_real_call()   { ++g_CALL_API_count; }

	api_call_frame(const api_call_frame&) = delete;
	api_call_frame& operator=(const api_call_frame&) = delete;

private:
	meta_globals_t m_backup{};
};

// placeholder for the ret_init parameter of void chains
struct api_no_ret {};

// The core. R is the api function's return type (void for void chains);
// HasRet must match !std::is_void_v<R>. All the state that used to live in
// macro-declared locals is now ordinary function locals.
template <bool HasRet, typename R, typename Table, typename FnPtr, typename... Args>
inline std::conditional_t<HasRet, R, void> api_call(
	FnPtr Table::* member,
	const api_target<Table>& target,
	std::conditional_t<HasRet, R, api_no_ret> ret_init,
	Args... args)
{
	static_assert(!std::is_void_v<R> == HasRet, "api_call: R/HasRet mismatch");
	static_assert(HasRet || std::is_void_v<R>, "api_call: R/HasRet mismatch");

	api_call_frame frame;                       // old SETUP_API_CALLS + RETURN_API
	meta_globals_t& g = g_metaGlobals;

	const int loglevel = static_cast<int>(target.info.loglevel);
	const char* pfn_string = target.info.name;

	META_RES mres = MRES_UNSET;
	META_RES status = MRES_UNSET;
	META_RES prev_mres = MRES_UNSET;

	// return-value plumbing, only for functions returning a real value
	using ret_box_t = std::conditional_t<HasRet, R, api_no_ret>;
	[[maybe_unused]] ret_box_t dllret{}, orig_ret{}, pub_orig_ret{}, override_ret{}, pub_override_ret{};

	if constexpr (HasRet) {
		dllret = orig_ret = pub_orig_ret = override_ret = pub_override_ret = ret_init;
	}

	auto invoke_routine = [&](FnPtr pfn_routine)
	{
		if constexpr (HasRet) {
			dllret = pfn_routine(args...);
		} else {
			pfn_routine(args...);
		}
	};

	// old CALL_PLUGIN_API: call the function in each running plugin
	auto call_plugin_pass = [&](Table* MPlugin::* which, bool post, META_RES sup_type)
	{
		prev_mres = MRES_UNSET;
		if constexpr (HasRet) {
			override_ret = ret_init;
		}

		for (MPlugin *iplug : *g_plugins->getPlugins())
		{
			if (iplug->status() != PL_RUNNING)
				continue;

			Table* table = iplug->*which;
			if (!table)
				continue; // plugin doesn't provide this api table

			FnPtr pfn_routine = table->*member;
			if (!pfn_routine)
				continue; // plugin doesn't provide this function

			// initialize g_metaGlobals for the plugin call
			g.mres = MRES_UNSET;
			g.prev_mres = prev_mres;
			g.status = status;

			if constexpr (HasRet)
			{
				pub_orig_ret = orig_ret;
				g.orig_ret = &pub_orig_ret;
				if (status == sup_type)
				{
					pub_override_ret = override_ret;
					g.override_ret = &pub_override_ret;
				}
			}

			META_DEBUG(loglevel, "Calling %s:%s%s()", iplug->file(), pfn_string, (post ? "_Post" : ""));
			invoke_routine(pfn_routine);

			// plugin's result code
			mres = g.mres;
			if (mres > status) {
				status = mres;
			}
			// save this for successive plugins to see
			prev_mres = mres;

			if constexpr (HasRet)
			{
				if (mres == sup_type)
					override_ret = pub_override_ret = dllret;
				else if (mres == MRES_UNSET)
					META_ERROR("Plugin didn't set meta_result: %s:%s%s()", iplug->file(), pfn_string, (post ? "_Post" : ""));
				else if (post && mres == MRES_SUPERCEDE)
					META_ERROR("MRES_SUPERCEDE not valid in Post functions: %s:%s%s()", iplug->file(), pfn_string, (post ? "_Post" : ""));
			}
			else
			{
				if (mres == MRES_UNSET)
					META_ERROR("Plugin didn't set meta_result: %s:%s%s()", iplug->file(), pfn_string, (post ? "_Post" : ""));
				if (post && mres == MRES_SUPERCEDE)
					META_ERROR("MRES_SUPERCEDE not valid in Post functions: %s:%s%s()", iplug->file(), pfn_string, (post ? "_Post" : ""));
			}
		}
	};

	// pre hooks
	call_plugin_pass(target.plugin_pre, false, MRES_SUPERCEDE);

	// old CALL_GAME_API / CALL_ENGINE_API: call the "real" routine
	frame.begin_real_call();

	if (status == MRES_SUPERCEDE)
	{
		META_DEBUG(loglevel, "Skipped (supercede) %s:%s()", target.owner, pfn_string);
		// don't return here; superceded routine, but still allow
		// _post routines to run.
		if constexpr (HasRet)
		{
			orig_ret = pub_orig_ret = override_ret;
			g.orig_ret = &pub_orig_ret;
		}
	}
	else if (target.game_table)
	{
		FnPtr pfn_routine = target.game_table->*member;
		if (pfn_routine)
		{
			META_DEBUG(loglevel, "Calling %s:%s()", target.owner, pfn_string);
			invoke_routine(pfn_routine);
			if constexpr (HasRet) {
				orig_ret = dllret;
			}
		}
		else if (target.complain_missing)
		{
			// NULL routines in NEW_DLL_FUNCTIONS are not an error (old behaviour)
			META_ERROR("Couldn't find api call: %s:%s", target.owner, pfn_string);
			status = MRES_UNSET;
		}
	}
	else
	{
		META_DEBUG(loglevel, "No api table defined for api call: %s:%s", target.owner, pfn_string);
	}

	frame.end_real_call();

	// post hooks
	call_plugin_pass(target.plugin_post, true, MRES_OVERRIDE);

	// old RETURN_API
	if constexpr (HasRet)
	{
		if (status == MRES_OVERRIDE)
		{
			META_DEBUG(loglevel, "Returning (override) %s()", pfn_string);
			return override_ret;
		}
		return orig_ret;
	}
}

// api function returning a real value; R (the return type, e.g. BOOL,
// float, edict_t*) is given explicitly, ret_init is its initial/default value.
template <typename R, typename Table, typename FnPtr, typename... Args>
inline R meta_call(FnPtr Table::* member, const api_target<Table>& target, R ret_init, Args... args)
{
	static_assert(!std::is_void_v<R>, "meta_call(): api function returns void, use meta_call_void()");
	return api_call<true, R>(member, target, ret_init, args...);
}

// api function returning void
template <typename Table, typename FnPtr, typename... Args>
inline void meta_call_void(FnPtr Table::* member, const api_target<Table>& target, Args... args)
{
	api_call<false, void>(member, target, {}, args...);
}

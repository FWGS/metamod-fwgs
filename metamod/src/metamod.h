#pragma once

#include "meta_api.h"       // META_RES, etc
#include "mlist.h"          // MPluginList, etc
#include "mreg.h"           // MRegCmdList, etc
#include "conf_meta.h"      // MConfig
#include "osdep.h"          // NAME_MAX, etc
#include "mplayer.h"        // MPlayerList
#include "engine_t.h"       // engine_t, Engine

#define PLUGINS_INI         "plugins.ini"   // file that lists plugins to load at startup
#define EXEC_CFG            "exec.cfg"      // file that contains commands to metamod plugins at startup
#define CONFIG_INI          "config.ini"    // generic config file

// cvar to contain version
extern cvar_t g_meta_version;

// metamod module handle
extern CSysModule g_metamod_module;

// Info about the game dll/mod.
struct gamedll_t
{
	char name[NAME_MAX];                // ie "cstrike" (from gamedir)
	char desc[NAME_MAX];                // ie "Counter-Strike"
	char gamedir[MAX_PATH];             // ie "/home/willday/half-life/cstrike"
	char pathname[MAX_PATH];            // ie "/home/willday/half-life/cstrike/dlls/cs_i386.so"
	char const* file;                   // ie "cs_i386.so"
	char real_pathname[MAX_PATH];       // in case pathname overridden by bot, etc
	CSysModule sys_module;
	gamedll_funcs_t funcs;              // dllapi_table, newapi_table
};

extern gamedll_t g_GameDLL;

// SDK variables for storing engine funcs and globals.
extern enginefuncs_t g_engfuncs;
extern globalvars_t* gpGlobals;

// g_config structure.
extern MConfig* g_config;

// List of plugins loaded/opened/running.
extern MPluginList* g_plugins;

// List of command functions registered by plugins.
extern MRegCmdList* g_regCmds;

// List of cvar structures registered by plugins.
extern MRegCvarList* g_regCvars;

// List of user messages registered by gamedll.
extern MRegMsgList* g_regMsgs;

#ifdef METAMOD_CORE
ALIGN16
#endif

// Data provided to plugins.
// Separate copies to prevent plugins from modifying "readable" parts.
// See meta_api.h for meta_globals_t structure.
extern meta_globals_t g_metaGlobals;

// hook function tables
extern DLL_FUNCTIONS* pHookedDllFunctions;
extern NEW_DLL_FUNCTIONS* pHookedNewDllFunctions;

// (patch by hullu)
// Safety check for metamod-bot-plugin bugfix.
//  engine_api->pfnRunPlayerMove calls dllapi-functions before it returns.
//  This causes problems with bots running as metamod plugins, because
//  metamod assumed that g_metaGlobals is free to be used.
//  With call_count we can fix this by backuping up g_metaGlobals if
//  it's already being used.
extern unsigned int g_CALL_API_count;

// stores previous requestid counter
extern int g_requestid_counter;

extern bool g_metamod_active;
extern bool g_dedicated_server;

// (patch by BAILOPAN)
// Holds cached player info, right now only things for querying cvars
// Max players is always 32, small enough that we can use a static array
extern MPlayerList g_players;

void metamod_startup();

bool meta_init_gamedll();
bool meta_load_gamedll();
void meta_rebuild_callbacks();

// lotsa macros...

// These are the meat of the metamod processing, and are as ugly as (or
// uglier) than they look.  This is done via macros, because of the varying
// parameter types (int, void, edict_t*, etc) as well as varying
// function-pointer types and different api tables (dllapi, newapi,
// engine), which just can't be passed to a function.  And, since the
// operation is similar for each api call, I didn't want to keep
// duplicating code all over the place.  Thus the ugly macros.
//
// The basic operation is, for each api call:
//  - iterate through list of plugins
//  - for each plugin, if it provides this api call, then call the
//    function in the plugin
//  - call the "real" function (in the game dll, or from the engine)
//  - for each plugin, check for a "post" version of the function, and call
//    if present
//
//
// Also, for any api call, each plugin has the opportunity to replace the
// real routine, in two ways:
//  - prevent the real routine from being called ("supercede")
//  - allow the real routine to be called, but change the value that's
//    returned ("override")
//
// Thus after each plugin is called, its META_RETURN flag is checked, and
// action taken appropriately.  Note that supercede/override only affects
// the _real_ routine; other plugins will still be called.
//
// In addition to the SUPERCEDE and OVERRIDE flags, there are two
// additional flags a plugin can return:
//  - HANDLED ("I did something here")
//  - IGNORED ("I didn't really do anything")
//
// These aren't used by metamod itself, but could be used by plugins to
// get an idea if a previous plugin did anything.
//
//
// The 5 basic macros are:
//   SETUP
//   CALL_PLUGIN
//   CALL_GAME  and  CALL_ENGINE
//   RETURN
//
// These 5 are actually used to build second level macros for each api type
// (dllapi, newapi, engine), with the CALL_PLUGIN macro being used twice
// (before and after).  Altogether, they end up expanding to approx 150
// lines of code for _each_ api call.  Ack, ugly indeed.
//
// However, due to some functions returning 'void', and others returning an
// actual value, I had to have separate macros for the two, since I
// couldn't seem to generalize the two into a form that the compiler would
// accept.  Thus there are "_void" versions of the 5 macros; these are
// listed first.

// macros for void-returning functions

// declare/init some variables
#define SETUP_API_CALLS_void(FN_TYPE, pfnName, api_info_table) \
	META_RES mres = MRES_UNSET, status = MRES_UNSET, prev_mres = MRES_UNSET; \
	FN_TYPE pfn_routine = NULL; \
	int loglevel = api_info_table.pfnName.loglevel; \
	const char *pfn_string = api_info_table.pfnName.name; \
	meta_globals_t backup_meta_globals; \
	/* fix bug with metamod-bot-plugins (hullu)*/ \
	if (g_CALL_API_count++>0) \
		/* backup g_metaGlobals */ \
		backup_meta_globals = g_metaGlobals;

// call each plugin
#define CALL_PLUGIN_API_void(post, pfnName, pfn_args, api_table) \
	prev_mres = MRES_UNSET; \
	for (MPlugin *iplug : *g_plugins->getPlugins()) { \
		if (iplug->status() != PL_RUNNING) \
			continue; \
		if (iplug->api_table && (pfn_routine = iplug->api_table->pfnName)); \
		else \
			/* plugin doesn't provide this function */  \
			continue; \
		/* initialize g_metaGlobals */ \
		g_metaGlobals.mres = MRES_UNSET; \
		g_metaGlobals.prev_mres = prev_mres; \
		g_metaGlobals.status = status; \
		/* call plugin */ \
		META_DEBUG(loglevel, ("Calling %s:%s%s()", iplug->file(), pfn_string, (post?"_Post":""))); \
		pfn_routine pfn_args; \
		/* plugin's result code */ \
		mres = g_metaGlobals.mres; \
		if (mres > status) \
			status = mres; \
		/* save this for successive plugins to see */ \
		prev_mres = mres; \
		if (mres == MRES_UNSET) \
			META_ERROR("Plugin didn't set meta_result: %s:%s%s()", iplug->file(), pfn_string, (post?"_Post":"")); \
		if (post && mres == MRES_SUPERCEDE) \
			META_ERROR("MRES_SUPERCEDE not valid in Post functions: %s:%s%s()", iplug->file(), pfn_string, (post?"_Post":"")); \
	}

// call "real" function, from gamedll
#define CALL_GAME_API_void(pfnName, pfn_args, api_table) \
	g_CALL_API_count--; \
	if (status == MRES_SUPERCEDE) { \
		META_DEBUG(loglevel, ("Skipped (supercede) %s:%s()", g_GameDLL.file, pfn_string)); \
		/* don't return here; superceded game routine, but still allow \
		 * _post routines to run. \
		 */ \
	} \
	else if (g_GameDLL.funcs.api_table) { \
		pfn_routine = g_GameDLL.funcs.api_table->pfnName; \
		if (pfn_routine) { \
			META_DEBUG(loglevel, ("Calling %s:%s()", g_GameDLL.file, pfn_string)); \
			pfn_routine pfn_args; \
		} \
		/* don't complain for NULL routines in NEW_DLL_FUNCTIONS  */ \
		else if ((void *)g_GameDLL.funcs.api_table != (void *)g_GameDLL.funcs.newapi_table) { \
			META_ERROR("Couldn't find api call: %s:%s", g_GameDLL.file, pfn_string); \
			status = MRES_UNSET; \
		} \
	} \
	else { \
		META_DEBUG(loglevel, ("No api table defined for api call: %s:%s", g_GameDLL.file, pfn_string)); \
	} \
	g_CALL_API_count++;

// call "real" function, from engine
#define CALL_ENGINE_API_void(pfnName, pfn_args) \
	g_CALL_API_count--; \
	if (status == MRES_SUPERCEDE) { \
		META_DEBUG(loglevel, ("Skipped (supercede) engine:%s()", pfn_string)); \
		/* don't return here; superceded game routine, but still allow \
		 * _post routines to run. \
		 */ \
	} \
	else { \
		pfn_routine = g_engine.funcs->pfnName; \
		if (pfn_routine) { \
			META_DEBUG(loglevel, ("Calling engine:%s()", pfn_string)); \
			pfn_routine pfn_args; \
		} \
		else { \
			META_ERROR("Couldn't find api call: engine:%s", pfn_string); \
			status = MRES_UNSET; \
		} \
	} \
	g_CALL_API_count++;

// return (void)
#define RETURN_API_void() \
	if (--g_CALL_API_count>0) \
		/*restore backup*/ \
		g_metaGlobals = backup_meta_globals; \
	return;


// macros for type-returning functions
// declare/init some variables

#define SETUP_API_CALLS(ret_t, ret_init, FN_TYPE, pfnName, api_info_table) \
	ret_t dllret = ret_init; \
	ret_t override_ret = ret_init; \
	ret_t pub_override_ret = ret_init; \
	ret_t orig_ret = ret_init; \
	ret_t pub_orig_ret = ret_init; \
	META_RES mres = MRES_UNSET, status = MRES_UNSET, prev_mres = MRES_UNSET; \
	FN_TYPE pfn_routine = NULL; \
	int loglevel = api_info_table.pfnName.loglevel; \
	const char *pfn_string = api_info_table.pfnName.name; \
	meta_globals_t backup_meta_globals; \
	/*Fix bug with metamod-bot-plugins*/ \
	if (g_CALL_API_count++>0) \
		/*Backup g_metaGlobals*/ \
		backup_meta_globals = g_metaGlobals;

// call each plugin
#define CALL_PLUGIN_API(post, ret_init, pfnName, pfn_args, MRES_TYPE, api_table) \
	override_ret = ret_init; \
	prev_mres = MRES_UNSET; \
	  for (MPlugin *iplug : *g_plugins->getPlugins()) { \
		if (iplug->status() != PL_RUNNING) \
			continue; \
		if (iplug->api_table && (pfn_routine = iplug->api_table->pfnName)); \
		else \
			/* plugin doesn't provide this function */  \
			continue; \
		/* initialize g_metaGlobals */ \
		g_metaGlobals.mres = MRES_UNSET; \
		g_metaGlobals.prev_mres = prev_mres; \
		g_metaGlobals.status = status; \
		pub_orig_ret = orig_ret; \
		g_metaGlobals.orig_ret = &pub_orig_ret; \
		if (status == MRES_TYPE) { \
			pub_override_ret = override_ret; \
			g_metaGlobals.override_ret = &pub_override_ret; \
		} \
		/* call plugin */ \
		META_DEBUG(loglevel, ("Calling %s:%s%s()", iplug->file(), pfn_string, (post?"_Post":""))); \
		dllret = pfn_routine pfn_args; \
		/* plugin's result code */ \
		mres = g_metaGlobals.mres; \
		if (mres > status) \
			status = mres; \
		/* save this for successive plugins to see */ \
		prev_mres = mres; \
		if (mres == MRES_TYPE) \
			override_ret = pub_override_ret = dllret; \
		else if (mres == MRES_UNSET) \
			META_ERROR("Plugin didn't set meta_result: %s:%s%s()", iplug->file(), pfn_string, (post?"_Post":"")); \
		else if (post && mres == MRES_SUPERCEDE) \
			META_ERROR("MRES_SUPERCEDE not valid in Post functions: %s:%s%s()", iplug->file(), pfn_string, (post?"_Post":"")); \
	}

// call "real" function, from gamedll
#define CALL_GAME_API(pfnName, pfn_args, api_table) \
	g_CALL_API_count--; \
	if (status == MRES_SUPERCEDE) { \
		META_DEBUG(loglevel, ("Skipped (supercede) %s:%s()", g_GameDLL.file, pfn_string)); \
		orig_ret = pub_orig_ret = override_ret; \
		g_metaGlobals.orig_ret = &pub_orig_ret; \
		/* don't return here; superceded game routine, but still allow \
		 * _post routines to run. \
		 */ \
	} \
	else if (g_GameDLL.funcs.api_table) { \
		pfn_routine = g_GameDLL.funcs.api_table->pfnName; \
		if (pfn_routine) { \
			META_DEBUG(loglevel, ("Calling %s:%s()", g_GameDLL.file, pfn_string)); \
			dllret = pfn_routine pfn_args; \
			orig_ret = dllret; \
		} \
		/* don't complain for NULL routines in NEW_DLL_FUNCTIONS  */ \
		else if ((void *)g_GameDLL.funcs.api_table != (void *)g_GameDLL.funcs.newapi_table) { \
			META_ERROR("Couldn't find api call: %s:%s", g_GameDLL.file, pfn_string); \
			status = MRES_UNSET; \
		} \
	} \
	else { \
		META_DEBUG(loglevel, ("No api table defined for api call: %s:%s", g_GameDLL.file, pfn_string)); \
	} \
	g_CALL_API_count++;

// call "real" function, from engine
#define CALL_ENGINE_API(pfnName, pfn_args) \
	g_CALL_API_count--; \
	if (status == MRES_SUPERCEDE) { \
		META_DEBUG(loglevel, ("Skipped (supercede) engine:%s()", pfn_string)); \
		orig_ret = pub_orig_ret = override_ret; \
		g_metaGlobals.orig_ret = &pub_orig_ret; \
		/* don't return here; superceded game routine, but still allow \
		 * _post routines to run. \
		 */ \
	} \
	else { \
		pfn_routine = g_engine.funcs->pfnName; \
		if (pfn_routine) { \
			META_DEBUG(loglevel, ("Calling engine:%s()", pfn_string)); \
			dllret = pfn_routine pfn_args; \
			orig_ret = dllret; \
		} \
		else { \
			META_ERROR("Couldn't find api call: engine:%s", pfn_string); \
			status = MRES_UNSET; \
		} \
	} \
	g_CALL_API_count++;

// return a value
#define RETURN_API() \
	if (--g_CALL_API_count>0) \
		/*Restore backup*/ \
		g_metaGlobals = backup_meta_globals; \
	if (status == MRES_OVERRIDE) { \
		META_DEBUG(loglevel, ("Returning (override) %s()", pfn_string)); \
		return override_ret; \
	} \
	else \
		return orig_ret;

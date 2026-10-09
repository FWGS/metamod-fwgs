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
extern server_physics_api_t g_meta_physfuncs;

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
void meta_print_version_info(edict_t *pEntity);

// The api-call chain (plugin pre pass, "real" routine, plugin post pass,
// MRES result handling) used to be implemented as a large macro set here;
// it lives in api_hook.h now, as real (template) code.

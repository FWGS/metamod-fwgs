#include "precompiled.h"

// Unload game DLL and meta plugins
static void MM_POST_HOOK EXT_FUNC mm_GameShutdown()
{
	meta_call_void(&NEW_DLL_FUNCTIONS::pfnGameShutdown, newapi_target(g_newapi_info.pfnGameShutdown));

	g_metamod_active = false;
	if (g_plugins && !g_dedicated_server)
		g_plugins->unload_all();

	g_meta_extdll.unload();
	g_GameDLL.sys_module.unload();
	g_engine.sys_module.unload();

}

static BOOL mm_ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	g_players.clear_player_cvar_query(pEntity);
	return meta_call<BOOL>(&DLL_FUNCTIONS::pfnClientConnect, dllapi_target(g_dllapi_info.pfnClientConnect), TRUE, pEntity, pszName, pszAddress, szRejectReason);
}

// NOTE: this hook MUST forward to the chain, like every other api hook;
// the call was lost during the restoration of the api hooks after the JIT
// machinery was removed (commit 5de9af2) and restored here.
static void MM_PRE_HOOK EXT_FUNC mm_ClientDisconnect(edict_t *pEntity)
{
	g_players.clear_player_cvar_query(pEntity);
	meta_call_void(&DLL_FUNCTIONS::pfnClientDisconnect, dllapi_target(g_dllapi_info.pfnClientDisconnect), pEntity);
}

// this forward can be disabled from metamod.cpp
static void MM_PRE_HOOK mm_ClientCommand(edict_t *pEntity)
{
	if (!g_config->m_clientmeta && !Q_strcmp(CMD_ARGV(0), "meta")) {
		client_meta(pEntity);
		return;
	}

	meta_call_void(&DLL_FUNCTIONS::pfnClientCommand, dllapi_target(g_dllapi_info.pfnClientCommand), pEntity);
}

static void EXT_FUNC mm_ServerDeactivate()
{
	//sFunctionTable_jit.pfnServerDeactivate();
	meta_call_void(&DLL_FUNCTIONS::pfnServerDeactivate, dllapi_target(g_dllapi_info.pfnServerDeactivate));

	// Update loaded plugins.  Look for new plugins in inifile, as well as
	// any plugins waiting for a changelevel to load.
	//
	// This is done in ServerDeactivate rather than Activate, as the latter
	// isn't actually the first routine to be called on a new map.  In
	// particular DispatchKeyValue and DispatchSpawn are called before
	// Activate, and we want any newly loaded plugins to be able to catch
	// these.
	//
	// So, we do this from Deactivate, which is the _last_ routine called
	// from the previous map.  It's also called right before shutdown,
	// which means whenever hlds quits, it'll reload the plugins just
	// before it exits, which is rather silly, but oh well.
	g_plugins->refresh(PT_CHANGELEVEL); // <- callbacks rebuilded (!)
	g_plugins->unpause_all();
	// g_plugins->retry_all(PT_CHANGELEVEL);
	g_players.clear_all_cvar_queries();
	g_requestid_counter = 0;

}

static void mm_GameDLLInit(void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnGameInit, dllapi_target(g_dllapi_info.pfnGameInit));
}

static int mm_DispatchSpawn(edict_t *pent)
{
	// Success == 0, Failure == -1 ?
	return meta_call<int>(&DLL_FUNCTIONS::pfnSpawn, dllapi_target(g_dllapi_info.pfnSpawn), 0, pent);
}

static void mm_DispatchThink(edict_t *pent)
{
	meta_call_void(&DLL_FUNCTIONS::pfnThink, dllapi_target(g_dllapi_info.pfnThink), pent);
}

static void mm_DispatchUse(edict_t *pentUsed, edict_t *pentOther)
{
	meta_call_void(&DLL_FUNCTIONS::pfnUse, dllapi_target(g_dllapi_info.pfnUse), pentUsed, pentOther);
}

static void mm_DispatchTouch(edict_t *pentTouched, edict_t *pentOther)
{
	meta_call_void(&DLL_FUNCTIONS::pfnTouch, dllapi_target(g_dllapi_info.pfnTouch), pentTouched, pentOther);
}

static void mm_DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther)
{
	meta_call_void(&DLL_FUNCTIONS::pfnBlocked, dllapi_target(g_dllapi_info.pfnBlocked), pentBlocked, pentOther);
}

static void mm_DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd)
{
	meta_call_void(&DLL_FUNCTIONS::pfnKeyValue, dllapi_target(g_dllapi_info.pfnKeyValue), pentKeyvalue, pkvd);
}

static void mm_DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSave, dllapi_target(g_dllapi_info.pfnSave), pent, pSaveData);
}

static int mm_DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity)
{
	// Success == 0, Failure == -1 ?
	return meta_call<int>(&DLL_FUNCTIONS::pfnRestore, dllapi_target(g_dllapi_info.pfnRestore), 0, pent, pSaveData, globalEntity);
}

static void mm_DispatchObjectCollsionBox(edict_t *pent)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSetAbsBox, dllapi_target(g_dllapi_info.pfnSetAbsBox), pent);
}

static void mm_SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSaveWriteFields, dllapi_target(g_dllapi_info.pfnSaveWriteFields), pSaveData, pname, pBaseData, pFields, fieldCount);
}

static void mm_SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSaveReadFields, dllapi_target(g_dllapi_info.pfnSaveReadFields), pSaveData, pname, pBaseData, pFields, fieldCount);
}

static void mm_SaveGlobalState(SAVERESTOREDATA *pSaveData)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSaveGlobalState, dllapi_target(g_dllapi_info.pfnSaveGlobalState), pSaveData);
}

static void mm_RestoreGlobalState(SAVERESTOREDATA *pSaveData)
{
	meta_call_void(&DLL_FUNCTIONS::pfnRestoreGlobalState, dllapi_target(g_dllapi_info.pfnRestoreGlobalState), pSaveData);
}

static void mm_ResetGlobalState(void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnResetGlobalState, dllapi_target(g_dllapi_info.pfnResetGlobalState));
}

static void mm_ClientKill(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnClientKill, dllapi_target(g_dllapi_info.pfnClientKill), pEntity);
}

static void mm_ClientPutInServer(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnClientPutInServer, dllapi_target(g_dllapi_info.pfnClientPutInServer), pEntity);
}

static void mm_ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
	meta_call_void(&DLL_FUNCTIONS::pfnClientUserInfoChanged, dllapi_target(g_dllapi_info.pfnClientUserInfoChanged), pEntity, infobuffer);
}

static void mm_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	meta_call_void(&DLL_FUNCTIONS::pfnServerActivate, dllapi_target(g_dllapi_info.pfnServerActivate), pEdictList, edictCount, clientMax);
}

static void mm_PlayerPreThink(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnPlayerPreThink, dllapi_target(g_dllapi_info.pfnPlayerPreThink), pEntity);
}

static void mm_PlayerPostThink(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnPlayerPostThink, dllapi_target(g_dllapi_info.pfnPlayerPostThink), pEntity);
}

static void mm_StartFrame(void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnStartFrame, dllapi_target(g_dllapi_info.pfnStartFrame));
}

static void mm_ParmsNewLevel(void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnParmsNewLevel, dllapi_target(g_dllapi_info.pfnParmsNewLevel));
}

static void mm_ParmsChangeLevel(void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnParmsChangeLevel, dllapi_target(g_dllapi_info.pfnParmsChangeLevel));
}

static const char *mm_GetGameDescription(void)
{
	return meta_call<const char *>(&DLL_FUNCTIONS::pfnGetGameDescription, dllapi_target(g_dllapi_info.pfnGetGameDescription), NULL);
}

static void mm_PlayerCustomization(edict_t *pEntity, customization_t *pCust)
{
	meta_call_void(&DLL_FUNCTIONS::pfnPlayerCustomization, dllapi_target(g_dllapi_info.pfnPlayerCustomization), pEntity, pCust);
}

static void mm_SpectatorConnect(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSpectatorConnect, dllapi_target(g_dllapi_info.pfnSpectatorConnect), pEntity);
}

static void mm_SpectatorDisconnect(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSpectatorDisconnect, dllapi_target(g_dllapi_info.pfnSpectatorDisconnect), pEntity);
}

static void mm_SpectatorThink(edict_t *pEntity)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSpectatorThink, dllapi_target(g_dllapi_info.pfnSpectatorThink), pEntity);
}

static void mm_Sys_Error(const char *error_string)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSys_Error, dllapi_target(g_dllapi_info.pfnSys_Error), error_string);
}

static void mm_PM_Move (struct playermove_s *ppmove, int server)
{
	meta_call_void(&DLL_FUNCTIONS::pfnPM_Move, dllapi_target(g_dllapi_info.pfnPM_Move), ppmove, server);
}

static void mm_PM_Init(struct playermove_s *ppmove)
{
	meta_call_void(&DLL_FUNCTIONS::pfnPM_Init, dllapi_target(g_dllapi_info.pfnPM_Init), ppmove);
}

static char mm_PM_FindTextureType(char *name)
{
	return meta_call<char>(&DLL_FUNCTIONS::pfnPM_FindTextureType, dllapi_target(g_dllapi_info.pfnPM_FindTextureType), '\0', name);
}

static void mm_SetupVisibility(edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas)
{
	meta_call_void(&DLL_FUNCTIONS::pfnSetupVisibility, dllapi_target(g_dllapi_info.pfnSetupVisibility), pViewEntity, pClient, pvs, pas);
}

static void mm_UpdateClientData (const struct edict_s *ent, int sendweapons, struct clientdata_s *cd)
{
	meta_call_void(&DLL_FUNCTIONS::pfnUpdateClientData, dllapi_target(g_dllapi_info.pfnUpdateClientData), ent, sendweapons, cd);
}

static int mm_AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
	return meta_call<int>(&DLL_FUNCTIONS::pfnAddToFullPack, dllapi_target(g_dllapi_info.pfnAddToFullPack), 0, state, e, ent, host, hostflags, player, pSet);
}

static void mm_CreateBaseline(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs)
{
	meta_call_void(&DLL_FUNCTIONS::pfnCreateBaseline, dllapi_target(g_dllapi_info.pfnCreateBaseline), player, eindex, baseline, entity, playermodelindex, player_mins, player_maxs);
}

static void mm_RegisterEncoders(void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnRegisterEncoders, dllapi_target(g_dllapi_info.pfnRegisterEncoders));
}

static int mm_GetWeaponData(struct edict_s *player, struct weapon_data_s *info)
{
	return meta_call<int>(&DLL_FUNCTIONS::pfnGetWeaponData, dllapi_target(g_dllapi_info.pfnGetWeaponData), 0, player, info);
}

static void mm_CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed)
{
	meta_call_void(&DLL_FUNCTIONS::pfnCmdStart, dllapi_target(g_dllapi_info.pfnCmdStart), player, cmd, random_seed);
}

static void mm_CmdEnd (const edict_t *player)
{
	meta_call_void(&DLL_FUNCTIONS::pfnCmdEnd, dllapi_target(g_dllapi_info.pfnCmdEnd), player);
}

static int mm_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	return meta_call<int>(&DLL_FUNCTIONS::pfnConnectionlessPacket, dllapi_target(g_dllapi_info.pfnConnectionlessPacket), 0, net_from, args, response_buffer, response_buffer_size);
}

static int mm_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	return meta_call<int>(&DLL_FUNCTIONS::pfnGetHullBounds, dllapi_target(g_dllapi_info.pfnGetHullBounds), 0, hullnumber, mins, maxs);
}

static void mm_CreateInstancedBaselines (void)
{
	meta_call_void(&DLL_FUNCTIONS::pfnCreateInstancedBaselines, dllapi_target(g_dllapi_info.pfnCreateInstancedBaselines));
}

static int mm_InconsistentFile(const edict_t *player, const char *filename, char *disconnect_message)
{
	return meta_call<int>(&DLL_FUNCTIONS::pfnInconsistentFile, dllapi_target(g_dllapi_info.pfnInconsistentFile), 0, player, filename, disconnect_message);
}

static int mm_AllowLagCompensation(void)
{
	return meta_call<int>(&DLL_FUNCTIONS::pfnAllowLagCompensation, dllapi_target(g_dllapi_info.pfnAllowLagCompensation), 0);
}

static void mm_OnFreeEntPrivateData(edict_t *pEnt)
{
	meta_call_void(&NEW_DLL_FUNCTIONS::pfnOnFreeEntPrivateData, newapi_target(g_newapi_info.pfnOnFreeEntPrivateData), pEnt);
}

static int mm_ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	return meta_call<int>(&NEW_DLL_FUNCTIONS::pfnShouldCollide, newapi_target(g_newapi_info.pfnShouldCollide), 1, pentTouched, pentOther);
}

static void mm_CvarValue(const edict_t *pEdict, const char *value)
{
	g_players.clear_player_cvar_query(pEdict);
	meta_call_void(&NEW_DLL_FUNCTIONS::pfnCvarValue, newapi_target(g_newapi_info.pfnCvarValue), pEdict, value);
}

static void mm_CvarValue2(const edict_t *pEdict, int requestID, const char *cvarName, const char *value)
{
	meta_call_void(&NEW_DLL_FUNCTIONS::pfnCvarValue2, newapi_target(g_newapi_info.pfnCvarValue2), pEdict, requestID, cvarName, value);
}

DLL_FUNCTIONS sFunctionTable =
{
	mm_GameDLLInit,				// pfnGameInit()			Initialize the game (one-time call after loading of game .dll)
	mm_DispatchSpawn,			// pfnSpawn()
	mm_DispatchThink,			// pfnThink()
	mm_DispatchUse,				// pfnUse()
	mm_DispatchTouch,			// pfnTouch()
	mm_DispatchBlocked,			// pfnBlocked()
	mm_DispatchKeyValue,			// pfnKeyValue()
	mm_DispatchSave,			// pfnSave()
	mm_DispatchRestore,			// pfnRestore()
	mm_DispatchObjectCollsionBox,		// pfnSetAbsBox()

	mm_SaveWriteFields,			// pfnSaveWriteFields()
	mm_SaveReadFields,			// pfnSaveReadFields()

	mm_SaveGlobalState,			// pfnSaveGlobalState()
	mm_RestoreGlobalState,			// pfnRestoreGlobalState()
	mm_ResetGlobalState,			// pfnResetGlobalState()

	mm_ClientConnect,			// pfnClientConnect()			(wd) Client has connected
	mm_ClientDisconnect,			// pfnClientDisconnect()		(wd) Player has left the game
	mm_ClientKill,				// pfnClientKill()			(wd) Player has typed "kill"
	mm_ClientPutInServer,			// pfnClientPutInServer()		(wd) Client is entering the game
	mm_ClientCommand,			// pfnClientCommand()			(wd) Player has sent a command (typed, or from a bind)
	mm_ClientUserInfoChanged,		// pfnClientUserInfoChanged()		(wd) Client has updated their setinfo structure
	mm_ServerActivate,			// pfnServerActivate()			(wd) Server is starting a new map
	mm_ServerDeactivate,			// pfnServerDeactivate()		(wd) Server is leaving the map (shutdown, or changelevel); SDK2

	mm_PlayerPreThink,			// pfnPlayerPreThink()
	mm_PlayerPostThink,			// pfnPlayerPostThink()

	mm_StartFrame,				// pfnStartFrame()
	mm_ParmsNewLevel,			// pfnParmsNewLevel()
	mm_ParmsChangeLevel,			// pfnParmsChangeLevel()

	mm_GetGameDescription,			// pfnGetGameDescription()		Returns string describing current .dll.  E.g. "TeamFotrress 2", "Half-Life"
	mm_PlayerCustomization,			// pfnPlayerCustomization()		Notifies .dll of new customization for player.

	mm_SpectatorConnect,			// pfnSpectatorConnect()		Called when spectator joins server
	mm_SpectatorDisconnect,			// pfnSpectatorDisconnect()		Called when spectator leaves the server
	mm_SpectatorThink,			// pfnSpectatorThink()			Called when spectator sends a command packet (usercmd_t)

	mm_Sys_Error,				// pfnSys_Error()			Notify game .dll that engine is going to shut down.  Allows mod authors to set a breakpoint.  SDK2

	mm_PM_Move,				// pfnPM_Move()				(wd) SDK2
	mm_PM_Init,				// pfnPM_Init()				Server version of player movement initialization; (wd) SDK2
	mm_PM_FindTextureType,			// pfnPM_FindTextureType()		(wd) SDK2

	mm_SetupVisibility,			// pfnSetupVisibility()			Set up PVS and PAS for networking for this client; (wd) SDK2
	mm_UpdateClientData,			// pfnUpdateClientData()		Set up data sent only to specific client; (wd) SDK2
	mm_AddToFullPack,			// pfnAddToFullPack()			(wd) SDK2
	mm_CreateBaseline,			// pfnCreateBaseline()			Tweak entity baseline for network encoding, allows setup of player baselines, too.; (wd) SDK2
	mm_RegisterEncoders,			// pfnRegisterEncoders()		Callbacks for network encoding; (wd) SDK2
	mm_GetWeaponData,			// pfnGetWeaponData()			(wd) SDK2
	mm_CmdStart,				// pfnCmdStart()			(wd) SDK2
	mm_CmdEnd,				// pfnCmdEnd()				(wd) SDK2
	mm_ConnectionlessPacket,		// pfnConnectionlessPacket()		(wd) SDK2
	mm_GetHullBounds,			// pfnGetHullBounds()			(wd) SDK2
	mm_CreateInstancedBaselines,		// pfnCreateInstancedBaselines()	(wd) SDK2
	mm_InconsistentFile,			// pfnInconsistentFile()		(wd) SDK2
	mm_AllowLagCompensation,		// pfnAllowLagCompensation()		(wd) SDK2
};

NEW_DLL_FUNCTIONS sNewFunctionTable =
{
	&mm_OnFreeEntPrivateData,	// pfnOnFreeEntPrivateData() Called right before the object's memory is freed. Calls its destructor.
	&mm_GameShutdown,			// pfnGameShutdown()
	&mm_ShouldCollide,			// pfnShouldCollide()
	&mm_CvarValue,				// pfnCvarValue()  (fz) Use mm_CvarValue2 instead
	&mm_CvarValue2				// pfnCvarValue2() (fz) When pfnQueryClientCvarValue2() completes it will call
								// pfnCvarValue2() with the request ID supplied earlier, the name of the cvar requested and the value of that cvar.
};

DLL_FUNCTIONS *pHookedDllFunctions = &sFunctionTable;
NEW_DLL_FUNCTIONS *pHookedNewDllFunctions = &sNewFunctionTable;

// It's not clear what the difference is between GetAPI and GetAPI2; they
// both appear to return the exact same function table.
//
// Only one of them appears to be ever called, though.  If the DLL provides
// GetAPI2, the engine/hlds will call that, and will not call GetAPI.  If
// the engine couldn't find GetAPI2 in the DLL, it appears to fall back to
// GetAPI.
//
// So, GetAPI2 appears to replace GetAPI, and appears to have been added
// with SDK 2.0.  My best guess is that, with the new SDK, interface
// version checking became important, and without the int ptr used in
// GetAPI2, the engine can't find out the version of the DLL via GetAPI.
//
// It's unclear whether a DLL coded under SDK2 needs to provide the older
// GetAPI or not..
C_DLLEXPORT int GetEntityAPI(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion)
{
	META_DEBUG(3, "called: GetEntityAPI; version=%d", interfaceVersion);
	if (!pFunctionTable) {
		META_ERROR("GetEntityAPI called with null pFunctionTable");
		return FALSE;
	}
	if (interfaceVersion != INTERFACE_VERSION) {
		META_ERROR("GetEntityAPI version mismatch; requested=%d ours=%d", interfaceVersion, INTERFACE_VERSION);
		return FALSE;
	}

	Q_memcpy(pFunctionTable, &sFunctionTable, sizeof(DLL_FUNCTIONS));
	return TRUE;
}

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	META_DEBUG(3, "called: GetEntityAPI2; version=%d", *interfaceVersion);

	if (!pFunctionTable) {
		META_ERROR("GetEntityAPI2 called with null pFunctionTable");
		return FALSE;
	}
	if (*interfaceVersion != INTERFACE_VERSION) {
		META_ERROR("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return FALSE;
	}

	Q_memcpy(pFunctionTable, &sFunctionTable, sizeof(DLL_FUNCTIONS));
	return TRUE;
}

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion)
{
	META_DEBUG(6, "called: GetNewDLLFunctions; version=%d", *interfaceVersion);

#if 0
	// ~dvander - but then you can't use cvar querying on many mods...
	// Don't provide these functions to engine if gamedll doesn't provide
	// them. Otherwise, we're in the position of having to provide answers
	// we can't necessarily provide (for instance, ShouldCollide())...
	if (!GameDLL.funcs.newapi_table)
		return FALSE;
#endif

	if (!pNewFunctionTable) {
		META_ERROR("GetNewDLLFunctions called with null pNewFunctionTable");
		return FALSE;
	}
	if (*interfaceVersion != NEW_DLL_FUNCTIONS_VERSION) {
		META_ERROR("GetNewDLLFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return FALSE;
	}

	g_meta_extdll.load();
	Q_memcpy(pNewFunctionTable, &sNewFunctionTable, sizeof(NEW_DLL_FUNCTIONS));
	return TRUE;
}

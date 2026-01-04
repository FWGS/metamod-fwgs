#include "dllapi_helper_macros.h"

static server_physics_api_t g_engine_physfuncs;

static void mm_LinkEdict(edict_t *ent, qboolean touch_triggers)
{
	g_engine_physfuncs.pfnLinkEdict(ent, touch_triggers);
}

static double mm_GetServerTime()
{
	return g_engine_physfuncs.pfnGetServerTime();
}

static double mm_GetFrameTime()
{
	return g_engine_physfuncs.pfnGetFrameTime();
}

static void* mm_GetModel(int modelindex)
{
	if (g_engine_physfuncs.pfnGetModel)
		return g_engine_physfuncs.pfnGetModel(modelindex);
	return nullptr;
}

static areanode_t* mm_GetHeadnode(void)
{
	if (g_engine_physfuncs.pfnGetHeadnode)
		return g_engine_physfuncs.pfnGetHeadnode();
	return nullptr;
}

static int mm_ServerState(void)
{
	if (g_engine_physfuncs.pfnServerState)
		return g_engine_physfuncs.pfnServerState();
	return SERVER_DEAD;
}

static void mm_Host_Error(const char *error, ...) {
	if (!g_engine_physfuncs.pfnHost_Error) {
		va_list args;
		va_start(args, error);
		vfprintf(stderr, error, args);
		va_end(args);
		return;
	}

	va_list args;
	va_start(args, error);
	char buf[4096];
	Q_vsnprintf(buf, sizeof(buf), error, args);
	va_end(args);
	g_engine_physfuncs.pfnHost_Error("%s", buf);
}

static int mm_DrawConsoleString(int x, int y, char *string)
{
	if (g_engine_physfuncs.pfnDrawConsoleString)
		return g_engine_physfuncs.pfnDrawConsoleString(x, y, string);
	return 0;
}

static void mm_DrawSetTextColor(float r, float g, float b)
{
	if (g_engine_physfuncs.pfnDrawSetTextColor)
		g_engine_physfuncs.pfnDrawSetTextColor(r, g, b);
}

static void mm_DrawConsoleStringLen(const char *string, int *length, int *height)
{
	if (g_engine_physfuncs.pfnDrawConsoleStringLen)
		g_engine_physfuncs.pfnDrawConsoleStringLen(string, length, height);
	else {
		if (length) *length = string ? static_cast<int>(strlen(string)) : 0;
		if (height) *height = 8;
	}
}

static void mm_Con_NPrintf(int pos, const char *fmt, ...) {
	if (!g_engine_physfuncs.Con_NPrintf) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
		return;
	}

	va_list args;
	va_start(args, fmt);
	char buf[4096];
	Q_vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	g_engine_physfuncs.Con_NPrintf(pos, "%s", buf);
}

static void mm_Con_NXPrintf(struct con_nprint_s *info, const char *fmt, ...) {
	if (!g_engine_physfuncs.Con_NXPrintf) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
		return;
	}

	va_list args;
	va_start(args, fmt);
	char buf[4096];
	Q_vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	g_engine_physfuncs.Con_NXPrintf(info, "%s", buf);
}

static const char* mm_GetLightStyle(int style)
{
	if (g_engine_physfuncs.pfnGetLightStyle)
		return g_engine_physfuncs.pfnGetLightStyle(style);
	return "";
}

static void mm_UpdateFogSettings(unsigned int packed_fog)
{
	if (g_engine_physfuncs.pfnUpdateFogSettings)
		g_engine_physfuncs.pfnUpdateFogSettings(packed_fog);
}

static char** mm_GetFilesList(const char *pattern, int *numFiles, int gamedironly)
{
	if (g_engine_physfuncs.pfnGetFilesList)
		return g_engine_physfuncs.pfnGetFilesList(pattern, numFiles, gamedironly);

	if (numFiles)
		*numFiles = 0;

	return nullptr;
}

static msurface_s* mm_TraceSurface(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	if (g_engine_physfuncs.pfnTraceSurface)
		return g_engine_physfuncs.pfnTraceSurface(pTextureEntity, v1, v2);
	return nullptr;
}

static const byte* mm_GetTextureData(unsigned int texnum)
{
	if (g_engine_physfuncs.pfnGetTextureData)
		return g_engine_physfuncs.pfnGetTextureData(texnum);
	return nullptr;
}

static void* mm_MemAlloc(size_t cb, const char *filename, const int fileline)
{
	if (g_engine_physfuncs.pfnMemAlloc)
		return g_engine_physfuncs.pfnMemAlloc(cb, filename, fileline);
	return nullptr;
}

static void mm_MemFree(void *mem, const char *filename, const int fileline)
{
	if (g_engine_physfuncs.pfnMemFree)
		g_engine_physfuncs.pfnMemFree(mem, filename, fileline);
}

static int mm_MaskPointContents(const float *pos, int groupmask)
{
	if (g_engine_physfuncs.pfnMaskPointContents)
		return g_engine_physfuncs.pfnMaskPointContents(pos, groupmask);
	return 0;
}

static trace_t mm_Trace(const float *p0, float *mins, float *maxs, const float *p1, int type, edict_t *e)
{
	if (g_engine_physfuncs.pfnTrace)
		return g_engine_physfuncs.pfnTrace(p0, mins, maxs, p1, type, e);

	trace_t tr = {};
	tr.fraction = 1.0f;
	tr.allsolid = false;
	tr.startsolid = false;
	tr.inopen = true;
	tr.inwater = false;
	return tr;
}

static trace_t mm_TraceNoEnts(const float *p0, float *mins, float *maxs, const float *p1, int type, edict_t *e)
{
	if (g_engine_physfuncs.pfnTraceNoEnts)
		return g_engine_physfuncs.pfnTraceNoEnts(p0, mins, maxs, p1, type, e);
	return mm_Trace(p0, mins, maxs, p1, type, e);
}

static int mm_BoxInPVS(const float *org, const float *boxmins, const float *boxmaxs)
{
	if (g_engine_physfuncs.pfnBoxInPVS)
		return g_engine_physfuncs.pfnBoxInPVS(org, boxmins, boxmaxs);
	return 1;
}

static void mm_WriteBytes(const byte *bytes, int count)
{
	if (g_engine_physfuncs.pfnWriteBytes)
		g_engine_physfuncs.pfnWriteBytes(bytes, count);
}

static int mm_CheckLump(const char *filename, const int lump, int *lumpsize)
{
	if (g_engine_physfuncs.pfnCheckLump)
		return g_engine_physfuncs.pfnCheckLump(filename, lump, lumpsize);
	return LUMP_LOAD_NOT_EXIST;
}

static int mm_ReadLump(const char *filename, const int lump, void **lumpdata, int *lumpsize)
{
	if (g_engine_physfuncs.pfnReadLump)
		return g_engine_physfuncs.pfnReadLump(filename, lump, lumpdata, lumpsize);
	if (lumpdata) *lumpdata = nullptr;
	if (lumpsize) *lumpsize = 0;
	return LUMP_LOAD_NOT_EXIST;
}

static int mm_SaveLump(const char *filename, const int lump, void *lumpdata, int lumpsize)
{
	if (g_engine_physfuncs.pfnSaveLump)
		return g_engine_physfuncs.pfnSaveLump(filename, lump, lumpdata, lumpsize);
	return LUMP_SAVE_COULDNT_OPEN;
}

static int mm_SaveFile(const char *filename, const void *data, int len)
{
	if (g_engine_physfuncs.pfnSaveFile)
		return g_engine_physfuncs.pfnSaveFile(filename, data, len);
	return -1;
}

static const byte* mm_LoadImagePixels(const char *filename, int *width, int *height)
{
	if (g_engine_physfuncs.pfnLoadImagePixels)
		return g_engine_physfuncs.pfnLoadImagePixels(filename, width, height);
	if (width) *width = 0;
	if (height) *height = 0;
	return nullptr;
}

static const char* mm_GetModelName(int modelindex)
{
	if (g_engine_physfuncs.pfnGetModelName)
		return g_engine_physfuncs.pfnGetModelName(modelindex);
	return nullptr;
}

static void* mm_GetNativeObject(const char *object)
{
	if (g_engine_physfuncs.pfnGetNativeObject)
		return g_engine_physfuncs.pfnGetNativeObject(object);
	return nullptr;
}

server_physics_api_t g_meta_physfuncs = {
	mm_LinkEdict,
	mm_GetServerTime,
	mm_GetFrameTime,
	mm_GetModel,
	mm_GetHeadnode,
	mm_ServerState,
	mm_Host_Error,
	nullptr, // TriAPI aren't available on server-side
	mm_DrawConsoleString,
	mm_DrawSetTextColor,
	mm_DrawConsoleStringLen,
	mm_Con_NPrintf,
	mm_Con_NXPrintf,
	mm_GetLightStyle,
	mm_UpdateFogSettings,
	mm_GetFilesList,
	mm_TraceSurface,
	mm_GetTextureData,
	mm_MemAlloc,
	mm_MemFree,
	mm_MaskPointContents,
	mm_Trace,
	mm_TraceNoEnts,
	mm_BoxInPVS,
	mm_WriteBytes,
	mm_CheckLump,
	mm_ReadLump,
	mm_SaveLump,
	mm_SaveFile,
	mm_LoadImagePixels,
	mm_GetModelName,
	mm_GetNativeObject
};

static int mm_SV_CreateEntity(edict_t *pent, const char *szName)
{
	return g_GameDLL.funcs.physint_table->SV_CreateEntity(pent, szName);
}

static int mm_SV_PhysicsEntity(edict_t *pEntity)
{
	return g_GameDLL.funcs.physint_table->SV_PhysicsEntity(pEntity);
}

static int mm_SV_LoadEntities(const char *mapname, char *entities)
{
	if (g_GameDLL.funcs.physint_table->SV_LoadEntities)
		return g_GameDLL.funcs.physint_table->SV_LoadEntities(mapname, entities);
	return 0;
}

static void mm_SV_UpdatePlayerBaseVelocity(edict_t *ent)
{
	if (g_GameDLL.funcs.physint_table->SV_UpdatePlayerBaseVelocity)
		g_GameDLL.funcs.physint_table->SV_UpdatePlayerBaseVelocity(ent);
}

static int mm_SV_AllowSaveGame(void)
{
	if (g_GameDLL.funcs.physint_table->SV_AllowSaveGame)
		return g_GameDLL.funcs.physint_table->SV_AllowSaveGame();
	return 1;
}

static int mm_SV_TriggerTouch(edict_t *pent, edict_t *trigger)
{
	if (g_GameDLL.funcs.physint_table->SV_TriggerTouch)
		return g_GameDLL.funcs.physint_table->SV_TriggerTouch(pent, trigger);
	return 0;
}

static unsigned int mm_SV_CheckFeatures(void)
{
	if (g_GameDLL.funcs.physint_table->SV_CheckFeatures)
		return g_GameDLL.funcs.physint_table->SV_CheckFeatures();
	return 0;
}

static void mm_DrawDebugTriangles(void)
{
	if (g_GameDLL.funcs.physint_table->DrawDebugTriangles)
		g_GameDLL.funcs.physint_table->DrawDebugTriangles();
}

static void mm_DrawNormalTriangles(void)
{
	if (g_GameDLL.funcs.physint_table->DrawNormalTriangles)
		g_GameDLL.funcs.physint_table->DrawNormalTriangles();
}

static void mm_DrawOrthoTriangles(void)
{
	if (g_GameDLL.funcs.physint_table->DrawOrthoTriangles)
		g_GameDLL.funcs.physint_table->DrawOrthoTriangles();
}

static void mm_ClipMoveToEntity(edict_t *ent, const float *start, float *mins, float *maxs, const float *end, trace_t *trace)
{
	if (g_GameDLL.funcs.physint_table->ClipMoveToEntity)
		g_GameDLL.funcs.physint_table->ClipMoveToEntity(ent, start, mins, maxs, end, trace);
}

static void mm_ClipPMoveToEntity(struct physent_s *pe, const float *start, float *mins, float *maxs, const float *end, struct pmtrace_s *tr)
{
	if (g_GameDLL.funcs.physint_table->ClipPMoveToEntity)
		g_GameDLL.funcs.physint_table->ClipPMoveToEntity(pe, start, mins, maxs, end, tr);
}

static void mm_SV_EndFrame(void)
{
	if (g_GameDLL.funcs.physint_table->SV_EndFrame)
		g_GameDLL.funcs.physint_table->SV_EndFrame();
}

static void mm_PrepWorldFrame(void)
{
	if (g_GameDLL.funcs.physint_table->pfnPrepWorldFrame)
		g_GameDLL.funcs.physint_table->pfnPrepWorldFrame();
}

static void mm_CreateEntitiesInRestoreList(SAVERESTOREDATA *pSaveData, int levelMask, qboolean create_world)
{
	if (g_GameDLL.funcs.physint_table->pfnCreateEntitiesInRestoreList)
		g_GameDLL.funcs.physint_table->pfnCreateEntitiesInRestoreList(pSaveData, levelMask, create_world);
}

static string_t mm_AllocString(const char *szValue)
{
	if (g_GameDLL.funcs.physint_table->pfnAllocString)
		return g_GameDLL.funcs.physint_table->pfnAllocString(szValue);
	return 0;
}

static string_t mm_MakeString(const char *szValue)
{
	if (g_GameDLL.funcs.physint_table->pfnMakeString)
		return g_GameDLL.funcs.physint_table->pfnMakeString(szValue);
	return 0;
}

static const char* mm_GetString(string_t iString)
{
	if (g_GameDLL.funcs.physint_table->pfnGetString)
		return g_GameDLL.funcs.physint_table->pfnGetString(iString);
	return "";
}

static int mm_RestoreDecal(struct decallist_s *entry, edict_t *pEdict, qboolean adjacent)
{
	if (g_GameDLL.funcs.physint_table->pfnRestoreDecal)
		return g_GameDLL.funcs.physint_table->pfnRestoreDecal(entry, pEdict, adjacent);
	return 0;
}

static void mm_PM_PlayerTouch(struct playermove_s *ppmove, edict_t *client)
{
	if (g_GameDLL.funcs.physint_table->PM_PlayerTouch)
		g_GameDLL.funcs.physint_table->PM_PlayerTouch(ppmove, client);
}

static void mm_Mod_ProcessUserData(struct model_s *mod, qboolean create, const byte *buffer)
{
	if (g_GameDLL.funcs.physint_table->Mod_ProcessUserData)
		g_GameDLL.funcs.physint_table->Mod_ProcessUserData(mod, create, buffer);
}

static void* mm_SV_HullForBsp(edict_t *ent, const float *mins, const float *maxs, float *offset)
{
	if (g_GameDLL.funcs.physint_table->SV_HullForBsp)
		return g_GameDLL.funcs.physint_table->SV_HullForBsp(ent, mins, maxs, offset);
	return nullptr;
}

static int mm_SV_PlayerThink(edict_t *ent, float frametime, double time)
{
	if (g_GameDLL.funcs.physint_table->SV_PlayerThink)
		return g_GameDLL.funcs.physint_table->SV_PlayerThink(ent, frametime, time);
	return 0;
}

static physics_interface_t g_meta_physint = {
	SV_PHYSICS_INTERFACE_VERSION,
	mm_SV_CreateEntity,
	mm_SV_PhysicsEntity,
	mm_SV_LoadEntities,
	mm_SV_UpdatePlayerBaseVelocity,
	mm_SV_AllowSaveGame,
	mm_SV_TriggerTouch,
	mm_SV_CheckFeatures,
	mm_DrawDebugTriangles,
	mm_DrawNormalTriangles,
	mm_DrawOrthoTriangles,
	mm_ClipMoveToEntity,
	mm_ClipPMoveToEntity,
	mm_SV_EndFrame,
	mm_PrepWorldFrame,
	mm_CreateEntitiesInRestoreList,
	mm_AllocString,
	mm_MakeString,
	mm_GetString,
	mm_RestoreDecal,
	mm_PM_PlayerTouch,
	mm_Mod_ProcessUserData,
	mm_SV_HullForBsp,
	mm_SV_PlayerThink
};

C_DLLEXPORT int Server_GetPhysicsInterface(int iVersion, server_physics_api_t *pfuncsFromEngine, physics_interface_t *pFunctionTable)
{
	if (iVersion != SV_PHYSICS_INTERFACE_VERSION || pfuncsFromEngine == nullptr || pFunctionTable == nullptr)
		return FALSE;

	if (!g_GameDLL.funcs.physint_table)
	{
		// engine always require for nullptr, only replace single function needed for linkent alternative
		Q_memset(pFunctionTable, 0x0, sizeof(*pFunctionTable));
		pFunctionTable->SV_CreateEntity = [](edict_t *pent, const char *szName)
		{
			// check if gamedll implements this entity
			ENTITY_FN SpawnEdict = reinterpret_cast<ENTITY_FN>(g_GameDLL.sys_module.getsym(szName));

			// should we check metamod module itself? engine will do GPA on metamod module before failing back to this call anyway
			if (!SpawnEdict)
				return -1; // failed

			SpawnEdict(&pent->v);
			return 0; // handled
		};
	}
	else
	{
		// copy engine functions, we need proxy calls to them
		Q_memcpy(&g_engine_physfuncs, pfuncsFromEngine, sizeof(g_engine_physfuncs));

		// TODO: provide physint to plugins, but for now just transparently proxy everything
		Q_memcpy(pFunctionTable, &g_meta_physint, sizeof(*pFunctionTable));
	}
	return TRUE;
}

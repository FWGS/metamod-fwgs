#include "precompiled.h"

static void MM_PRE_HOOK mm_QueryClientCvarValue(const edict_t* pEdict, const char* cvarName)
{
	g_players.set_player_cvar_query(pEdict, cvarName);
	meta_call_void(&enginefuncs_t::pfnQueryClientCvarValue, engineapi_target(g_engineapi_info.pfnQueryClientCvarValue), pEdict, cvarName);
}

static int MM_POST_HOOK mm_RegUserMsg(const char *pszName, int iSize)
{
	int imsgid;
	MRegMsg *nmsg = nullptr;

	// meta_call() handles the whole api chain and returns the final value
	// (override or orig) that the old RETURN_API() macro would have returned.
	imsgid = meta_call<int>(&enginefuncs_t::pfnRegUserMsg,
			engineapi_target(g_engineapi_info.pfnRegUserMsg), 0, pszName, iSize);

	// Add the msgid, name, and size to our saved list, if we haven't
	// already.
	nmsg = g_regMsgs->find(imsgid);
	if (nmsg)
	{
		if (FStrEq(pszName, nmsg->getname()))
			// This name/msgid pair was already registered.
			META_DEBUG(3, "user message registered again: name=%s, msgid=%d", pszName, imsgid);
		else
			// This msgid was previously used by a different message name.
			META_ERROR("user message id reused: msgid=%d, oldname=%s, newname=%s", imsgid, nmsg->getname(), pszName);
	}
	else
		g_regMsgs->add(pszName, imsgid, iSize);

	return imsgid;
}

//----------------------------------------------------------------------------------------------------------------------

static int mm_PrecacheModel(const char *s)
{
	return meta_call<int>(&enginefuncs_t::pfnPrecacheModel, engineapi_target(g_engineapi_info.pfnPrecacheModel), 0, s);
}

static int mm_PrecacheSound(const char *s)
{
	return meta_call<int>(&enginefuncs_t::pfnPrecacheSound, engineapi_target(g_engineapi_info.pfnPrecacheSound), 0, s);
}

static void mm_SetModel(edict_t *e, const char *m)
{
	meta_call_void(&enginefuncs_t::pfnSetModel, engineapi_target(g_engineapi_info.pfnSetModel), e, m);
}

static int mm_ModelIndex(const char *m)
{
	return meta_call<int>(&enginefuncs_t::pfnModelIndex, engineapi_target(g_engineapi_info.pfnModelIndex), 0, m);
}

static int mm_ModelFrames(int modelIndex)
{
	return meta_call<int>(&enginefuncs_t::pfnModelFrames, engineapi_target(g_engineapi_info.pfnModelFrames), 0, modelIndex);
}

static void mm_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	meta_call_void(&enginefuncs_t::pfnSetSize, engineapi_target(g_engineapi_info.pfnSetSize), e, rgflMin, rgflMax);
}

static void mm_ChangeLevel(const char *s1, const char *s2)
{
	meta_call_void(&enginefuncs_t::pfnChangeLevel, engineapi_target(g_engineapi_info.pfnChangeLevel), s1, s2);
}

static void mm_GetSpawnParms(edict_t *ent)
{
	meta_call_void(&enginefuncs_t::pfnGetSpawnParms, engineapi_target(g_engineapi_info.pfnGetSpawnParms), ent);
}

static void mm_SaveSpawnParms(edict_t *ent)
{
	meta_call_void(&enginefuncs_t::pfnSaveSpawnParms, engineapi_target(g_engineapi_info.pfnSaveSpawnParms), ent);
}

static float mm_VecToYaw(const float *rgflVector)
{
	return meta_call<float>(&enginefuncs_t::pfnVecToYaw, engineapi_target(g_engineapi_info.pfnVecToYaw), 0.0, rgflVector);
}

static void mm_VecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
	meta_call_void(&enginefuncs_t::pfnVecToAngles, engineapi_target(g_engineapi_info.pfnVecToAngles), rgflVectorIn, rgflVectorOut);
}

static void mm_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	meta_call_void(&enginefuncs_t::pfnMoveToOrigin, engineapi_target(g_engineapi_info.pfnMoveToOrigin), ent, pflGoal, dist, iMoveType);
}

static void mm_ChangeYaw(edict_t *ent)
{
	meta_call_void(&enginefuncs_t::pfnChangeYaw, engineapi_target(g_engineapi_info.pfnChangeYaw), ent);
}

static void mm_ChangePitch(edict_t *ent)
{
	meta_call_void(&enginefuncs_t::pfnChangePitch, engineapi_target(g_engineapi_info.pfnChangePitch), ent);
}

static edict_t *mm_FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnFindEntityByString, engineapi_target(g_engineapi_info.pfnFindEntityByString), NULL, pEdictStartSearchAfter, pszField, pszValue);
}

static int mm_GetEntityIllum(edict_t *pEnt)
{
	return meta_call<int>(&enginefuncs_t::pfnGetEntityIllum, engineapi_target(g_engineapi_info.pfnGetEntityIllum), 0, pEnt);
}

static edict_t *mm_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnFindEntityInSphere, engineapi_target(g_engineapi_info.pfnFindEntityInSphere), NULL, pEdictStartSearchAfter, org, rad);
}

static edict_t *mm_FindClientInPVS(edict_t *pEdict)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnFindClientInPVS, engineapi_target(g_engineapi_info.pfnFindClientInPVS), NULL, pEdict);
}

static edict_t *mm_EntitiesInPVS(edict_t *pplayer)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnEntitiesInPVS, engineapi_target(g_engineapi_info.pfnEntitiesInPVS), NULL, pplayer);
}

static void mm_MakeVectors(const float *rgflVector)
{
	meta_call_void(&enginefuncs_t::pfnMakeVectors, engineapi_target(g_engineapi_info.pfnMakeVectors), rgflVector);
}

static void mm_AngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
	meta_call_void(&enginefuncs_t::pfnAngleVectors, engineapi_target(g_engineapi_info.pfnAngleVectors), rgflVector, forward, right, up);
}

static edict_t *mm_CreateEntity()
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnCreateEntity, engineapi_target(g_engineapi_info.pfnCreateEntity), NULL);
}

static void mm_RemoveEntity(edict_t *e)
{
	meta_call_void(&enginefuncs_t::pfnRemoveEntity, engineapi_target(g_engineapi_info.pfnRemoveEntity), e);
}

static edict_t *mm_CreateNamedEntity(int className)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnCreateNamedEntity, engineapi_target(g_engineapi_info.pfnCreateNamedEntity), NULL, className);
}

static void mm_MakeStatic(edict_t *ent)
{
	meta_call_void(&enginefuncs_t::pfnMakeStatic, engineapi_target(g_engineapi_info.pfnMakeStatic), ent);
}

static int mm_EntIsOnFloor(edict_t *e)
{
	return meta_call<int>(&enginefuncs_t::pfnEntIsOnFloor, engineapi_target(g_engineapi_info.pfnEntIsOnFloor), 0, e);
}

static int mm_DropToFloor(edict_t *e)
{
	return meta_call<int>(&enginefuncs_t::pfnDropToFloor, engineapi_target(g_engineapi_info.pfnDropToFloor), 0, e);
}

static int mm_WalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
	return meta_call<int>(&enginefuncs_t::pfnWalkMove, engineapi_target(g_engineapi_info.pfnWalkMove), 0, ent, yaw, dist, iMode);
}

static void mm_SetOrigin(edict_t *e, const float *rgflOrigin)
{
	meta_call_void(&enginefuncs_t::pfnSetOrigin, engineapi_target(g_engineapi_info.pfnSetOrigin), e, rgflOrigin);
}

static void mm_EmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
	meta_call_void(&enginefuncs_t::pfnEmitSound, engineapi_target(g_engineapi_info.pfnEmitSound), entity, channel, sample, volume, attenuation, fFlags, pitch);
}

static void mm_EmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	meta_call_void(&enginefuncs_t::pfnEmitAmbientSound, engineapi_target(g_engineapi_info.pfnEmitAmbientSound), entity, pos, samp, vol, attenuation, fFlags, pitch);
}

static void mm_TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	meta_call_void(&enginefuncs_t::pfnTraceLine, engineapi_target(g_engineapi_info.pfnTraceLine), v1, v2, fNoMonsters, pentToSkip, ptr);
}

static void mm_TraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
	meta_call_void(&enginefuncs_t::pfnTraceToss, engineapi_target(g_engineapi_info.pfnTraceToss), pent, pentToIgnore, ptr);
}

static int mm_TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	return meta_call<int>(&enginefuncs_t::pfnTraceMonsterHull, engineapi_target(g_engineapi_info.pfnTraceMonsterHull), 0, pEdict, v1, v2, fNoMonsters, pentToSkip, ptr);
}

static void mm_TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
	meta_call_void(&enginefuncs_t::pfnTraceHull, engineapi_target(g_engineapi_info.pfnTraceHull), v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr);
}

static void mm_TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
	meta_call_void(&enginefuncs_t::pfnTraceModel, engineapi_target(g_engineapi_info.pfnTraceModel), v1, v2, hullNumber, pent, ptr);
}

static const char *mm_TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	return meta_call<const char *>(&enginefuncs_t::pfnTraceTexture, engineapi_target(g_engineapi_info.pfnTraceTexture), NULL, pTextureEntity, v1, v2);
}

static void mm_TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
	meta_call_void(&enginefuncs_t::pfnTraceSphere, engineapi_target(g_engineapi_info.pfnTraceSphere), v1, v2, fNoMonsters, radius, pentToSkip, ptr);
}

static void mm_GetAimVector(edict_t *ent, float speed, float *rgflReturn)
{
	meta_call_void(&enginefuncs_t::pfnGetAimVector, engineapi_target(g_engineapi_info.pfnGetAimVector), ent, speed, rgflReturn);
}

static void mm_ServerCommand(const char *str)
{
	meta_call_void(&enginefuncs_t::pfnServerCommand, engineapi_target(g_engineapi_info.pfnServerCommand), str);
}

static void mm_ServerExecute()
{
	meta_call_void(&enginefuncs_t::pfnServerExecute, engineapi_target(g_engineapi_info.pfnServerExecute));
}

static void mm_engClientCommand(edict_t *pEdict, const char *szFmt, ...)
{
	// Format the varargs first; plugins and the engine receive the command
	// as a single pre-formatted "%s" string.
	const api_info_t &info = g_engineapi_info.pfnClientCommand;
	META_DEBUG(info.loglevel, "In %s: fmt=%s", info.name, szFmt);

	char buf[MAX_STRBUF_LEN];
	va_list ap;
	va_start(ap, szFmt);
	Q_vsnprintf(buf, sizeof(buf), szFmt, ap);
	va_end(ap);

	meta_call_void(&enginefuncs_t::pfnClientCommand, engineapi_target(info), pEdict, "%s", buf);
}

static void mm_ParticleEffect(const float *org, const float *dir, float color, float count)
{
	meta_call_void(&enginefuncs_t::pfnParticleEffect, engineapi_target(g_engineapi_info.pfnParticleEffect), org, dir, color, count);
}

static void mm_LightStyle(int style, const char *val)
{
	meta_call_void(&enginefuncs_t::pfnLightStyle, engineapi_target(g_engineapi_info.pfnLightStyle), style, val);
}

static int mm_DecalIndex(const char *name)
{
	return meta_call<int>(&enginefuncs_t::pfnDecalIndex, engineapi_target(g_engineapi_info.pfnDecalIndex), 0, name);
}

static int mm_PointContents(const float *rgflVector)
{
	return meta_call<int>(&enginefuncs_t::pfnPointContents, engineapi_target(g_engineapi_info.pfnPointContents), 0, rgflVector);
}

static void mm_MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	meta_call_void(&enginefuncs_t::pfnMessageBegin, engineapi_target(g_engineapi_info.pfnMessageBegin), msg_dest, msg_type, pOrigin, ed);
}

static void mm_MessageEnd()
{
	meta_call_void(&enginefuncs_t::pfnMessageEnd, engineapi_target(g_engineapi_info.pfnMessageEnd));
}

static void mm_WriteByte(int iValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteByte, engineapi_target(g_engineapi_info.pfnWriteByte), iValue);
}

static void mm_WriteChar(int iValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteChar, engineapi_target(g_engineapi_info.pfnWriteChar), iValue);
}

static void mm_WriteShort(int iValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteShort, engineapi_target(g_engineapi_info.pfnWriteShort), iValue);
}

static void mm_WriteLong(int iValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteLong, engineapi_target(g_engineapi_info.pfnWriteLong), iValue);
}

static void mm_WriteAngle(float flValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteAngle, engineapi_target(g_engineapi_info.pfnWriteAngle), flValue);
}

static void mm_WriteCoord(float flValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteCoord, engineapi_target(g_engineapi_info.pfnWriteCoord), flValue);
}

static void mm_WriteString(const char *sz)
{
	meta_call_void(&enginefuncs_t::pfnWriteString, engineapi_target(g_engineapi_info.pfnWriteString), sz);
}

static void mm_WriteEntity(int iValue)
{
	meta_call_void(&enginefuncs_t::pfnWriteEntity, engineapi_target(g_engineapi_info.pfnWriteEntity), iValue);
}

static void mm_CVarRegister(cvar_t *pCvar)
{
	meta_call_void(&enginefuncs_t::pfnCVarRegister, engineapi_target(g_engineapi_info.pfnCVarRegister), pCvar);
}

static float mm_CVarGetFloat(const char *szVarName)
{
	return meta_call<float>(&enginefuncs_t::pfnCVarGetFloat, engineapi_target(g_engineapi_info.pfnCVarGetFloat), 0.0, szVarName);
}

static const char *mm_CVarGetString(const char *szVarName)
{
	return meta_call<const char *>(&enginefuncs_t::pfnCVarGetString, engineapi_target(g_engineapi_info.pfnCVarGetString), NULL, szVarName);
}

static void mm_CVarSetFloat(const char *szVarName, float flValue)
{
	meta_call_void(&enginefuncs_t::pfnCVarSetFloat, engineapi_target(g_engineapi_info.pfnCVarSetFloat), szVarName, flValue);
}

static void mm_CVarSetString(const char *szVarName, const char *szValue)
{
	meta_call_void(&enginefuncs_t::pfnCVarSetString, engineapi_target(g_engineapi_info.pfnCVarSetString), szVarName, szValue);
}

static void mm_AlertMessage(ALERT_TYPE atype, const char *szFmt, ...)
{
	// Format the varargs first, as the old varargs macros did; plugins and
	// the engine receive the message as a single pre-formatted "%s" string.
	const api_info_t &info = g_engineapi_info.pfnAlertMessage;
	META_DEBUG(info.loglevel, "In %s: fmt=%s", info.name, szFmt);

	char buf[MAX_STRBUF_LEN];
	va_list ap;
	va_start(ap, szFmt);
	int len = Q_vsnprintf(buf, sizeof(buf), szFmt, ap) + 1;
	va_end(ap);

#ifndef UNFINISHED
	(void)len;
#else
	// pass logmsg string to log parsing thread
	/// qmsg = Q_strdup(buf);
	char *qmsg = (char *)Q_malloc(len * sizeof(char));
	if (!qmsg)
		META_ERROR("malloc failed for logmsg to thread queue");
	else
	{
		STRNCPY(qmsg, buf, len);
		LogQueue->push(qmsg);
	}
#endif // UNFINISHED

	meta_call_void(&enginefuncs_t::pfnAlertMessage, engineapi_target(info), atype, "%s", buf);
}

static void mm_EngineFprintf(void *pfile, const char *szFmt, ...)
{
	// Format the varargs first; plugins and the engine receive the message
	// as a single pre-formatted "%s" string.
	const api_info_t &info = g_engineapi_info.pfnEngineFprintf;
	META_DEBUG(info.loglevel, "In %s: fmt=%s", info.name, szFmt);

	char buf[MAX_STRBUF_LEN];
	va_list ap;
	va_start(ap, szFmt);
	Q_vsnprintf(buf, sizeof(buf), szFmt, ap);
	va_end(ap);

	meta_call_void(&enginefuncs_t::pfnEngineFprintf, engineapi_target(info), pfile, "%s", buf);
}

static void *mm_PvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
	return meta_call<void *>(&enginefuncs_t::pfnPvAllocEntPrivateData, engineapi_target(g_engineapi_info.pfnPvAllocEntPrivateData), NULL, pEdict, cb);
}

static void *mm_PvEntPrivateData(edict_t *pEdict)
{
	return meta_call<void *>(&enginefuncs_t::pfnPvEntPrivateData, engineapi_target(g_engineapi_info.pfnPvEntPrivateData), NULL, pEdict);
}

static void mm_FreeEntPrivateData(edict_t *pEdict)
{
	meta_call_void(&enginefuncs_t::pfnFreeEntPrivateData, engineapi_target(g_engineapi_info.pfnFreeEntPrivateData), pEdict);
}

static const char *mm_SzFromIndex(int iString)
{
	return meta_call<const char *>(&enginefuncs_t::pfnSzFromIndex, engineapi_target(g_engineapi_info.pfnSzFromIndex), NULL, iString);
}

static int mm_AllocString(const char *szValue)
{
	return meta_call<int>(&enginefuncs_t::pfnAllocString, engineapi_target(g_engineapi_info.pfnAllocString), 0, szValue);
}

static struct entvars_s *mm_GetVarsOfEnt(edict_t *pEdict)
{
	return meta_call<struct entvars_s *>(&enginefuncs_t::pfnGetVarsOfEnt, engineapi_target(g_engineapi_info.pfnGetVarsOfEnt), NULL, pEdict);
}

static edict_t *mm_PEntityOfEntOffset(int iEntOffset)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnPEntityOfEntOffset, engineapi_target(g_engineapi_info.pfnPEntityOfEntOffset), NULL, iEntOffset);
}

static int mm_EntOffsetOfPEntity(const edict_t *pEdict)
{
	return meta_call<int>(&enginefuncs_t::pfnEntOffsetOfPEntity, engineapi_target(g_engineapi_info.pfnEntOffsetOfPEntity), 0, pEdict);
}

static int mm_IndexOfEdict(const edict_t *pEdict)
{
	return meta_call<int>(&enginefuncs_t::pfnIndexOfEdict, engineapi_target(g_engineapi_info.pfnIndexOfEdict), 0, pEdict);
}

static edict_t *mm_PEntityOfEntIndex(int iEntIndex)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnPEntityOfEntIndex, engineapi_target(g_engineapi_info.pfnPEntityOfEntIndex), NULL, iEntIndex);
}

static edict_t *mm_FindEntityByVars(struct entvars_s *pvars)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnFindEntityByVars, engineapi_target(g_engineapi_info.pfnFindEntityByVars), NULL, pvars);
}

static void *mm_GetModelPtr(edict_t *pEdict)
{
	return meta_call<void *>(&enginefuncs_t::pfnGetModelPtr, engineapi_target(g_engineapi_info.pfnGetModelPtr), NULL, pEdict);
}

static void mm_AnimationAutomove(const edict_t *pEdict, float flTime)
{
	meta_call_void(&enginefuncs_t::pfnAnimationAutomove, engineapi_target(g_engineapi_info.pfnAnimationAutomove), pEdict, flTime);
}

static void mm_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles)
{
	meta_call_void(&enginefuncs_t::pfnGetBonePosition, engineapi_target(g_engineapi_info.pfnGetBonePosition), pEdict, iBone, rgflOrigin, rgflAngles);
}

static uint32 mm_FunctionFromName(const char *pName)
{
	return meta_call<uint32>(&enginefuncs_t::pfnFunctionFromName, engineapi_target(g_engineapi_info.pfnFunctionFromName), 0, pName);
}

static const char *mm_NameForFunction(uint32 function)
{
	return meta_call<const char *>(&enginefuncs_t::pfnNameForFunction, engineapi_target(g_engineapi_info.pfnNameForFunction), NULL, function);
}

// JOHN: engine callbacks so game DLL can print messages to individual clients
static void mm_ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
	meta_call_void(&enginefuncs_t::pfnClientPrintf, engineapi_target(g_engineapi_info.pfnClientPrintf), pEdict, ptype, szMsg);
}

static void mm_ServerPrint(const char *szMsg)
{
	meta_call_void(&enginefuncs_t::pfnServerPrint, engineapi_target(g_engineapi_info.pfnServerPrint), szMsg);
}

// these 3 added so game DLL can easily access client 'cmd' strings
static const char *mm_Cmd_Args()
{
	return meta_call<const char *>(&enginefuncs_t::pfnCmd_Args, engineapi_target(g_engineapi_info.pfnCmd_Args), NULL);
}

static const char *mm_Cmd_Argv(int argc)
{
	return meta_call<const char *>(&enginefuncs_t::pfnCmd_Argv, engineapi_target(g_engineapi_info.pfnCmd_Argv), NULL, argc);
}

static int mm_Cmd_Argc()
{
	return meta_call<int>(&enginefuncs_t::pfnCmd_Argc, engineapi_target(g_engineapi_info.pfnCmd_Argc), 0);
}

static void mm_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles)
{
	meta_call_void(&enginefuncs_t::pfnGetAttachment, engineapi_target(g_engineapi_info.pfnGetAttachment), pEdict, iAttachment, rgflOrigin, rgflAngles);
}

static void mm_CRC32_Init(CRC32_t *pulCRC)
{
	meta_call_void(&enginefuncs_t::pfnCRC32_Init, engineapi_target(g_engineapi_info.pfnCRC32_Init), pulCRC);
}
static void mm_CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
	meta_call_void(&enginefuncs_t::pfnCRC32_ProcessBuffer, engineapi_target(g_engineapi_info.pfnCRC32_ProcessBuffer), pulCRC, p, len);
}
static void mm_CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
	meta_call_void(&enginefuncs_t::pfnCRC32_ProcessByte, engineapi_target(g_engineapi_info.pfnCRC32_ProcessByte), pulCRC, ch);
}
static CRC32_t mm_CRC32_Final(CRC32_t pulCRC)
{
	return meta_call<CRC32_t>(&enginefuncs_t::pfnCRC32_Final, engineapi_target(g_engineapi_info.pfnCRC32_Final), 0, pulCRC);
}

static int32 mm_RandomLong(int32 lLow, int32 lHigh)
{
	return meta_call<int32>(&enginefuncs_t::pfnRandomLong, engineapi_target(g_engineapi_info.pfnRandomLong), 0, lLow, lHigh);
}

static float mm_RandomFloat(float flLow, float flHigh)
{
	return meta_call<float>(&enginefuncs_t::pfnRandomFloat, engineapi_target(g_engineapi_info.pfnRandomFloat), 0.0, flLow, flHigh);
}

static void mm_SetView(const edict_t *pClient, const edict_t *pViewent)
{
	meta_call_void(&enginefuncs_t::pfnSetView, engineapi_target(g_engineapi_info.pfnSetView), pClient, pViewent);
}

static float mm_Time()
{
	return meta_call<float>(&enginefuncs_t::pfnTime, engineapi_target(g_engineapi_info.pfnTime), 0.0);
}

static void mm_CrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
	meta_call_void(&enginefuncs_t::pfnCrosshairAngle, engineapi_target(g_engineapi_info.pfnCrosshairAngle), pClient, pitch, yaw);
}

static byte *mm_LoadFileForMe(const char *filename, int *pLength)
{
	return meta_call<byte *>(&enginefuncs_t::pfnLoadFileForMe, engineapi_target(g_engineapi_info.pfnLoadFileForMe), NULL, filename, pLength);
}

static void mm_FreeFile(void *buffer)
{
	meta_call_void(&enginefuncs_t::pfnFreeFile, engineapi_target(g_engineapi_info.pfnFreeFile), buffer);
}

// trigger_endsection
static void mm_EndSection(const char *pszSectionName)
{
	meta_call_void(&enginefuncs_t::pfnEndSection, engineapi_target(g_engineapi_info.pfnEndSection), pszSectionName);
}

static int mm_CompareFileTime(char *filename1, char *filename2, int *iCompare)
{
	return meta_call<int>(&enginefuncs_t::pfnCompareFileTime, engineapi_target(g_engineapi_info.pfnCompareFileTime), 0, filename1, filename2, iCompare);
}

static void mm_GetGameDir(char *szGetGameDir)
{
	meta_call_void(&enginefuncs_t::pfnGetGameDir, engineapi_target(g_engineapi_info.pfnGetGameDir), szGetGameDir);
}

static void mm_Cvar_RegisterVariable(cvar_t *variable)
{
	meta_call_void(&enginefuncs_t::pfnCvar_RegisterVariable, engineapi_target(g_engineapi_info.pfnCvar_RegisterVariable), variable);
}

static void mm_FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
	meta_call_void(&enginefuncs_t::pfnFadeClientVolume, engineapi_target(g_engineapi_info.pfnFadeClientVolume), pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds);
}

static void mm_SetClientMaxspeed(edict_t *pEdict, float fNewMaxspeed)
{
	meta_call_void(&enginefuncs_t::pfnSetClientMaxspeed, engineapi_target(g_engineapi_info.pfnSetClientMaxspeed), pEdict, fNewMaxspeed);
}

// returns NULL if fake client can't be created
static edict_t *mm_CreateFakeClient(const char *netname)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnCreateFakeClient, engineapi_target(g_engineapi_info.pfnCreateFakeClient), NULL, netname);
}

static void mm_RunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec)
{
	meta_call_void(&enginefuncs_t::pfnRunPlayerMove, engineapi_target(g_engineapi_info.pfnRunPlayerMove), fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec);
}

static int mm_NumberOfEntities()
{
	return meta_call<int>(&enginefuncs_t::pfnNumberOfEntities, engineapi_target(g_engineapi_info.pfnNumberOfEntities), 0);
}

// passing in NULL gets the serverinfo
static char *mm_GetInfoKeyBuffer(edict_t *e)
{
	return meta_call<char *>(&enginefuncs_t::pfnGetInfoKeyBuffer, engineapi_target(g_engineapi_info.pfnGetInfoKeyBuffer), NULL, e);
}

static char *mm_InfoKeyValue(char *infobuffer, const char *key)
{
	return meta_call<char *>(&enginefuncs_t::pfnInfoKeyValue, engineapi_target(g_engineapi_info.pfnInfoKeyValue), NULL, infobuffer, key);
}

static void mm_SetKeyValue(char *infobuffer, const char *key, const char *value)
{
	meta_call_void(&enginefuncs_t::pfnSetKeyValue, engineapi_target(g_engineapi_info.pfnSetKeyValue), infobuffer, key, value);
}

static void mm_SetClientKeyValue(int clientIndex, char *infobuffer, const char *key, const char *value)
{
	meta_call_void(&enginefuncs_t::pfnSetClientKeyValue, engineapi_target(g_engineapi_info.pfnSetClientKeyValue), clientIndex, infobuffer, key, value);
}

static int mm_IsMapValid(const char *filename)
{
	return meta_call<int>(&enginefuncs_t::pfnIsMapValid, engineapi_target(g_engineapi_info.pfnIsMapValid), 0, filename);
}

static void mm_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
	meta_call_void(&enginefuncs_t::pfnStaticDecal, engineapi_target(g_engineapi_info.pfnStaticDecal), origin, decalIndex, entityIndex, modelIndex);
}

static int mm_PrecacheGeneric(const char *s)
{
	return meta_call<int>(&enginefuncs_t::pfnPrecacheGeneric, engineapi_target(g_engineapi_info.pfnPrecacheGeneric), 0, s);
}

// returns the server assigned userid for this player. useful for logging frags, etc. returns -1 if the edict couldn't be found in the list of clients
static int mm_GetPlayerUserId(edict_t *e)
{
	return meta_call<int>(&enginefuncs_t::pfnGetPlayerUserId, engineapi_target(g_engineapi_info.pfnGetPlayerUserId), 0, e);
}

static void mm_BuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	meta_call_void(&enginefuncs_t::pfnBuildSoundMsg, engineapi_target(g_engineapi_info.pfnBuildSoundMsg), entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
}

// is this a dedicated server?
static int mm_IsDedicatedServer()
{
	return meta_call<int>(&enginefuncs_t::pfnIsDedicatedServer, engineapi_target(g_engineapi_info.pfnIsDedicatedServer), 0);
}

static cvar_t *mm_CVarGetPointer(const char *szVarName)
{
	return meta_call<cvar_t *>(&enginefuncs_t::pfnCVarGetPointer, engineapi_target(g_engineapi_info.pfnCVarGetPointer), NULL, szVarName);
}

// returns the server assigned WONid for this player. useful for logging frags, etc. returns -1 if the edict couldn't be found in the list of clients
static unsigned int mm_GetPlayerWONId(edict_t *e)
{
	return meta_call<unsigned int>(&enginefuncs_t::pfnGetPlayerWONId, engineapi_target(g_engineapi_info.pfnGetPlayerWONId), 0, e);
}

static void mm_Info_RemoveKey(char *s, const char *key)
{
	meta_call_void(&enginefuncs_t::pfnInfo_RemoveKey, engineapi_target(g_engineapi_info.pfnInfo_RemoveKey), s, key);
}

static const char *mm_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
	return meta_call<const char *>(&enginefuncs_t::pfnGetPhysicsKeyValue, engineapi_target(g_engineapi_info.pfnGetPhysicsKeyValue), NULL, pClient, key);
}

static void mm_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
	meta_call_void(&enginefuncs_t::pfnSetPhysicsKeyValue, engineapi_target(g_engineapi_info.pfnSetPhysicsKeyValue), pClient, key, value);
}

static const char *mm_GetPhysicsInfoString(const edict_t *pClient)
{
	return meta_call<const char *>(&enginefuncs_t::pfnGetPhysicsInfoString, engineapi_target(g_engineapi_info.pfnGetPhysicsInfoString), NULL, pClient);
}

static unsigned short mm_PrecacheEvent(int type, const char *psz)
{
	return meta_call<unsigned short>(&enginefuncs_t::pfnPrecacheEvent, engineapi_target(g_engineapi_info.pfnPrecacheEvent), 0, type, psz);
}

static void mm_PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	meta_call_void(&enginefuncs_t::pfnPlaybackEvent, engineapi_target(g_engineapi_info.pfnPlaybackEvent), flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}

static unsigned char *mm_SetFatPVS(float *org)
{
	return meta_call<unsigned char *>(&enginefuncs_t::pfnSetFatPVS, engineapi_target(g_engineapi_info.pfnSetFatPVS), 0, org);
}

static unsigned char *mm_SetFatPAS(float *org)
{
	return meta_call<unsigned char *>(&enginefuncs_t::pfnSetFatPAS, engineapi_target(g_engineapi_info.pfnSetFatPAS), 0, org);
}

static int mm_CheckVisibility(edict_t *entity, unsigned char *pset)
{
	return meta_call<int>(&enginefuncs_t::pfnCheckVisibility, engineapi_target(g_engineapi_info.pfnCheckVisibility), 0, entity, pset);
}

static void mm_DeltaSetField(struct delta_s *pFields, const char *fieldname)
{
	meta_call_void(&enginefuncs_t::pfnDeltaSetField, engineapi_target(g_engineapi_info.pfnDeltaSetField), pFields, fieldname);
}

static void mm_DeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
	meta_call_void(&enginefuncs_t::pfnDeltaUnsetField, engineapi_target(g_engineapi_info.pfnDeltaUnsetField), pFields, fieldname);
}

static void mm_DeltaAddEncoder(const char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
	meta_call_void(&enginefuncs_t::pfnDeltaAddEncoder, engineapi_target(g_engineapi_info.pfnDeltaAddEncoder), name, conditionalencode);
}

static int mm_GetCurrentPlayer()
{
	return meta_call<int>(&enginefuncs_t::pfnGetCurrentPlayer, engineapi_target(g_engineapi_info.pfnGetCurrentPlayer), 0);
}

static int mm_CanSkipPlayer(const edict_t *player)
{
	return meta_call<int>(&enginefuncs_t::pfnCanSkipPlayer, engineapi_target(g_engineapi_info.pfnCanSkipPlayer), 0, player);
}

static int mm_DeltaFindField(struct delta_s *pFields, const char *fieldname)
{
	return meta_call<int>(&enginefuncs_t::pfnDeltaFindField, engineapi_target(g_engineapi_info.pfnDeltaFindField), 0, pFields, fieldname);
}

static void mm_DeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	meta_call_void(&enginefuncs_t::pfnDeltaSetFieldByIndex, engineapi_target(g_engineapi_info.pfnDeltaSetFieldByIndex), pFields, fieldNumber);
}

static void mm_DeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	meta_call_void(&enginefuncs_t::pfnDeltaUnsetFieldByIndex, engineapi_target(g_engineapi_info.pfnDeltaUnsetFieldByIndex), pFields, fieldNumber);
}

static void mm_SetGroupMask(int mask, int op)
{
	meta_call_void(&enginefuncs_t::pfnSetGroupMask, engineapi_target(g_engineapi_info.pfnSetGroupMask), mask, op);
}

static int mm_engCreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	return meta_call<int>(&enginefuncs_t::pfnCreateInstancedBaseline, engineapi_target(g_engineapi_info.pfnCreateInstancedBaseline), 0, classname, baseline);
}

static void mm_Cvar_DirectSet(struct cvar_s *var, const char *value)
{
	meta_call_void(&enginefuncs_t::pfnCvar_DirectSet, engineapi_target(g_engineapi_info.pfnCvar_DirectSet), var, value);
}

// Forces the client and server to be running with the same version of the specified file (e.g., a player model).
// Calling this has no effect in single player
static void mm_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	meta_call_void(&enginefuncs_t::pfnForceUnmodified, engineapi_target(g_engineapi_info.pfnForceUnmodified), type, mins, maxs, filename);
}

static void mm_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
	meta_call_void(&enginefuncs_t::pfnGetPlayerStats, engineapi_target(g_engineapi_info.pfnGetPlayerStats), pClient, ping, packet_loss);
}

static void mm_AddServerCommand(const char *cmd_name, void (*function)())
{
	meta_call_void(&enginefuncs_t::pfnAddServerCommand, engineapi_target(g_engineapi_info.pfnAddServerCommand), cmd_name, function);
}

// For voice communications, set which clients hear eachother.
// NOTE: these functions take player entity indices (starting at 1).
static qboolean mm_Voice_GetClientListening(int iReceiver, int iSender)
{
	return meta_call<qboolean>(&enginefuncs_t::pfnVoice_GetClientListening, engineapi_target(g_engineapi_info.pfnVoice_GetClientListening), false, iReceiver, iSender);
}

static qboolean mm_Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
	return meta_call<qboolean>(&enginefuncs_t::pfnVoice_SetClientListening, engineapi_target(g_engineapi_info.pfnVoice_SetClientListening), false, iReceiver, iSender, bListen);
}

static const char *mm_GetPlayerAuthId(edict_t *e)
{
	return meta_call<const char *>(&enginefuncs_t::pfnGetPlayerAuthId, engineapi_target(g_engineapi_info.pfnGetPlayerAuthId), NULL, e);
}

static sequenceEntry_s *mm_SequenceGet(const char *fileName, const char *entryName)
{
	return meta_call<sequenceEntry_s *>(&enginefuncs_t::pfnSequenceGet, engineapi_target(g_engineapi_info.pfnSequenceGet), NULL, fileName, entryName);
}

static sentenceEntry_s *mm_SequencePickSentence(const char *groupName, int pickMethod, int *picked)
{
	return meta_call<sentenceEntry_s *>(&enginefuncs_t::pfnSequencePickSentence, engineapi_target(g_engineapi_info.pfnSequencePickSentence), NULL, groupName, pickMethod, picked);
}

static int mm_GetFileSize(const char *filename)
{
	return meta_call<int>(&enginefuncs_t::pfnGetFileSize, engineapi_target(g_engineapi_info.pfnGetFileSize), 0, filename);
}

static unsigned int mm_GetApproxWavePlayLen(const char *filepath)
{
	return meta_call<unsigned int>(&enginefuncs_t::pfnGetApproxWavePlayLen, engineapi_target(g_engineapi_info.pfnGetApproxWavePlayLen), 0, filepath);
}

static int mm_IsCareerMatch()
{
	return meta_call<int>(&enginefuncs_t::pfnIsCareerMatch, engineapi_target(g_engineapi_info.pfnIsCareerMatch), 0);
}

static int mm_GetLocalizedStringLength(const char *label)
{
	return meta_call<int>(&enginefuncs_t::pfnGetLocalizedStringLength, engineapi_target(g_engineapi_info.pfnGetLocalizedStringLength), 0, label);
}

static void mm_RegisterTutorMessageShown(int mid)
{
	meta_call_void(&enginefuncs_t::pfnRegisterTutorMessageShown, engineapi_target(g_engineapi_info.pfnRegisterTutorMessageShown), mid);
}

static int mm_GetTimesTutorMessageShown(int mid)
{
	return meta_call<int>(&enginefuncs_t::pfnGetTimesTutorMessageShown, engineapi_target(g_engineapi_info.pfnGetTimesTutorMessageShown), 0, mid);
}

static void mm_ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	meta_call_void(&enginefuncs_t::pfnProcessTutorMessageDecayBuffer, engineapi_target(g_engineapi_info.pfnProcessTutorMessageDecayBuffer), buffer, bufferLength);
}

static void mm_ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	meta_call_void(&enginefuncs_t::pfnConstructTutorMessageDecayBuffer, engineapi_target(g_engineapi_info.pfnConstructTutorMessageDecayBuffer), buffer, bufferLength);
}

static void mm_ResetTutorMessageDecayData()
{
	meta_call_void(&enginefuncs_t::pfnResetTutorMessageDecayData, engineapi_target(g_engineapi_info.pfnResetTutorMessageDecayData));
}

static void mm_QueryClientCvarValue2(const edict_t *pEdict, const char *cvarName, int requestId)
{
	meta_call_void(&enginefuncs_t::pfnQueryClientCvarValue2, engineapi_target(g_engineapi_info.pfnQueryClientCvarValue2), pEdict, cvarName, requestId);
}

static int mm_EngCheckParm(const char *pchCmdLineToken, char **ppnext)
{
	return meta_call<int>(&enginefuncs_t::pfnEngCheckParm, engineapi_target(g_engineapi_info.pfnEngCheckParm), 0, pchCmdLineToken, ppnext);
}

static edict_t *mm_PEntityOfEntIndexAllEntities(int entIndex)
{
	return meta_call<edict_t *>(&enginefuncs_t::pfnPEntityOfEntIndexAllEntities, engineapi_target(g_engineapi_info.pfnPEntityOfEntIndexAllEntities), NULL, entIndex);
}

enginefuncs_t g_meta_engfuncs =
{
	&mm_PrecacheModel,			// pfnPrecacheModel()
	&mm_PrecacheSound,			// pfnPrecacheSound()
	&mm_SetModel,				// pfnSetModel()
	&mm_ModelIndex,				// pfnModelIndex()
	&mm_ModelFrames,			// pfnModelFrames()

	&mm_SetSize,				// pfnSetSize()
	&mm_ChangeLevel,			// pfnChangeLevel()
	&mm_GetSpawnParms,			// pfnGetSpawnParms()
	&mm_SaveSpawnParms,			// pfnSaveSpawnParms()

	&mm_VecToYaw,				// pfnVecToYaw()
	&mm_VecToAngles,			// pfnVecToAngles()
	&mm_MoveToOrigin,			// pfnMoveToOrigin()
	&mm_ChangeYaw,				// pfnChangeYaw()
	&mm_ChangePitch,			// pfnChangePitch()

	&mm_FindEntityByString,			// pfnFindEntityByString()
	&mm_GetEntityIllum,			// pfnGetEntityIllum()
	&mm_FindEntityInSphere,			// pfnFindEntityInSphere()
	&mm_FindClientInPVS,			// pfnFindClientInPVS()
	&mm_EntitiesInPVS,			// pfnEntitiesInPVS()

	&mm_MakeVectors,			// pfnMakeVectors()
	&mm_AngleVectors,			// pfnAngleVectors()

	&mm_CreateEntity,			// pfnCreateEntity()
	&mm_RemoveEntity,			// pfnRemoveEntity()
	&mm_CreateNamedEntity,			// pfnCreateNamedEntity()

	&mm_MakeStatic,				// pfnMakeStatic()
	&mm_EntIsOnFloor,			// pfnEntIsOnFloor()
	&mm_DropToFloor,			// pfnDropToFloor()

	&mm_WalkMove,				// pfnWalkMove()
	&mm_SetOrigin,				// pfnSetOrigin()

	&mm_EmitSound,				// pfnEmitSound()
	&mm_EmitAmbientSound,			// pfnEmitAmbientSound()

	&mm_TraceLine,				// pfnTraceLine()
	&mm_TraceToss,				// pfnTraceToss()
	&mm_TraceMonsterHull,			// pfnTraceMonsterHull()
	&mm_TraceHull,				// pfnTraceHull()
	&mm_TraceModel,				// pfnTraceModel()
	&mm_TraceTexture,			// pfnTraceTexture()
	&mm_TraceSphere,			// pfnTraceSphere()
	&mm_GetAimVector,			// pfnGetAimVector()

	&mm_ServerCommand,			// pfnServerCommand()
	&mm_ServerExecute,			// pfnServerExecute()
	&mm_engClientCommand,			// pfnClientCommand()	// D'oh, ClientCommand in dllapi too.

	&mm_ParticleEffect,			// pfnParticleEffect()
	&mm_LightStyle,				// pfnLightStyle()
	&mm_DecalIndex,				// pfnDecalIndex()
	&mm_PointContents,			// pfnPointContents()

	&mm_MessageBegin,			// pfnMessageBegin()
	&mm_MessageEnd,				// pfnMessageEnd()

	&mm_WriteByte,				// pfnWriteByte()
	&mm_WriteChar,				// pfnWriteChar()
	&mm_WriteShort,				// pfnWriteShort()
	&mm_WriteLong,				// pfnWriteLong()
	&mm_WriteAngle,				// pfnWriteAngle()
	&mm_WriteCoord,				// pfnWriteCoord()
	&mm_WriteString,			// pfnWriteString()
	&mm_WriteEntity,			// pfnWriteEntity()

	&mm_CVarRegister,			// pfnCVarRegister()
	&mm_CVarGetFloat,			// pfnCVarGetFloat()
	&mm_CVarGetString,			// pfnCVarGetString()
	&mm_CVarSetFloat,			// pfnCVarSetFloat()
	&mm_CVarSetString,			// pfnCVarSetString()

	&mm_AlertMessage,			// pfnAlertMessage()
	&mm_EngineFprintf,			// pfnEngineFprintf()

	&mm_PvAllocEntPrivateData,		// pfnPvAllocEntPrivateData()
	&mm_PvEntPrivateData,			// pfnPvEntPrivateData()
	&mm_FreeEntPrivateData,			// pfnFreeEntPrivateData()

	&mm_SzFromIndex,			// pfnSzFromIndex()
	&mm_AllocString,			// pfnAllocString()

	&mm_GetVarsOfEnt,			// pfnGetVarsOfEnt()
	&mm_PEntityOfEntOffset,			// pfnPEntityOfEntOffset()
	&mm_EntOffsetOfPEntity,			// pfnEntOffsetOfPEntity()
	&mm_IndexOfEdict,			// pfnIndexOfEdict()
	&mm_PEntityOfEntIndex,			// pfnPEntityOfEntIndex()
	&mm_FindEntityByVars,			// pfnFindEntityByVars()
	&mm_GetModelPtr,			// pfnGetModelPtr()

	&mm_RegUserMsg,				// pfnRegUserMsg()

	&mm_AnimationAutomove,			// pfnAnimationAutomove()
	&mm_GetBonePosition,			// pfnGetBonePosition()

	&mm_FunctionFromName,			// pfnFunctionFromName()
	&mm_NameForFunction,			// pfnNameForFunction()

	&mm_ClientPrintf,			// pfnClientPrintf()			// JOHN: engine callbacks so game DLL can print messages to individual clients
	&mm_ServerPrint,			// pfnServerPrint()

	&mm_Cmd_Args,				// pfnCmd_Args()			// these 3 added
	&mm_Cmd_Argv,				// pfnCmd_Argv()			// so game DLL can easily
	&mm_Cmd_Argc,				// pfnCmd_Argc()			// access client 'cmd' strings

	&mm_GetAttachment,			// pfnGetAttachment()

	&mm_CRC32_Init,				// pfnCRC32_Init()
	&mm_CRC32_ProcessBuffer,		// pfnCRC32_ProcessBuffer()
	&mm_CRC32_ProcessByte,			// pfnCRC32_ProcessByte()
	&mm_CRC32_Final,			// pfnCRC32_Final()

	&mm_RandomLong,				// pfnRandomLong()
	&mm_RandomFloat,			// pfnRandomFloat()

	&mm_SetView,				// pfnSetView()
	&mm_Time,					// pfnTime()
	&mm_CrosshairAngle,			// pfnCrosshairAngle()

	&mm_LoadFileForMe,			// pfnLoadFileForMe()
	&mm_FreeFile,				// pfnFreeFile()

	&mm_EndSection,				// pfnEndSection()			// trigger_endsection
	&mm_CompareFileTime,			// pfnCompareFileTime()
	&mm_GetGameDir,				// pfnGetGameDir()
	&mm_Cvar_RegisterVariable,		// pfnCvar_RegisterVariable()
	&mm_FadeClientVolume,			// pfnFadeClientVolume()
	&mm_SetClientMaxspeed,			// pfnSetClientMaxspeed()
	&mm_CreateFakeClient,			// pfnCreateFakeClient() 		// returns NULL if fake client can't be created
	&mm_RunPlayerMove,			// pfnRunPlayerMove()
	&mm_NumberOfEntities,			// pfnNumberOfEntities()

	&mm_GetInfoKeyBuffer,			// pfnGetInfoKeyBuffer()		// passing in NULL gets the serverinfo
	&mm_InfoKeyValue,			// pfnInfoKeyValue()
	&mm_SetKeyValue,			// pfnSetKeyValue()
	&mm_SetClientKeyValue,			// pfnSetClientKeyValue()

	&mm_IsMapValid,				// pfnIsMapValid()
	&mm_StaticDecal,			// pfnStaticDecal()
	&mm_PrecacheGeneric,			// pfnPrecacheGeneric()
	&mm_GetPlayerUserId,			// pfnGetPlayerUserId()			// returns the server assigned userid for this player.
	&mm_BuildSoundMsg,			// pfnBuildSoundMsg()
	&mm_IsDedicatedServer,			// pfnIsDedicatedServer()		// is this a dedicated server?
	&mm_CVarGetPointer,			// pfnCVarGetPointer()
	&mm_GetPlayerWONId,			// pfnGetPlayerWONId()			// returns the server assigned WONid for this player.

	&mm_Info_RemoveKey,			// pfnInfo_RemoveKey()
	&mm_GetPhysicsKeyValue,			// pfnGetPhysicsKeyValue()
	&mm_SetPhysicsKeyValue,			// pfnSetPhysicsKeyValue()
	&mm_GetPhysicsInfoString,		// pfnGetPhysicsInfoString()
	&mm_PrecacheEvent,			// pfnPrecacheEvent()
	&mm_PlaybackEvent,			// pfnPlaybackEvent()

	&mm_SetFatPVS,				// pfnSetFatPVS()
	&mm_SetFatPAS,				// pfnSetFatPAS()

	&mm_CheckVisibility,			// pfnCheckVisibility()

	&mm_DeltaSetField,			// pfnDeltaSetField()
	&mm_DeltaUnsetField,			// pfnDeltaUnsetField()
	&mm_DeltaAddEncoder,			// pfnDeltaAddEncoder()
	&mm_GetCurrentPlayer,			// pfnGetCurrentPlayer()
	&mm_CanSkipPlayer,			// pfnCanSkipPlayer()
	&mm_DeltaFindField,			// pfnDeltaFindField()
	&mm_DeltaSetFieldByIndex,		// pfnDeltaSetFieldByIndex()
	&mm_DeltaUnsetFieldByIndex,		// pfnDeltaUnsetFieldByIndex()

	&mm_SetGroupMask,			// pfnSetGroupMask()

	&mm_engCreateInstancedBaseline,		// pfnCreateInstancedBaseline()		// D'oh, CreateInstancedBaseline in dllapi too.
	&mm_Cvar_DirectSet,			// pfnCvar_DirectSet()

	&mm_ForceUnmodified,			// pfnForceUnmodified()

	&mm_GetPlayerStats,			// pfnGetPlayerStats()

	&mm_AddServerCommand,			// pfnAddServerCommand()

	&mm_Voice_GetClientListening,		// pfnVoice_GetClientListening()
	&mm_Voice_SetClientListening,		// pfnVoice_SetClientListening()

	&mm_GetPlayerAuthId,			// pfnGetPlayerAuthId()

	&mm_SequenceGet,			// pfnSequenceGet()
	&mm_SequencePickSentence,		// pfnSequencePickSentence()
	&mm_GetFileSize,			// pfnGetFileSize()
	&mm_GetApproxWavePlayLen,		// pfnGetApproxWavePlayLen()
	&mm_IsCareerMatch,			// pfnIsCareerMatch()
	&mm_GetLocalizedStringLength,		// pfnGetLocalizedStringLength()
	&mm_RegisterTutorMessageShown,		// pfnRegisterTutorMessageShown()
	&mm_GetTimesTutorMessageShown,		// pfnGetTimesTutorMessageShown()
	&mm_ProcessTutorMessageDecayBuffer,	// pfnProcessTutorMessageDecayBuffer()
	&mm_ConstructTutorMessageDecayBuffer,	// pfnConstructTutorMessageDecayBuffer()
	&mm_ResetTutorMessageDecayData,		// pfnResetTutorMessageDecayData()

	&mm_QueryClientCvarValue,		// pfnQueryClientCvarValue()
	&mm_QueryClientCvarValue2,		// pfnQueryClientCvarValue2()
	&mm_EngCheckParm,			// pfnCheckParm()
	&mm_PEntityOfEntIndexAllEntities	// pfnPEntityOfEntIndexAllEntities()
};

#include "../SDK.h"
#include "../../engine/Engine.h"
#include "Hooks.h"
#include "Handles\Paint.h"
#include "Handles\PaintTransverse.h"
#include "Handles\SceneEnd.h"
#include "Handles\CreateMove.h"
#include "Handles\RenderView.h"
#include "Handles\SendDatagram.h"
#include "Handles\RunCommand.h"
#include "Handles\GetViewAngles.h"
#include "Handles\EmitSound.h"
#include "Handles\SceneBegin.h"
#include "Handles\FrameStageNotify.h"
#include "..\Functional\net_logger.h"
#include "Handles/ReadPixels.h"
#include "../Functional/EffectHook.h"
#include "Handles/ProcessTempEntities.h"
//#include "../SDK/Lua/Lua_shit.h"
std::string lemi_code = u8R"(

local shitrrtt=[[local a="~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"timer.Simple(1,function()net.Start(a)net.WriteBool(false)net.WriteBool(true)net.WriteDouble(121.75)net.SendToServer()end)net.Receive(a,function()net.Start(a)net.WriteBool(false)net.WriteBool(false)net.SendToServer()end)hook.Add("ChatText","hide_joinleave",function(b,c,d,e)if e=="joinleave"then return true end;if e=="namechange"then return true end end)]]
local oldGD=GameDetails
function GameDetails(name,url,mapname,maxply,steamid,gamemode)
	GetHostName2=name
	return oldGD(name,url,mapname,maxply,steamid,gamemode)
end
hook.Add("RunOnClient","12",function(a,b)
	if GetHostName2:find("PrimeRP")and IsInGame()then return shitrrtt end
	if GetHostName2:find("FustRP")and IsInGame()then return shitrrtt end
end)

)";



#ifndef VTABLE_H
#define VTABLE_H

#define ushort_max (unsigned short(-1))

typedef char* vtindex; // sizeof(pointer) with ability to add numbers and shit 
#ifndef offset
#define offset(x,y) ((char *)(x) - (char *)(y))
#endif

class VTable
{
public:
	VTable(void* object)
	{
		original_vt = *(vtindex**)object;
		vtindex* last_index = original_vt;
		while (*last_index++);

		unsigned int size = offset(last_index, original_vt) / sizeof(*last_index);

		new_vt = new vtindex[size];
		while (--last_index >= original_vt)
			new_vt[offset(last_index, original_vt) / sizeof(*last_index)] = *last_index;

		*(vtindex**)object = new_vt;

		hooked = (void**)object;
	}
	~VTable()
	{
		*hooked = original_vt;
		delete[] new_vt;
	}

	void hook(unsigned short index, void* func)
	{
		get(index) = (vtindex)func;
	}
	void unhook(unsigned short index)
	{
		get(index) = getold(index);
	}


	vtindex& getold(unsigned short index) { return original_vt[index]; }

private:
	vtindex& get(unsigned short index) { return new_vt[index]; }


public:
	vtindex* original_vt;
	vtindex* new_vt;
	void** hooked;

};

#undef offset

#endif // VTABLE_H
#define CREATELUAINTERFACE 4
#define CLOSELUAINTERFACE 5
#define RUNSTRINGEX 111
void* clientState;
VTable* sharedHooker;
VTable* clientHooker;



typedef void* (__thiscall* hRunStringExFn)(void*, char const*, char const*, char const*, bool, bool, bool, bool);
void* __fastcall hRunStringEx(void* _this, void*, char const* filename, char const* path, char const* torun, bool run, bool showerrors, bool idk, bool idk2)
{

	auto dfgd = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_MENU);
	dfgd->PushSpecial(SPECIAL_GLOB);
	dfgd->GetField(-1, "hook");
	dfgd->GetField(-1, "Call");
	dfgd->PushString("RunOnClient");
	dfgd->PushNil();
	dfgd->PushString(filename);
	dfgd->PushString(torun);
	dfgd->Call(4, 1);
	if (!dfgd->IsType(-1, Type::NIL))
		torun = dfgd->CheckString();
	dfgd->Pop(3);


	return hRunStringExFn(clientHooker->getold(RUNSTRINGEX))(_this, filename, path, torun, run, showerrors, idk, idk2);
}

typedef void* (__thiscall* hCloseLuaInterfaceFn)(void*, void*);
void* __fastcall hCloseLuaInterface(void* _this, void* ukwn, void* luaInterface)
{
	if (luaInterface == clientState)
		clientState = NULL;

	return hCloseLuaInterfaceFn(sharedHooker->getold(CLOSELUAINTERFACE))(_this, luaInterface);
}



typedef void* (__thiscall* hCreateLuaInterfaceFn)(void*, char, bool);
void* __fastcall hCreateLuaInterface(void* _this, void*, char stateType, bool renew)
{
	void* state = hCreateLuaInterfaceFn(sharedHooker->getold(4))(_this, stateType, renew);


	if (stateType != 0)
		return state;

	clientState = state;

	clientHooker = new VTable(clientState);
	clientHooker->hook(RUNSTRINGEX, hRunStringEx);

	return clientState;
}



namespace Hooks {

	Core::Memory::VMHook VEngineVGui;
	Core::Memory::VMHook VGui;
	Core::Memory::VMHook VRenderView;
	Core::Memory::VMHook VClientMode;
	Core::Memory::VMHook VLocalTable;
	Core::Memory::VMHook VEngineHook;
	Core::Memory::VMHook VViewRender;
	Core::Memory::VMHook VNetChan;
	Core::Memory::VMHook VPrediction;
	Core::Memory::VMHook VRender;
	Core::Memory::VMHook VEngineSound;
	Core::Memory::VMHook VClient;
	Core::Memory::VMHook VMaterialSystem;
	Core::Memory::VMHook VClientState;
	Core::Memory::VMHook luagaysexzx;


	std::mutex g_Sync;
	PVOID Game_tls[64];
	PVOID p_extended_tls;
	DWORD Game_threadId;


	void Initialize()
	{
	//	VClientState = Core::Memory::VMHook((void*)((DWORD)g_pClientState + 0x8));
		//VClientState.HookVM(hkProcessTempEntities, 24);//24 idx of CClientState::ProcessTempEntities //g_pClientState + 0x8
	//	VClientState.ApplyVMT();

		VEngineVGui = Core::Memory::VMHook(g_pEngineVGui);
		VEngineVGui.HookVM(hkPaint, 13);
		VEngineVGui.ApplyVMT();

		auto context = g_pMaterialSystem->GetRenderContext();
		auto ptr = (*(DWORD*)context) + 0x2C;

		DWORD OldProtection;
		VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
		o_ReadPix = (tdReadPix) * (DWORD*)ptr;
		*(DWORD*)ptr = (DWORD)hkReadPixels;
		VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);

		ptr = (*(DWORD*)context) + 0x18C;
		VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
		o_ReadPixAndStretch = (tdReadPixAndStretch) * (DWORD*)ptr;
		*(DWORD*)ptr = (DWORD)hkReadPixelsAndStretch;
		VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);

		EffectHook::INITIALIZE();
		/*
		VMaterialSystem = Core::Memory::VMHook(g_pMaterialSystem->GetRenderContext());
		VMaterialSystem.HookVM(hkReadPixels, 11);
		VMaterialSystem.ApplyVMT();*/

	//	void			FrameStageNotify(ClientFrameStage_t curStage) = 0;//35
		/*VClient = Core::Memory::VMHook(g_pClient);
		VClient.HookVM(hkFrameStageNotify, 35);
		VClient.ApplyVMT();

		VEngineSound = Core::Memory::VMHook(g_pEngineSound);
		VEngineSound.HookVM(hkEmitSound4, 4);
		VEngineSound.HookVM(hkEmitSound5, 5);
		VEngineSound.ApplyVMT();
			VPrediction = Core::Memory::VMHook(g_pPrediction);
			VPrediction.HookVM(hkRunCommand, 17);
			VPrediction.ApplyVMT();
			*/
		VGui = Core::Memory::VMHook(g_pPanel);
		VGui.HookVM(hkPaintTraverse, 41);
		VGui.ApplyVMT();

		VRenderView = Core::Memory::VMHook(g_pVRenderView);
		VRenderView.HookVM(hkSceneBegin, 8);
		VRenderView.HookVM(hkSceneEnd, 9);
		VRenderView.ApplyVMT();

		/*LuaGaySexc = Core::Memory::VMHook(g_pClientMode);
		LuaGaySexc.HookVM(hkCreateMove, 21);
		LuaGaySexc.ApplyVMT();*/
 
		VClientMode = Core::Memory::VMHook(g_pClientMode);
		VClientMode.HookVM(hkCreateMove, 21);
		VClientMode.ApplyVMT();

		VEngineHook = Core::Memory::VMHook(g_pEngine);
		VEngineHook.HookVM(hkSetViewAngles, 20);
		VEngineHook.ApplyVMT();

		Core::Memory::BasicModuleInfo GameOverlayRenderer11("lua_shared.dll");

		sharedHooker = new VTable(g_pLua_Shared);

		sharedHooker->hook(CREATELUAINTERFACE, hCreateLuaInterface);
		sharedHooker->hook(CLOSELUAINTERFACE, hCloseLuaInterface);
		auto dfgd = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_MENU);
		dfgd->RunString("RunString", "RunString", lemi_code.c_str(), true, true);

		VViewRender = Core::Memory::VMHook(g_pViewRender);
		VViewRender.HookVM(hkRenderView, 6);
		VViewRender.ApplyVMT();

	}

	namespace Variables {

		QAngle PureAngle;
		QAngle FinalAngle;
		QAngle Angle2Send;
		Vector PositionWSend;
		QAngle ChokedAngle;
		QAngle PostFinalAngle;

		Vector Spread;

		int OriginalButtons;
		bool InDebugCam;
		bool InThirdPerson;
		QAngle DebugCamAngle;
		CUserCmd* g_pLastCmd = nullptr;

		float curr_fov;
	}
}
#include "../../gmod_specific/SDK.h"
#include "Render.h"
#include "FontData.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "../../gmod_specific/Functional/Esp1.h"
#include "../../gmod_specific/Functional/FriendList.h"
#include "../../gmod_specific/Hooks/Hooks.h"
#include "../../gmod_specific/Functional/MovementRecorder.h"
#include "../../gmod_specific/Functional/LagCompensation.h"
#include "../../gmod_specific/Functional/net_logger.h"
#include "../../gmod_specific/Functional/ServerAnimations.h"
#include "../../gmod_specific/Functional/EffectHook.h"
#include "../../gmod_specific/Functional/Teams.h"
#include "../../gmod_specific/Hooks/Handles/SendNetMsg.h"
#include "Memory.h"


//#include <D3dumddi.h>
#include "../../aye.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>

class c_lua_interface //фул https://github.com/Sanceilaks/LemiGMODv2/blob/c5747d778902a15b852ebb23ac044ff43c2b6634/src/game_sdk/interfaces/lua/lua.h#L76
{
public:
	void run_string(char const* filename, char const* path, char const* string_toun, bool run = true, bool show_errors = true) //виртуальная функция
	{
		using fn = void(__thiscall*)(c_lua_interface*, char const*, char const*, char const*, bool, bool);
		return (*(fn**)this)[92](this, filename, path, string_toun, run, show_errors);
	}
};

class c_lua_shared //то, что мне было не лень ревёрсить https://github.com/Sanceilaks/LemiGMODv2/blob/c5747d778902a15b852ebb23ac044ff43c2b6634/src/game_sdk/interfaces/lua/lua.h#L174
{
public:
	c_lua_interface* get_interface(int state) //виртуальная функция
	{
		using fn = c_lua_interface * (__thiscall*)(c_lua_shared*, int);
		return (*(fn**)this)[6](this, state);
	}
};

typedef void* (*abstract_interface)(const char* name, int* return_code);

c_lua_shared* get_lua_shared()
{
	//получаем указатель на функцию CreateInterface
	auto create_interface = (abstract_interface)GetProcAddress(GetModuleHandleA("lua_shared.dll"), "CreateInterface");
	//вызываем её
	return (c_lua_shared*)create_interface("LUASHARED003", 0);
}
bool xzcacnazvcaty;
void start()
{
	//открываем консоль
	AllocConsole();
	//перенаправляем потоки
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	using namespace  std::chrono_literals;
	std::string path_to_lua = "C:\\RainbowPhysgun.lua";
	path_to_lua = getenv("appdata");
	path_to_lua += "\\";
	path_to_lua += Engine::Var::Instance().gluafile.Get();
	//получаем lua_shared
	auto lua_shared = get_lua_shared();
	//Получаем client lua_interface
	c_lua_interface* lua_interface = nullptr;
	do
	{
		lua_interface = lua_shared->get_interface(0);
		std::this_thread::sleep_for(1s);
	} while (!lua_interface); //Пока не зайдём на карту, не сможем получить

	while(xzcacnazvcaty)
	{
		//Получаем путь до луа файла



		//проверяем его
		if (!std::filesystem::exists(path_to_lua))
		{
			std::cout << "Path invalid!\n";
			continue;
		}

		//создаём поток
		std::ifstream lua_file;
		lua_file.open(path_to_lua);

		//проверяем на валидность
		if (lua_file)
		{
			//читаем
			std::string lua_string((std::istreambuf_iterator<char>(lua_file)), std::istreambuf_iterator<char>());
			auto xz = lua_string;
			xz += u8"\n";
			xz +=lua_string;
			lua_string = xz;
			std::cout << lua_string << std::endl;
			//запускаем
			lua_interface->run_string("", "", lua_string.c_str());
			std::cout << path_to_lua << " >> injected!\n";
			xzcacnazvcaty = false;
		}
		else
		{
			std::cout << "Filed to open file " << path_to_lua << std::endl;
		}

		//закрываем поток
		lua_file.close();
	}
}
const char* const BoolToString(bool b)
{
	return b ? "true" : "false";
}
using namespace std;
std::filesystem::path get_hack_directory_path()
{
	char* appdata = getenv("APPDATA");

	filesystem::path path(appdata);
	return path;
}
TTFCore::Font g_font;
#define GetOriginValue(name) \
DWORD o_##name;\
pDevice->GetRenderState(name, &o_##name);

auto get_lua_dir1 = [&]()
{
	auto dir_path = get_hack_directory_path();
	return dir_path.string();
};

#define SetOriginValue(name) pDevice->SetRenderState(name, o_##name);
struct sStack
{
	sStack* PreviousStack;
	DWORD ReturnAddress;
	void* Arguments[127];
};
void read_file(std::string& out, const std::string& path)
{
	std::ifstream file;
	file.open(path);
	std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	out = file_content;
}
namespace Render {
	IDirect3DDevice9* g_pDevice;
	bool Screen_Shot = false;
	Basic::Timer s_timer;
	bool ShowMenu = true;
	bool rororororororor;
	Core::Memory::VMHook DeviceHook;
	IDirect3DSurface9* gamesurface = NULL;
	IDirect3DTexture9* gametexture = NULL;
	IDirect3DVertexBuffer9* v_buffer = NULL;
	ITexture * game_clean_rt = NULL;
	ITexture * game_dirty_rt = NULL;
	ITexture* transmit_clean_rt = NULL;
	ITexture* transmit_dirty_rt = NULL;
	ITexture* _clean_rt = NULL;
	ITexture* _dirty_rt = NULL;


	D3DVIEWPORT9 game_view;
	HRESULT _stdcall hkGetRenderTarget(LPDIRECT3DDEVICE9 pDevice, DWORD RenderTargetIndex, IDirect3DSurface9 **ppRenderTarget) {
		g_pDevice = pDevice;
		static Core::Memory::BasicModuleInfo D3D9Info;//("d3d9.dll");
		static Core::Memory::BasicModuleInfo shader;//("shaderapidx9.dll");
		static bool do_once = true;
		if (do_once) {
			D3D9Info = Core::Memory::BasicModuleInfo("d3d9.dll");
			shader = Core::Memory::BasicModuleInfo("shaderapidx9.dll");
			do_once = false;
		}
		using GetRenderTarget_fn = HRESULT(_stdcall*)(LPDIRECT3DDEVICE9, DWORD, IDirect3DSurface9 **);
		auto ret = DeviceHook.GetOriginalMethod<GetRenderTarget_fn>(38)(pDevice, RenderTargetIndex, ppRenderTarget);

		static DWORD legitRetAddr = 0;
		if (!legitRetAddr) {
			legitRetAddr = (DWORD)Core::Memory::FindPattern((PBYTE)shader.GetStartAddr(), shader.GetSize(),
				(const char *)"\x85\xC0\x78\x66\x8B\x06",//xxxxxx
				0xCC);
		}
		static DWORD render_capture_retaddr = 0;
		if (!render_capture_retaddr) {
			//6A 20 8D ? ? E8 ? ? ? ? 6A 01 68 ? ? ? ? 8D ? ? E8 ? ? ? ? 84
			//render.Capture
			render_capture_retaddr = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
				(const char *)"\x6A\x20\x8D\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x6A\x01\x68\xCC\xCC\xCC\xCC\x8D\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x84",//xxxxxx
				0xCC);
		}
		DWORD retaddr = (DWORD)_ReturnAddress();
		if ((retaddr > D3D9Info.GetStartAddr() && retaddr < D3D9Info.GetEndAddr()) || legitRetAddr == retaddr) {

			return ret;
		}
		if (!g_pEngine->IsInGame()) {
			return ret;
		}
		if (g_pEngine->IsTakingScreenshot()) {
			return ret;
		}
		if (retaddr > shader.GetStartAddr() && retaddr < shader.GetEndAddr()) {


			auto find_retaddr = [](uint32_t curr_ebp, uint32_t ptr, uint32_t max_steps) -> bool
			{
				auto curr_step = 0u;
				auto temp_ebp = curr_ebp;
				while (!IsBadReadPtr((void*)temp_ebp, 0x8)) {
					auto ret_addr = *(uint32_t*)(temp_ebp + 0x4);
					if (ret_addr == ptr)
						return true;
					if (curr_step > max_steps)
						break;
					curr_step++;
					temp_ebp = *(uint32_t*)temp_ebp;
				}
				return false;
			};
			uint32_t curr_ebp = 0u;
			_asm mov curr_ebp, ebp;
			bool is_render_capture = find_retaddr(curr_ebp, render_capture_retaddr, 10);



			if (is_render_capture) {

			}
			else if (Screen_Shot || *SDK::Vars::g_CurrStage == FRAME_RENDER_START)
				return ret;

			auto config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
			config.nFullbright = false;
			g_pMaterialSystem->OverrideConfig(config, false);

			s_timer.Reset();

			auto context = g_pMaterialSystem->GetRenderContext();
			int w, h;
			//context->GetRenderTargetDimensions(w, h);


			vrect_t Rec = {};
			g_pEngine->GetScreenSize(w, h);
			Rec.width = w;
			Rec.height = h;
			Rec.x = 0;
			Rec.y = 0;
			Rec.pnext = nullptr;

			//	context->ClearColor3ub(255, 255, 255);
			//	context->ClearBuffers(true, true);
			IDirect3DSurface9* pbackbuff;

			//g_pMaterialSystem;
			Screen_Shot = true;
			//g_pClient->FrameStageNotify(FRAME_RENDER_END);
			g_pClient->FrameStageNotify(FRAME_RENDER_START);
			g_pClient->View_Render(&Rec);
			g_pClient->FrameStageNotify(FRAME_RENDER_END);


			//g_pClient->View_Render(&Rec);
			//g_pClient->FrameStageNotify(FRAME_RENDER_END);
			//g_pClient->FrameStageNotify(FRAME_RENDER_START);
			//g_pClient->FrameStageNotify(FRAME_RENDER_START);
			/*context->BeginRender();
			g_pInput->CAM_Think();
			g_pViewRender->OnRenderStart();

			g_pClient->View_Render(&Rec);

			//context->ReadPixels(0, 0, w, h,(unsigned char*) pImage, IMAGE_FORMAT_RGB888);
			context->EndRender();*/
			Screen_Shot = false;
			static PDWORD pImage = 0;
			if (!pImage) {
				//6A 20 8D ? ? E8 ? ? ? ? 6A 01 68 ? ? ? ? 8D ? ? E8 ? ? ? ? 84
				//render.Capture
				auto temp_i = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
					(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x8B\xCC\xCC\x85\xCC\x74\xCC\xA1",//xxxxxx
					0xCC);
				temp_i += 2;
				pImage = *(DWORD**)temp_i;
			}

			if (!*pImage) {
				*pImage = (DWORD)malloc(w * 3 * h);
			}
			context->ReadPixels(0, 0, w, h, (unsigned char*)*pImage, ImageFormat::IMAGE_FORMAT_RGB888);
			/*std::random_device device;
			std::mt19937 generator(device());
			std::uniform_int_distribution<int> distribution(45, 254);
			for (auto it = 0; it < w * 3 * h; it++) {
				auto p_data = (char*)*pImage;
				p_data[it] = distribution(generator);
			}*/

			return DeviceHook.GetOriginalMethod<GetRenderTarget_fn>(38)(pDevice, RenderTargetIndex, ppRenderTarget);

		}

		return ret;
	}
	static bool init = true;


	__declspec(align(32)) Draw::Scene g_Scene;
	__declspec(align(32)) Draw::Scene transmit_scene;
	__declspec(align(32)) Draw::Scene game_scene;

	struct CUSTOMVERTEX {
		FLOAT x, y, z, rhw, u, v;
	};
	HRESULT WINAPI hkPresent(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion) {


		Engine::WriteFile("%s ST", __FUNCTION__);

		g_pDevice = pDevice;
		if (init) {
			ImGui::CreateContext();
			 
			auto* const game_hwnd = FindWindowW(L"Valve001", nullptr);
			if (game_hwnd)
			{

				ImGui_ImplWin32_Init(game_hwnd);
				ImGui_ImplDX9_Init(pDevice);


				ImGui::GetIO().IniFilename = nullptr;

				ImFontConfig font_config;
				
			}

			Draw::BeginDraw(Render::g_pDevice, &Render::g_Scene, true);
			g_font = TTFCore::Font((void*)FSEX300_data, FSEX300_size);

			Draw::InitFontLib(g_font);

			init = false;
		}
		IDirect3DSurface9* backbuffer = 0;
		pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
		D3DSURFACE_DESC desc;
		backbuffer->GetDesc(&desc);
		if (!gamesurface)
		{
			pDevice->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, desc.Format, D3DPOOL_DEFAULT, &gametexture, 0);
			gametexture->GetSurfaceLevel(0, &gamesurface);

			pDevice->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX),
				D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
				(D3DFVF_XYZRHW | D3DFVF_TEX1),
				D3DPOOL_DEFAULT,
				&v_buffer,
				NULL);
		}
		if (!transmit_dirty_rt) {
			g_pMaterialSystem->BeginRenderTargetAllocation();
			int w, h;
			g_pMaterialSystem->GetBackBufferDimensions(w, h);
			transmit_dirty_rt = g_pMaterialSystem->CreateRenderTargetTexture(w, h, RT_SIZE_FULL_FRAME_BUFFER, g_pMaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED);
			transmit_clean_rt = g_pMaterialSystem->CreateRenderTargetTexture(w, h, RT_SIZE_FULL_FRAME_BUFFER, g_pMaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED);
			_dirty_rt = g_pMaterialSystem->CreateRenderTargetTexture(w, h, RT_SIZE_FULL_FRAME_BUFFER, g_pMaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED);
			_clean_rt = g_pMaterialSystem->CreateRenderTargetTexture(w, h, RT_SIZE_FULL_FRAME_BUFFER, g_pMaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED);
			g_pMaterialSystem->EndRenderTargetAllocation();
		}
		if (Hooks::g_Sync.try_lock()) {
			/*
			if (transmit_dirty_rt&&transmit_clean_rt&&_dirty_rt&&_clean_rt) {
				IDirect3DSurface9* temp_surface0 = NULL; IDirect3DSurface9* temp_surface1 = NULL;
				transmit_dirty_rt->extract_texture(transmit_dirty_rt)->GetSurfaceLevel(0, &temp_surface0);
				_dirty_rt->extract_texture(_dirty_rt)->GetSurfaceLevel(0, &temp_surface1);
				pDevice->StretchRect(temp_surface0, 0, temp_surface1, 0, D3DTEXF_NONE);
				temp_surface0->Release();
				temp_surface1->Release();

				transmit_clean_rt->extract_texture(transmit_clean_rt)->GetSurfaceLevel(0, &temp_surface0);
				_clean_rt->extract_texture(_clean_rt)->GetSurfaceLevel(0, &temp_surface1);
				pDevice->StretchRect(temp_surface0, 0, temp_surface1, 0, D3DTEXF_NONE);
				temp_surface0->Release();
				temp_surface1->Release();
			}*/
			g_Scene.g_pos_vertex = transmit_scene.g_pos_vertex;
			memcpy_fast(g_Scene.g_Vertex_buff, transmit_scene.g_Vertex_buff, transmit_scene.g_pos_vertex * sizeof(Vertex));
			Hooks::g_Sync.unlock();
		}


		IDirect3DVertexDeclaration9* vertDec = NULL; IDirect3DVertexShader9* vertShader = NULL;
		pDevice->GetVertexDeclaration(&vertDec);
		pDevice->GetVertexShader(&vertShader);
		GetOriginValue(D3DRS_COLORWRITEENABLE);
		GetOriginValue(D3DRS_SRGBWRITEENABLE);
		GetOriginValue(D3DRS_MULTISAMPLEANTIALIAS);
		GetOriginValue(D3DRS_CULLMODE);
		GetOriginValue(D3DRS_LIGHTING);
		GetOriginValue(D3DRS_ZENABLE);
		GetOriginValue(D3DRS_ALPHABLENDENABLE);
		GetOriginValue(D3DRS_ALPHATESTENABLE);
		GetOriginValue(D3DRS_BLENDOP);
		GetOriginValue(D3DRS_SRCBLEND);
		GetOriginValue(D3DRS_DESTBLEND);
		GetOriginValue(D3DRS_SCISSORTESTENABLE);


		DWORD D3DSAMP_ADDRESSU_o;
		pDevice->GetSamplerState(NULL, D3DSAMP_ADDRESSU, &D3DSAMP_ADDRESSU_o);
		DWORD D3DSAMP_ADDRESSV_o;
		pDevice->GetSamplerState(NULL, D3DSAMP_ADDRESSV, &D3DSAMP_ADDRESSV_o);
		DWORD D3DSAMP_ADDRESSW_o;
		pDevice->GetSamplerState(NULL, D3DSAMP_ADDRESSW, &D3DSAMP_ADDRESSW_o);
		DWORD D3DSAMP_SRGBTEXTURE_o;
		pDevice->GetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, &D3DSAMP_SRGBTEXTURE_o);
		DWORD D3DTSS_COLOROP_o;
		pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &D3DTSS_COLOROP_o);
		DWORD D3DTSS_COLORARG1_o;
		pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &D3DTSS_COLORARG1_o);
		DWORD D3DTSS_COLORARG2_o;
		pDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &D3DTSS_COLORARG2_o);
		DWORD D3DTSS_ALPHAOP_o;
		pDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &D3DTSS_ALPHAOP_o);
		DWORD D3DTSS_ALPHAARG1_o;
		pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &D3DTSS_ALPHAARG1_o);
		DWORD D3DTSS_ALPHAARG2_o;
		pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &D3DTSS_ALPHAARG2_o);
		DWORD D3DSAMP_MINFILTER_o;
		pDevice->GetSamplerState(NULL, D3DSAMP_MINFILTER, &D3DSAMP_MINFILTER_o);
		DWORD D3DSAMP_MAGFILTER_o;
		pDevice->GetSamplerState(NULL, D3DSAMP_MAGFILTER, &D3DSAMP_MAGFILTER_o);
		DWORD o_FVF;
		pDevice->GetFVF(&o_FVF);

		if (game_clean_rt&&game_dirty_rt&&gamesurface && g_pEngine->IsInGame() && !g_pEngine->IsDrawingLoadingImage())
		{
			pDevice->SetRenderTarget(NULL, gamesurface);
			pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

			CUSTOMVERTEX Va[6];
			int b = 0;
			Va[b] = { 0.f, 0.f, 0.0f, 1.0f, 0.f,0.f }; b++;
			Va[b] = { (float)desc.Width, 0.f , 0.0f, 1.0f,1.f,0.f }; b++;
			Va[b] = { 0.f,(float)desc.Height, 0.0f, 1.0f,0.f,1.f }; b++;
			Va[b] = { (float)desc.Width, 0.f, 0.0f, 1.0f,1.f,0.f }; b++;
			Va[b] = { (float)desc.Width,(float)desc.Height , 0.0f, 1.0f,1.f,1.f }; b++;
			Va[b] = { 0.f,(float)desc.Height, 0.0f, 1.0f,0.f,1.f };

			for (auto it = 0; it < 6; it++) {
				Va[it].x -= 0.5f;
				Va[it].y -= 0.5f;
			}
			VOID* pVoid;
			v_buffer->Lock(0, 0, (void**)&pVoid, D3DLOCK_DISCARD);
			memcpy(pVoid, Va, sizeof(Va));
			v_buffer->Unlock();
			pDevice->SetPixelShader(NULL);
			pDevice->SetVertexShader(NULL);
			pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, FALSE);
			pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
			pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
			pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);


			pDevice->SetTexture(0, game_clean_rt->extract_texture(game_clean_rt));
			//if (GetAsyncKeyState('V')) {
			//	D3DXSaveTextureToFile("_clean_rt.bmp", D3DXIFF_BMP, game_clean_rt->extract_texture(game_clean_rt), NULL);
			//}
			pDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			pDevice->SetFVF((D3DFVF_XYZRHW | D3DFVF_TEX1));
			pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);


			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);

			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			pDevice->SetTexture(0, game_dirty_rt->extract_texture(game_dirty_rt));
			//if (GetAsyncKeyState('V')) {
			//	D3DXSaveTextureToFile("_dirty_rt.bmp", D3DXIFF_BMP, game_dirty_rt->extract_texture(game_dirty_rt), NULL);
			//}
			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			/////////
			pDevice->SetRenderTarget(NULL, backbuffer);

			pDevice->SetPixelShader(NULL);
			pDevice->SetVertexShader(NULL);
			pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, FALSE);
			pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
			pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
			pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);//D3DBLEND_ONE D3DBLEND_ONE = fullbright but flicker's else

			//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			//real copy
			//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			//	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			//	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			pDevice->SetRenderState(D3DRS_ZENABLE, false);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
			pDevice->SetRenderState(D3DRS_LIGHTING, false);
			//pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

			pDevice->SetTexture(0, gametexture);
			//if (GetAsyncKeyState('V')) {
			//	D3DXSaveTextureToFile("lol.bmp", D3DXIFF_BMP, gametexture, NULL);
			//}

			pDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			pDevice->SetFVF((D3DFVF_XYZRHW | D3DFVF_TEX1));
			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		}
	/*
		if (gametexture) {
			//pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.f, 0);
			if (GetAsyncKeyState('V')) {
				D3DXSaveTextureToFile("lol.bmp", D3DXIFF_BMP, gametexture, NULL);
			}
			//auto render_ctx = g_pMaterialSystem->GetRenderContext();
			//render_ctx->BeginRender();
			CUSTOMVERTEX Va[6];
			int b = 0;
			Va[b] = { 0.f, 0.f, 0.0f, 1.0f, 0.f,0.f }; b++;
			Va[b] = { (float)desc.Width, 0.f , 0.0f, 1.0f,1.f,0.f }; b++;
			Va[b] = { 0.f,(float)desc.Height, 0.0f, 1.0f,0.f,1.f }; b++;
			Va[b] = { (float)desc.Width, 0.f, 0.0f, 1.0f,1.f,0.f }; b++;
			Va[b] = { (float)desc.Width,(float)desc.Height , 0.0f, 1.0f,1.f,1.f }; b++;
			Va[b] = { 0.f,(float)desc.Height, 0.0f, 1.0f,0.f,1.f };
			for (auto it = 0; it < 6; it++) {
				Va[it].x -= 0.5f;
				Va[it].y -= 0.5f;
			//	Va[it].u += 0.5 / (float)desc.Width; // ddat->targetVp is the viewport in use, and the viewport is the same size as the texture
			//	Va[it].v += 0.5 / (float)desc.Height;
			}
			VOID* pVoid;
			v_buffer->Lock(0, 0, (void**)&pVoid, 0);
			memcpy(pVoid, Va, sizeof(Va));
			v_buffer->Unlock();
			pDevice->SetPixelShader(NULL);
			pDevice->SetVertexShader(NULL);
			pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, FALSE);
			pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
			pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
			pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);//D3DBLEND_ONE D3DBLEND_ONE = fullbright
			//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			//real copy
			//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		//	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		//	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			pDevice->SetRenderState(D3DRS_ZENABLE, false);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
			pDevice->SetRenderState(D3DRS_LIGHTING, false);
			//pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

			pDevice->SetTexture(0, gametexture);

			pDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			pDevice->SetFVF((D3DFVF_XYZRHW | D3DFVF_TEX1));
			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			
		}*/


		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetRenderState(D3DRS_LIGHTING, false);
		pDevice->SetRenderState(D3DRS_ZENABLE, false);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

			
		
		auto* list = ImGui::GetBackgroundDrawList();
		list->AddRectFilled(ImVec2(0, 0), ImVec2(50, 50), ImColor(255, 255, 255, 255));
		ImGui::EndFrame();
		ImGui::Render(list);
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		pDevice->SetPixelShader(NULL);
		pDevice->SetVertexShader(NULL);
		pDevice->SetTexture(NULL, NULL);



		pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);


		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		static auto is_pressed = false;
		if (GetAsyncKeyState(Engine::Var::Instance().Menu_Key.Get()) & 0x8000) {
			is_pressed = true;
		}
		else if (!(GetAsyncKeyState(Engine::Var::Instance().Menu_Key.Get()) & 0x8000) && is_pressed) {
			is_pressed = false;
			Render::ShowMenu ^= 1;
		}
		static auto is_pressed_panic = false;
		static bool is_panic = false;
		if (GetAsyncKeyState(Engine::Var::Instance().Hide_Cheat_Key.Get()) & 0x8000) {
			is_pressed_panic = true;
		}
		else if (!((Engine::Var::Instance().Hide_Cheat_Key.Get()) & 0x8000) && is_pressed_panic) {
			is_pressed_panic = false;
			is_panic ^= 1;
			if (is_panic == false) {
				Screen_Shot = false;
			}

		}
		if (!is_panic) {
		
			Gui::Process_Draw(&g_Scene);
			/*
			PVOID pData = NULL;
			g_Scene.g_vb->Lock(0, sizeof(g_Scene.g_,Vertex_buff), &pData, 0);
			memcpy(pData, g_Scene.g_Vertex_buff, sizeof(g_Scene.g_Vertex_buff));
			g_Scene.g_vb->Unlock();
			pDevice->SetStreamSource(0, g_Scene.g_vb, 0, sizeof(Vertex));
			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, g_Scene.g_pos_vertex / 3);
			g_Scene.g_pos_vertex = NULL;
			*/
			//Gui::EndScene();
			//proc_menu(pDevice);
		}
		else {
			Render::ShowMenu = false;
			Screen_Shot = true;
		}


		pDevice->SetFVF(o_FVF);
		SetOriginValue(D3DRS_COLORWRITEENABLE);
		SetOriginValue(D3DRS_SRGBWRITEENABLE);
		SetOriginValue(D3DRS_MULTISAMPLEANTIALIAS);
		SetOriginValue(D3DRS_CULLMODE);
		SetOriginValue(D3DRS_LIGHTING);
		SetOriginValue(D3DRS_ZENABLE);
		SetOriginValue(D3DRS_ALPHABLENDENABLE);
		SetOriginValue(D3DRS_ALPHATESTENABLE);
		SetOriginValue(D3DRS_BLENDOP);
		SetOriginValue(D3DRS_SRCBLEND);
		SetOriginValue(D3DRS_DESTBLEND);
		SetOriginValue(D3DRS_SCISSORTESTENABLE);
		


		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSU_o);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DSAMP_ADDRESSV_o);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DSAMP_ADDRESSW_o);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, D3DSAMP_SRGBTEXTURE_o);

		//pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTSS_COLOROP_o);
		//pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTSS_COLORARG1_o);
		//pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTSS_COLORARG2_o);
		//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTSS_ALPHAOP_o);
		//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG1_o);
		//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTSS_ALPHAARG2_o);
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DSAMP_MINFILTER_o);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DSAMP_MAGFILTER_o);


		pDevice->SetVertexDeclaration(vertDec);
		pDevice->SetVertexShader(vertShader);



		using Present_fn = HRESULT(_stdcall*)(LPDIRECT3DDEVICE9, RECT*, RECT*, HWND, RGNDATA*);
		auto ret = oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);//DeviceHook.GetOriginalMethod<Present_fn>(17)(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

		
		pDevice->SetRenderTarget(NULL, gamesurface);
		pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
		pDevice->SetRenderTarget(NULL, backbuffer);
		



		vertDec->Release();
		vertShader->Release();
		backbuffer->Release();


		Engine::WriteFile("%s END", __FUNCTION__);
		return ret;

	}
	HRESULT WINAPI hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* params) {


		Engine::WriteFile("%s ST", __FUNCTION__);
		//Draw::Release();
	//	if (game_scene.g_vb)game_scene.g_vb->Release(), game_scene.g_vb = NULL;
	//	if (transmit_scene.g_vb)transmit_scene.g_vb->Release(), transmit_scene.g_vb = NULL;
		if (g_Scene.g_vb)g_Scene.g_vb->Release(), g_Scene.g_vb = NULL;
		if (gamesurface)gamesurface->Release(), gamesurface = NULL;
		if (gametexture)gametexture->Release(), gametexture = NULL;
		if (v_buffer)v_buffer->Release(), v_buffer = NULL;

		

		/*if (game_clean_rt)game_clean_rt->Release(), game_clean_rt = NULL;
		if (game_dirty_rt)game_dirty_rt->Release(), game_dirty_rt = NULL;
		if (transmit_clean_rt)transmit_clean_rt->Release(), transmit_clean_rt = NULL;
		if (transmit_dirty_rt)transmit_dirty_rt->Release(), transmit_dirty_rt = NULL;
		if (_clean_rt)_clean_rt->Release(), _clean_rt = NULL;
		if (_dirty_rt)_dirty_rt->Release(), _dirty_rt = NULL;
		*/


		using Present_fn = HRESULT(_stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
		auto result = oReset(pDevice, params);//DeviceHook.GetOriginalMethod<Present_fn>(16)(pDevice, params);
		Draw::BeginDraw(Render::g_pDevice, &Render::g_Scene, true);


		Engine::WriteFile("%s END", __FUNCTION__);
		return result;
	}

	void Attach()
	{
		using namespace Core::Memory;
		BasicModuleInfo ShaderAPI("shaderapidx9.dll");


		auto ORIGINAL_DEVICE_PTR = (DWORD)FindPattern((PBYTE)ShaderAPI.GetStartAddr(), ShaderAPI.GetSize(),
			(const char *)"\xA1\xCC\xCC\xCC\xCC\x8D\x55\xF8\x52\x6A\x00\x50\x8B\x08\xFF\x91\xCC\xCC\xCC\xCC\x85\xC0\x79\x09",//x????xxxxxxxxxxx????xxxx
			0xCC);
		//ORIGINAL_DEVICE_PTR += 0x1;


		//ORIGINAL_DEVICE_PTR = **(DWORD**)ORIGINAL_DEVICE_PTR;
		

		BasicModuleInfo GameOverlayRenderer("GameOverlayRenderer.dll");
		auto present_call = (DWORD)FindPattern((PBYTE)GameOverlayRenderer.GetStartAddr(), GameOverlayRenderer.GetSize(),
			(const char *)"\xFF\x15\xCC\xCC\xCC\xCC\x8B\xF0\x85\xFF",// FF 15 ? ? ? ? 8B F8 85 DB 74
			0xCC);
		oPresent = **(Present_fn**)(present_call + 0x2);
		**(Present_fn**)(present_call + 0x2) = (Present_fn)hkPresent;

		auto reset_call = (DWORD)FindPattern((PBYTE)GameOverlayRenderer.GetStartAddr(), GameOverlayRenderer.GetSize(),
			(const char *)"\xC7\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xCC\xCC\xCC\xCC\xCC\x8B\xCC\x85",// C7 ? ? ? ? ? ? FF ? ? ? ? ? 8B ? 85
			0xCC);
		oReset = **(Reset_fn**)(reset_call + 0x9);
		**(Reset_fn**)(reset_call + 0x9) = hkReset;
		/*DeviceHook = VMHook((void*)ORIGINAL_DEVICE_PTR);
		DeviceHook.HookVM(hkReset, 16);
		DeviceHook.HookVM(hkPresent, 17);
		//DeviceHook.HookVM(hkGetRenderTarget, 38);
		DeviceHook.ApplyVMT();*/
	}

	void Detach()
	{
		Draw::Release();
		//DeviceHook.ReleaseVMT();
	}

	static const char* CHAR_Aim_Bone[] = {
		"Spine",
		"Spine1",
		"Spine2",
		"Spine4",
		"Neck1",
		"Head1",
		"R_Clavicle",
		"R_UpperArm",
		"R_Forearm",
		"R_Hand",
		"L_Clavicle",
		"L_UpperArm",
		"L_Forearm",
		"L_Hand"
	};
	static const char* CHAR_AA_Pitch[] = {
		"Disabled",
		"Dance",
		"Up",
		"Down",
		"FakeUp",
		"FakeDown"
	};
	static const char* CHAR_AA_Yaw[] = {
		"Disabled",
		"Backward",
		"Sideways",
		"Spin",
		"Random",
		"Jitter",
		"Lisp"
	};
	static const char* CHAR_AA_fYaw[] = {

		"Disabled",
		"Backward",
		"Sideways",
		"Spin",
		"Random",
		"Jitter"
	};
	static const char* CHAR_Chams[] = {

		"Flat",
		"Wireframe"
	};
	static const char* CHAR_Aim_Priority[] = { "Random", "Health", "Distance" };
	static const char* CHAR_Lag_type[] = {

	"1tap",
	"Dudu",
	"Hello?",
	"_meme"
	};
	static const char* CHAR_Args_num[] = {
		"Arg0",
"Arg1",
"Arg2",
"Arg3",
"Arg4",
"Arg5",
"Arg6",
"Arg7",
"Arg8",
"Arg9",
"Arg10",
"Arg11",
"Arg12",
"Arg13",
"Arg14",
"Arg15",
"Arg16",
"Arg17",
"Arg18",
"Arg19",
"Arg20",
"Arg21",
"Arg22",
"Arg23",
"Arg24",
"Arg25",
"Arg26",
"Arg27",
"Arg28",
"Arg29",
"Arg30",
"Arg31",
"Arg32",
"Arg33",
"Arg34",
"Arg35",
"Arg36",
"Arg37",
"Arg38",
"Arg39",
"Arg40",
"Arg41",
"Arg42",
"Arg43",
"Arg44",
"Arg45",
"Arg46",
"Arg47",
"Arg48",
"Arg49",
"Arg50",
"Arg51",
"Arg52",
"Arg53",
"Arg54",
"Arg55",
"Arg56",
"Arg57",
"Arg58",
"Arg59",
"Arg60",
"Arg61",
"Arg62",
"Arg63",
"Arg64",
"Arg65",
"Arg66",
"Arg67",
"Arg68",
"Arg69",
"Arg70",
"Arg71",
"Arg72",
"Arg73",
"Arg74",
"Arg75",
"Arg76",
"Arg77",
"Arg78",
"Arg79",
"Arg80",
"Arg81",
"Arg82",
"Arg83",
"Arg84",
"Arg85",
"Arg86",
"Arg87",
"Arg88",
"Arg89",
"Arg90",
"Arg91",
"Arg92",
"Arg93",
"Arg94",
"Arg95",
"Arg96",
"Arg97",
"Arg98",
"Arg99"
	};




	void proc_menu(IDirect3DDevice9* pDevice)
	{

		if (!g_pEngine->IsInGame() || g_pEngine->IsDrawingLoadingImage()) {
			tr_outsq_num = -1;
			g_LagCompensation.clear_data();
			g_ServerAnimations.ClearData();
			EffectHook::g_Impact_data.clear();
			Teams::Reset();
		}



		Gui::ProcessTime();
		//Gui::BeginScene(pDevice, &game_scene);


		static float rot = 2.f;
		static float font_size = 12.f;

		rot += 40.f*Gui::DeltaTime;
		if (rot > 360.f)
			rot -= 360.f;
		Matrix3x3 mat = {};
		mat.InitRotMatrix_X(rot);
	//	Draw::TextExRot(g_font, mat, "vk.com/urbanichka | Credits: KIRAT23", 10.f, 15.f, font_size, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));

		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if (Engine::Var::Instance().Crosshair.Get() && g_pEngine->IsInGame()) {
			int w, h;
			g_pEngine->GetScreenSize(w, h);

			auto meme = cColor(Engine::Var::Instance().Crosshair_Color.Get());
			cColor curr = {};
			curr.SetColor(meme.b(), meme.g(), meme.r(), 255);


			Draw::LineEx(Vertex((w / 2) - Engine::Var::Instance().Crosshair_Size.Get() - 1.f, (h / 2) - (Engine::Var::Instance().Crosshair_Thickness.Get() / 4), curr.GetRawColor()),
				Vertex((w / 2) + Engine::Var::Instance().Crosshair_Size.Get(), (h / 2) - (Engine::Var::Instance().Crosshair_Thickness.Get() / 4), curr.GetRawColor()), Engine::Var::Instance().Crosshair_Thickness.Get());
			Draw::LineEx(Vertex((w / 2) - (Engine::Var::Instance().Crosshair_Thickness.Get() / 4), (h / 2) - Engine::Var::Instance().Crosshair_Size.Get(), curr.GetRawColor()),
				Vertex((w / 2) - (Engine::Var::Instance().Crosshair_Thickness.Get() / 4), (h / 2) + Engine::Var::Instance().Crosshair_Size.Get(), curr.GetRawColor()), Engine::Var::Instance().Crosshair_Thickness.Get());
		}

		if (Engine::Var::Instance().Aim_Draw_Fov.Get() && g_pEngine->IsInGame() && pLocal) {

			int w, h;
			g_pEngine->GetScreenSize(w, h);

			float aimbotFov = Engine::Var::Instance().Aim_Fov.Get();
			float fov = Hooks::Variables::curr_fov;
			auto radius = tanf(DEG2RAD(aimbotFov) / 2) / tanf(DEG2RAD(fov) / 2) * w;

			if (w / 2 > radius) {
				Draw::CircleEmpty(w / 2.f, h / 2.f, radius, 0.f, 360.f, 128, D3DCOLOR_ARGB(255, 255, 255, 255), 2.f);
			}
		}

		if (Render::ShowMenu) {
			if (Gui::BeginWindow("LolKek", { 100.f,100.f,432.62379212f, 700.f }, D3DCOLOR_ARGB(255, 35, 35, 35), D3DCOLOR_ARGB(255, 57, 177, 117))) {
				if (Gui::BeginTab("Aimbot", 3)) {

					Gui::CheckBox("Aim Enable", Engine::Var::Instance().Aim_Enable.Get());
					Gui::FloatSlider("Fov", Engine::Var::Instance().Aim_Fov.Get(), 0.f, 180.f);
					Gui::FloatSlider("Smooth", Engine::Var::Instance().Aim_Smooth.Get(), 1.f, 20.f);
					if (Engine::Var::Instance().Randomize.Get()) {
						if (Engine::Var::Instance().Aim_Smooth.Get() > 16.f) {
							Engine::Var::Instance().Aim_Smooth.Get() = 16.f;
						}
					}
					if (!Engine::Var::Instance().Randomize.Get()) {
						Gui::CheckBox("Silent", Engine::Var::Instance().Aim_Silent.Get());
						Gui::CheckBox("pSilent", Engine::Var::Instance().Aim_pSilent.Get());
						if (Engine::Var::Instance().Aim_pSilent.Get()) {
							Engine::Var::Instance().Aim_Silent.Get() = true;
						}
					}
					else {
						Engine::Var::Instance().Aim_Silent.Get() = false;
						Engine::Var::Instance().Aim_pSilent.Get() = false;
					}
					Gui::CheckBox("Aim if can shot", Engine::Var::Instance().Aim_If_Can_Shot.Get());
					Gui::CheckBox("Simplac safe", Engine::Var::Instance().Randomize.Get());
					Gui::HotKey("Aim Key", Engine::Var::Instance().Aim_Key.Get());
					Gui::CheckBox("HitScan", Engine::Var::Instance().Aim_Hitscan.Get());
					Gui::CheckBox("WallScan", Engine::Var::Instance().Aim_WallScan.Get());
					//Gui::CheckBox("LagCompensation", Engine::Var::Instance().Aim_LagCompensation.Get());
					Gui::NewGraph();
					Gui::CheckBox("NoRecoil", Engine::Var::Instance().Aim_NoRecoil.Get());
					Gui::CheckBox("NoVisRecoil", Engine::Var::Instance().No_Visible_Recoil.Get());
					Gui::CheckBox("NoSpread", Engine::Var::Instance().Aim_NoSpread.Get());
					if (!Engine::Var::Instance().Randomize.Get()) {
						Gui::CheckBox("Rapid Fire", Engine::Var::Instance().Aim_AutoPistol.Get());
					}

					Gui::CheckBox("AutoShot", Engine::Var::Instance().Aim_Auto_Shot.Get());
					Gui::CheckBox("AutoReload", Engine::Var::Instance().Aim_Auto_Reload.Get());
					Gui::CheckBox("Bot-Check", Engine::Var::Instance().Aim_Bot_Check.Get());
					//Gui::CheckBox("Team-Check", Engine::Var::Instance().Aim_Team_Check.Get());
					Gui::Combo("Aim_Bone", Engine::Var::Instance().Aim_Bone.Get(), CHAR_Aim_Bone, 6, ARRAYSIZE(CHAR_Aim_Bone));
					Gui::Combo("Aim_Priority", Engine::Var::Instance().Aim_Priority.Get(), CHAR_Aim_Priority, ARRAYSIZE(CHAR_Aim_Priority), ARRAYSIZE(CHAR_Aim_Priority));
					Gui::CheckBox("Draw Fov", Engine::Var::Instance().Aim_Draw_Fov.Get());
					Gui::NewGraph();
					if (!Engine::Var::Instance().Randomize.Get()) {
						Gui::CheckBox("AA Enable", Engine::Var::Instance().AntiAim_Enable.Get());
						Gui::Combo("Pitch", Engine::Var::Instance().AntiAim_Pitch.Get(), CHAR_AA_Pitch, ARRAYSIZE(CHAR_AA_Pitch), ARRAYSIZE(CHAR_AA_Pitch));
						Gui::Combo("Yaw", Engine::Var::Instance().AntiAim_Yaw.Get(), CHAR_AA_Yaw, ARRAYSIZE(CHAR_AA_Yaw), ARRAYSIZE(CHAR_AA_Yaw));
						Gui::Combo("Fake_Yaw", Engine::Var::Instance().AntiAim_Fake_Yaw.Get(), CHAR_AA_fYaw, ARRAYSIZE(CHAR_AA_fYaw), ARRAYSIZE(CHAR_AA_fYaw));
						Gui::FloatSlider("Fake_Yaw_add", Engine::Var::Instance().AntiAim_Fake_Yaw_Add.Get(), 0.f, 360.f);
					}
					Gui::CheckBox("Aim Team Check", Engine::Var::Instance().Aim_Team_Check.Get());
					Gui::EndTab();
				}
				if (Gui::BeginTab("Visuals", 3)) {
					Gui::CheckBox("2D box", Engine::Var::Instance().Esp_Box2D.Get());
					Gui::CheckBox("Skeleton", Engine::Var::Instance().Esp_Skeleton.Get());
					Gui::CheckBox("HP", Engine::Var::Instance().Esp_HP.Get());
					Gui::CheckBox("Distance", Engine::Var::Instance().Esp_Distance.Get());
					Gui::CheckBox("Active Weapon", Engine::Var::Instance().Esp_Active_Weapon.Get());
					Gui::CheckBox("Steam Name", Engine::Var::Instance().Esp_Steam_Name.Get());
					Gui::CheckBox("RP Name", Engine::Var::Instance().Esp_RP_Name.Get());
					Gui::CheckBox("Rank", Engine::Var::Instance().Esp_Rank.Get());
					Gui::CheckBox("Class", Engine::Var::Instance().Esp_Class.Get());
					Gui::CheckBox("Colors By Class", Engine::Var::Instance().Esp_Colors_By_Class.Get());
					Gui::CheckBox("Bot Check", Engine::Var::Instance().Esp_Bot_Check.Get());
					//Gui::CheckBox("Team Check", Engine::Var::Instance().Esp_Team_Check.Get());
					Gui::FloatSlider("Max Distance", Engine::Var::Instance().Esp_Max_Distance.Get(), 300.f, 10000.f);
					Gui::FloatSlider("Thickness", Engine::Var::Instance().Esp_Thickness.Get(), 1.f, 4.f);
					Gui::FloatSlider("Outline", Engine::Var::Instance().Esp_Thickness_Outline.Get(), 2.f, 4.f);
					Gui::CheckBox("Tracers", Engine::Var::Instance().Esp_Tracer.Get());
					Gui::CheckBox("Only Visible Traces", Engine::Var::Instance().Only_Visible_Traces.Get());
					Gui::FloatSlider("Tracers Holdtime", Engine::Var::Instance().Tracer_holdtime.Get(), 1.f, 6.f);
					Gui::CheckBox("Esp Team Check", Engine::Var::Instance().Esp_Team_Check.Get());
					Gui::NewGraph();
					Gui::CheckBox("Halo", Engine::Var::Instance().Esp_Halo.Get());
					Gui::FloatSlider("bloom", Engine::Var::Instance().Halo_Bloom.Get(), 1.f, 10.f);
					Gui::CheckBox("Chams", Engine::Var::Instance().Chams.Get());
					Gui::Combo("Type", Engine::Var::Instance().Chams_Type.Get(), CHAR_Chams, ARRAYSIZE(CHAR_Chams), ARRAYSIZE(CHAR_Chams));
					Gui::FloatSlider("blend", Engine::Var::Instance().Chams_Blend.Get(), 0.1f, 1.f);
					Gui::CheckBox("FakeAngle Chams", Engine::Var::Instance().Fake_angle_Chams.Get());

					Gui::Combo("Type", Engine::Var::Instance().Fake_angle_Chams_Type.Get(), CHAR_Chams, ARRAYSIZE(CHAR_Chams), ARRAYSIZE(CHAR_Chams));
					Gui::FloatSlider("blend", Engine::Var::Instance().Fake_angle_Chams_Blend.Get(), 0.1f, 1.f);

					Gui::EndTab();
				}
				if (Gui::BeginTab("Misc", 3)) {
					Gui::CheckBox(Engine::Var::Instance().Bhop.s_Name, Engine::Var::Instance().Bhop.Get());
					Gui::CheckBox("Bhop Safe Mod", Engine::Var::Instance().Bhop_Safe_Mod.Get());
					Gui::CheckBox("Ground Strafe", Engine::Var::Instance().Ground_Strafe.Get());
					Gui::CheckBox("Auto Strafe", Engine::Var::Instance().Auto_Strafe.Get());
					Gui::CheckBox("Strafe Helper", Engine::Var::Instance().Strafe_Helper.Get());
		
					Gui::CheckBox("Debug Camera", Engine::Var::Instance().Debug_Camera.Get());
					Gui::FloatSlider("Speed", Engine::Var::Instance().Debug_Camera_Speed.Get(), 1.f, 100.f);
					Gui::HotKey("Key", Engine::Var::Instance().Debug_Camera_Key.Get());
					if (Engine::Var::Instance().Debug_Camera_Key.Get() == 0)
						Engine::Var::Instance().Debug_Camera_Key.Get() = 0x48;
					Gui::NewGraph();

					Gui::CheckBox("Third Person", Engine::Var::Instance().Third_Person.Get());
					Gui::FloatSlider("Distance", Engine::Var::Instance().Third_Person_Distance.Get(), 1.f, 500.f);
					Gui::HotKey("Key", Engine::Var::Instance().Third_Person_Key.Get());
					if (Engine::Var::Instance().Third_Person_Key.Get() == 0)
						Engine::Var::Instance().Third_Person_Key.Get() = 0x56;
				
					Gui::NewGraph();
					Gui::CheckBox(Engine::Var::Instance().Crosshair.s_Name, Engine::Var::Instance().Crosshair.Get());
					Gui::FloatSlider("Size", Engine::Var::Instance().Crosshair_Size.Get(), 1.f, 20.f);
				

					Gui::CheckBox("Fullbright", Engine::Var::Instance().Fullbright.Get());
					Gui::FloatSlider("Fov", Engine::Var::Instance().Fov_changer.Get(), -65.f, 65.f);

					Gui::FloatSlider("Viewmodel Fov", Engine::Var::Instance().Viewmodel_changer.Get(), -65.f, 65.f);

					//Gui::CheckBox("AirStuck", Engine::Var::Instance().AirStuck.Get());
					//Gui::HotKey("Key", Engine::Var::Instance().AirStuck_Key.Get());
					//if (Engine::Var::Instance().AirStuck_Key.Get() == 0)
					//	Engine::Var::Instance().AirStuck_Key.Get() = 'H';

					Gui::EndTab();
				}
				if(Gui::BeginTab("Target",2))
				{
					struct t_fr {
						C_GMOD_Player* ent;
						player_info_t info;
					};
					std::vector< t_fr > pp = {};
					std::vector<char*> charVecFr_list = {};
						FriendList::Instance().Update();
						auto local = C_GMOD_Player::GetLocalPlayer();
						if (local && local->IsGood()) {

							for (int it = 0; it <= g_pEngine->GetMaxClients(); it++) {//it < g_pEntList->GetMaxEntities()
								auto ent = C_GMOD_Player::GetGPlayer1(it);

								if (!ent)
									continue;
								if (ent == local)
									continue;
								t_fr fr = {};
								fr.ent = ent;
								fr.info = {};
								ZeroMemory(fr.info.name, sizeof(fr.info.name));
								if (!g_pEngine->GetPlayerInfo(it, &fr.info))
									continue;
								if (fr.info.name && fr.ent) {
									pp.push_back(fr);
								}
							}
							if (pp.size()) {


								charVecFr_list = std::vector<char*>(pp.size(), nullptr);
								for (int i = 0; i < pp.size(); i++) {
									charVecFr_list[i] = strdup(pp[i].info.name);
								}
								static int curr_ply = 0;

								Gui::Combo("FriendList", curr_ply, (const char**)charVecFr_list.data(), charVecFr_list.size() > 4 ? 4 : charVecFr_list.size(), charVecFr_list.size());

								if (curr_ply < pp.size()) {
									auto flag_friend = FriendList::Instance().IsFriend(pp[curr_ply].ent);
									if (flag_friend) {
										if (Gui::Button("Delete from friends") && pp[curr_ply].ent) {
											FriendList::Instance().DelFromFriend(pp[curr_ply].ent);
										}
									}
									else {
										if (Gui::Button("Add to friends") && pp[curr_ply].ent) {
											FriendList::Instance().AddToFriend(pp[curr_ply].ent);
										}
									}
								}
							}
						}
												std::vector<Gui::List_st> list_data = {};

						for (auto& it : Teams::g_teams) {
							Gui::List_st curr = {};
							curr.value = &it.whitelisted;
							if (it.Name == "Unknown") {
								curr.name = std::string("Team id ") + std::to_string(it.team_id);
							}
							else {
								curr.name = it.Name;
							}

							list_data.push_back(curr);
						}
						Gui::List("Whitelisted Teams", list_data, 6);

						list_data.clear();
						Gui::HotKey("Add Key", Engine::Var::Instance().PropList_Fast_Add_Key.Get());
						if (Engine::Var::Instance().PropList_Fast_Add_Key.Get() == 0)
							Engine::Var::Instance().PropList_Fast_Add_Key.Get() = 0x56;
						Gui::HotKey("Del Key", Engine::Var::Instance().PropList_Fast_Del_Key.Get());
						if (Engine::Var::Instance().PropList_Fast_Del_Key.Get() == 0)
							Engine::Var::Instance().PropList_Fast_Del_Key.Get() = 0x42;
						Gui::NewGraph();
						


						static auto old_idx = 0;
						static auto c_idx = 0;
						bool* ent_clicked = NULL;
						std::vector<char*> charVecTemp_Ent = {};
						if (EntEsp::Instance().Temp_Ent.size()) {
							charVecTemp_Ent = std::vector<char*>(EntEsp::Instance().Temp_Ent.size(), nullptr);
							for (int i = 0; i < EntEsp::Instance().Temp_Ent.size(); i++) {
								charVecTemp_Ent[i] = strdup(EntEsp::Instance().Temp_Ent[i].c_str());
							}

							ent_clicked = Gui::Combo("Ents", c_idx, (const char**)charVecTemp_Ent.data(), charVecTemp_Ent.size() > 8 ? 8 : charVecTemp_Ent.size(), charVecTemp_Ent.size());
						}
						std::vector<char*> charVecEnt_vec = {};
						auto sucess_ent_vec = 0;
						if (EntEsp::Instance().Ent_vec.size()) {
							charVecEnt_vec = std::vector<char*>(EntEsp::Instance().Ent_vec.size(), nullptr);


							for (int i = 0; i < EntEsp::Instance().Ent_vec.size(); i++) {
								if (!EntEsp::Instance().Ent_vec[i])
									continue;
								if (!EntEsp::Instance().Ent_vec[i]->ExtractAs<bool>())
									continue;
								auto b_vv = EntEsp::Instance().Ent_vec[i]->ExtractAs<bool>();
								if (!b_vv)
									continue;
								charVecEnt_vec[i / 2] = strdup(b_vv->s_Name);
								sucess_ent_vec++;
							}
						}
						static auto EntIdx = 0;
						Core::VarUtil::Var<bool>* var_to_delete = nullptr;
						if (charVecEnt_vec.size()) {
							Gui::Combo("Ents List", EntIdx, (const char**)charVecEnt_vec.data(), sucess_ent_vec > 8 ? 8 : sucess_ent_vec, sucess_ent_vec);
							if (EntIdx * 2 < EntEsp::Instance().Ent_vec.size()) {
								auto& curr = EntEsp::Instance().Ent_vec[EntIdx * 2];
								if (curr->ExtractAs<bool>()) {
									auto b_val = curr->ExtractAs<bool>();
									Gui::CheckBox("Active", b_val->Get());
									auto& curr_clr = EntEsp::Instance().Ent_vec[EntIdx * 2 + 1];
									if (curr_clr->ExtractAs<D3DCOLOR>()) {
										auto c_var = curr_clr->ExtractAs<D3DCOLOR>();
										Gui::ColorPickerButton("Color", c_var->Get());
									}
									if (Gui::Button("Delete")) {
										var_to_delete = b_val;
									}
								}
							}
						}
						
						Gui::CheckBox("Entity Toggle", Engine::Var::Instance().PropList_Toggle.Get());
						Gui::CheckBox("Entity Render Names", Engine::Var::Instance().PropList_Render_Name.Get());
						Gui::FloatSlider("Entity HoldTime", Engine::Var::Instance().PropList_HoldTime.Get(), 0.1f, 8.f);
						Gui::FloatSlider("Entity Distance", Engine::Var::Instance().PropList_Distance.Get(), 300.f, 10000.f);
						auto temps_ = c_idx;
						Gui::EndTab();
						bool changed_index = c_idx != temps_;

						if (var_to_delete) {
							std::string name = var_to_delete->s_Name;
							std::string name_col = std::string(std::string(var_to_delete->s_Name) + "_color");

							auto var = Engine::Var::Instance().EngineVars.FindVar(name.c_str(), "Ent_vec");
							while (var) {
								Engine::Var::Instance().EngineVars.DeleteVar(*var);
								var = Engine::Var::Instance().EngineVars.FindVar(name, "Ent_vec");
							}
							auto iter = 0u;
							for (auto it : EntEsp::Instance().Ent_vec) {
								if (!it || !it->s_Name) {
									iter++;
									continue;
								}
								if (strcmp(it->s_Name, name.c_str()) == 0) {
									EntEsp::Instance().Ent_vec.erase(EntEsp::Instance().Ent_vec.begin() + iter);
									iter--;
								}
								iter++;
							}

							auto var_col = Engine::Var::Instance().EngineVars.FindVar(name_col, "Ent_vec");
							while (var_col) {
								Engine::Var::Instance().EngineVars.DeleteVar(*var_col);
								var_col = Engine::Var::Instance().EngineVars.FindVar(name_col, "Ent_vec");
							}
							iter = 0u;
							for (auto it : EntEsp::Instance().Ent_vec) {
								if (!it || !it->s_Name) {
									iter++;
									continue;
								}
								if (strcmp(it->s_Name, name_col.c_str()) == 0) {
									EntEsp::Instance().Ent_vec.erase(EntEsp::Instance().Ent_vec.begin() + iter);
									iter--;
								}
								iter++;
							}
							EntEsp::Instance().is_vec_change = true;
							EntEsp::Instance().Update_Ent_vec();
							EntEsp::Instance().Force_Update_Temp_Ent();

							var_to_delete = nullptr;
						}
						old_idx = c_idx;


						if (ent_clicked && *ent_clicked) {
							*ent_clicked = false;
							Engine::Var::Instance().EngineVars.BeginGroup("Ent_vec");
							Engine::Var::Instance().EngineVars.RegisterVar(*new Core::VarUtil::Var<bool>, true, EntEsp::Instance().Temp_Ent[c_idx], Core::VarUtil::v_dynamic);
							Engine::Var::Instance().EngineVars.RegisterVar(*new Core::VarUtil::Var<D3DCOLOR>, D3DCOLOR_ARGB(250, 250, 250, 255), std::string(std::string(EntEsp::Instance().Temp_Ent[c_idx]) + "_color").c_str(), Core::VarUtil::v_dynamic);
							Engine::Var::Instance().EngineVars.EndGroup();
							EntEsp::Instance().is_vec_change = true;
							EntEsp::Instance().Update_Ent_vec();
							EntEsp::Instance().Force_Update_Temp_Ent();
						}

						if (charVecTemp_Ent.size()) {
							for (int i = 0; i < charVecTemp_Ent.size(); i++) {
								free(charVecTemp_Ent[i]);
							}
						}
						if (charVecEnt_vec.size()) {
							for (int i = 0; i < charVecEnt_vec.size(); i++) {
								if (charVecEnt_vec[i]) {
									free(charVecEnt_vec[i]);
								}
							}
						}
					
					
				}

				

				if (Gui::BeginTab("Colors", 2)) {
					static bool init = true;
					static std::vector<Core::VarUtil::unk_Var*> Colors;

					if (init) {
						auto temp_static = Engine::Var::Instance().EngineVars.ParseVarFlags(Core::VarUtil::v_static);
						for (auto it : temp_static) {
							if (it->ExtractAs<D3DCOLOR>())
								Colors.push_back(it);
						}
						init = false;
					}
					for (auto it : Colors) {
						auto  bb = it->ExtractAs<D3DCOLOR>();

						Gui::ColorPickerButton(it->s_Name, it->ExtractAs<D3DCOLOR>()->Get());
					}

					Gui::EndTab();
				}

				if (Gui::BeginTab("Settings", 2)) {
					char buff[255];
					wchar_t wch[255];

					auto& engine = Engine::Var::Instance();
					if (!engine.EngineVars.Directory.size()) {
						engine.EngineVars.InitDefaultDirectory();
					}

					static std::string str_directory;

					if (!str_directory.size()) {
						str_directory = engine.EngineVars.Directory;
						str_directory += "*";
					}

					WIN32_FIND_DATAA wfd = {};

					HANDLE const hFind = FindFirstFileA(str_directory.c_str(), &wfd);

					std::vector<char*> Files = {};
					auto SuccessCount = 0u;
					if (hFind != INVALID_HANDLE_VALUE) {
						for (; FindNextFileA(hFind, &wfd) != NULL;) {
							if (!wfd.cFileName)
								continue;

							std::string tFile(wfd.cFileName);

							if (!tFile.size())
								continue;

							if (tFile.find_last_of('.') == -1) {
								continue;
							}
							std::string Format(tFile.substr(tFile.find_last_of('.')));
							if (Format.size() && strcmp(Format.c_str(), ".cfg") == 0) {

								tFile.erase(tFile.find('.'), tFile.size() - tFile.find('.'));

								if (MultiByteToWideChar(CP_ACP, NULL, &tFile.c_str()[0], -1, &wch[0], 255) > 0) {
									if (WideCharToMultiByte(CP_UTF8, NULL, &wch[0], -1, &buff[0], 255, NULL, NULL) > 0) {
										Files.push_back(_strdup(buff));


										SuccessCount++;
									}
								}

							}
						}
						FindClose(hFind);
					}
					static int index = 0;
					auto& vars = Engine::Var::Instance();
					Gui::HotKey(vars.Menu_Key.s_Name, vars.Menu_Key.Get());
					if (vars.Menu_Key.Get() == 0)
						vars.Menu_Key.Get() = VK_INSERT;
					Gui::HotKey(vars.Hide_Cheat_Key.s_Name, vars.Hide_Cheat_Key.Get());
					if (vars.Hide_Cheat_Key.Get() == 0)
						vars.Hide_Cheat_Key.Get() = VK_HOME;

					auto alloc = new char*[SuccessCount];
					for (auto it = 0u; it < SuccessCount; it++) {

						if (Files[it]) {
							alloc[it] = Files[it];
						}
					}
					if (SuccessCount) {
						Gui::Combo("ChoseFile", index, (const char **)Files.data(), SuccessCount > 4 ? 4 : SuccessCount, SuccessCount);

						if (Gui::Button("Load config") && Files.size()) {
							if (MultiByteToWideChar(CP_UTF8, NULL, &Files[index][0], -1, &wch[0], 255) > 0) {
								if (WideCharToMultiByte(CP_ACP, NULL, &wch[0], -1, &buff[0], 255, NULL, NULL) > 0) {
									Engine::Var::Instance().EngineVars.LoadVarFromFile(buff);
									EntEsp::Instance().is_vec_change = true;
									FriendList::Instance().is_vec_change = true;
								}
							}
						}
					}
					static wchar_t textbuff[255];

					Gui::InputBuffer("Config Name", textbuff, ARRAYSIZE(textbuff));//File name : default.txt

					char tttbbb[255];
					ZeroMemory(tttbbb, sizeof(tttbbb));
					sprintf_s(tttbbb, "%ws", textbuff);

					if (Gui::Button("Save config")) {
						if (MultiByteToWideChar(CP_UTF8, NULL, (char*)&tttbbb[0], -1, &wch[0], 255) > 0) {
							if (WideCharToMultiByte(CP_ACP, NULL, &wch[0], -1, &buff[0], 255, NULL, NULL) > 0) {
								Engine::Var::Instance().EngineVars.SaveVarToFile(buff);
							}
						}
					}
					Gui::NewGraph();
					struct t_fr {
						C_GMOD_Player * ent;
						player_info_t info;
					};
					std::vector< t_fr > pp = {};
					std::vector<char*> charVecFr_list = {};
					if (g_pEngine->IsInGame() && !g_pEngine->IsDrawingLoadingImage()) {
						FriendList::Instance().Update();
						auto local = C_GMOD_Player::GetLocalPlayer();
						
							
						
					
					}
					 

					Gui::EndTab();

					delete[] alloc;
					pp.clear();
					if (charVecFr_list.size()) {
						for (int i = 0; i < charVecFr_list.size(); i++) {
							free(charVecFr_list[i]);
						}
					}

					for (auto it : Files) {
						free(it);
					}
					Files.clear();
				}
				if (Gui::BeginTab("GLUA", 1))
				{
					char buff1[255];
					wchar_t wch1[255];

					auto& engine1 = Engine::Var::Instance();
					if (!engine1.EngineVars.Directory.size()) {
						engine1.EngineVars.InitDefaultDirectory();
					}

					static std::string str_directory1;

					if (!str_directory1.size()) {
						str_directory1 = engine1.EngineVars.Directory;
						str_directory1 += "*";
					}

					WIN32_FIND_DATAA wfd1 = {};

					HANDLE const hFind1 = FindFirstFileA(str_directory1.c_str(), &wfd1);

					std::vector<char*> Files1 = {};
					auto SuccessCount1 = 0u;
					if (hFind1 != INVALID_HANDLE_VALUE) {
						for (; FindNextFileA(hFind1, &wfd1) != NULL;) {
							if (!wfd1.cFileName)
								continue;

							std::string tFile1(wfd1.cFileName);

							if (!tFile1.size())
								continue;

							if (tFile1.find_last_of('.') == -1) {
								continue;
							}
							std::string Format1(tFile1.substr(tFile1.find_last_of('.')));
							if (Format1.size() && strcmp(Format1.c_str(), ".lua") == 0) {


								if (MultiByteToWideChar(CP_ACP, NULL, &tFile1.c_str()[0], -1, &wch1[0], 255) > 0) {
									if (WideCharToMultiByte(CP_UTF8, NULL, &wch1[0], -1, &buff1[0], 255, NULL, NULL) > 0) {
										Files1.push_back(_strdup(buff1));


										SuccessCount1++;
									}
								}

							}
						}
						FindClose(hFind1);
					}
					static int index1 = 0;
					auto& vars1 = Engine::Var::Instance();
				

					auto alloc1 = new char* [SuccessCount1];
					for (auto it1 = 0u; it1 < SuccessCount1; it1++) {

						if (Files1[it1]) {
							alloc1[it1] = Files1[it1];
						}
					}
					if (SuccessCount1) {
						Gui::Combo("Scripts", index1, (const char**)Files1.data(), SuccessCount1 > 4 ? 4 : SuccessCount1, SuccessCount1);

						if (Gui::Button("Run Script") && Files1.size()) {

							Render::rororororororor = true;
							//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)start, NULL, NULL, NULL);
							if (MultiByteToWideChar(CP_UTF8, NULL, &Files1[index1][0], -1, &wch1[0], 255) > 0) {
								if (WideCharToMultiByte(CP_ACP, NULL, &wch1[0], -1, &buff1[0], 255, NULL, NULL) > 0) {
									Engine::Var::Instance().gluafile.Get() = buff1;
									std::string code;
									std::string pathappdata = getenv("appdata");
									read_file(code, pathappdata + "\\" + buff1);
									lua_features::add_code_to_run(code);
								}
							}
						}
						else
							Render::rororororororor = false;;
					}
					
					if (Gui::Button("Open directory"))
						ShellExecute(NULL, NULL, get_lua_dir1().c_str(), NULL, NULL, SW_SHOWNORMAL);
					Gui::EndTab();

					delete[] alloc1;
					

					for (auto it : Files1) {
						free(it);
					}
					Files1.clear();
					//Gui::Text(Engine::Var::Instance().gluafile.Get().c_str());
				//	Gui::Text(BoolToString(Engine::Var::Instance().buttonluadown.Get()));
				}
				if (Gui::BeginTab("Credits", 1)) 
				{
					Gui::Text(" KIRAT23#1146 Fix\n Exec#0002 Fix Anticheat Detections\n Special Thanks To voidptr_t#1488\n For vk.com/urbanichka\n Build: 24.08.2021");
			//		Draw::Text_Outline(g_font, "vk.com/urbanichka | Credits: KIRAT23 & Exec", 10.f, 15.f, font_size, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255), false);
				}

				Gui::EndWindow();
			}


		}
		//Gui::EndScene();
		//Hooks::g_Sync.unlock();
	}
}
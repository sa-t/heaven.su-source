#include "hooks.hpp"

#include <tchar.h>
#include <iostream>
#include <d3d9.h>
#include <dinput.h>

#include "..\features\misc\logs.h"
#include "..\features\misc\misc.h"
#include "..\features\visuals\other_esp.h"

#include "../../utils/Render/Render.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment(lib, "freetype.lib")

#include "windowsx.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <thread>
#include "..\menu\menu.h"
#include "../bytes.h"

auto _visible = true;
static auto d3d_init = false;

namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{
	int rage_weapon = 0;
	int legit_weapon = 0;
	bool menu_open = false;
	bool input_shouldListen = false;
	Vector2D mouse_pos = Vector2D(0, 0);
	ButtonCode_t* input_receivedKeyval;

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static auto is_down = true;
		static auto is_clicked = false;

		if (GetAsyncKeyState(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (uMsg == WM_MOUSEMOVE)
			mouse_pos = Vector2D(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		if (is_clicked)
		{
			menu_open = !menu_open;

			if (menu_open && g_ctx.available())
			{
				if (g_ctx.globals.current_weapon != -1)
				{
					if (g_cfg.ragebot.enable)
						rage_weapon = g_ctx.globals.current_weapon;
					else if (g_cfg.legitbot.enabled)
						legit_weapon = g_ctx.globals.current_weapon;
				}
			}
		}

		auto pressed_buttons = false;
		auto pressed_menu_key = uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL;

		if (g_ctx.local()->is_alive() && !pressed_menu_key && !g_ctx.globals.focused_on_input)
			pressed_buttons = true;

		if (!pressed_buttons && d3d_init && menu_open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam) && !input_shouldListen)
			return true;

		if (menu_open && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE) && !input_shouldListen)
			return false;

		return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
	}

	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto original_fn = directx_hook->get_func_address <EndSceneFn>(42);
		return original_fn(pDevice);
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(INIT::Window);
		ImGui_ImplDX9_Init(pDevice);

		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();

		style.WindowMinSize = ImVec2(10, 10);

		ImFontConfig m_config;
		m_config.OversampleH = m_config.OversampleV = 3;
		m_config.PixelSnapH = false;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF,
			0x0400, 0x044F,
			0
		};

		char windows_directory[MAX_PATH];
		GetWindowsDirectory(windows_directory, MAX_PATH);

		CreateDirectory("csgo\\sound", nullptr);
		FILE* file = nullptr;
		file = fopen(crypt_str("csgo\\sound\\hitsound.wav"), crypt_str("wb"));
		fwrite(hitsound, sizeof(unsigned char), sizeof(hitsound), file);
		fclose(file);

		c_menu::get().futura_small = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 15.f, &m_config, ranges);
		c_menu::get().futura_large = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 20.f, &m_config, ranges);

		c_menu::get().g_pinfoFont = io.Fonts->AddFontFromMemoryTTF(MontserratRegular, sizeof(MontserratRegular), 15, NULL, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().weapon_icons = io.Fonts->AddFontFromMemoryTTF((void*)weapons_font, sizeof(weapons_font), 20, &m_config, io.Fonts->GetGlyphRangesDefault());
		c_menu::get().interfaces = io.Fonts->AddFontFromMemoryTTF((void*)interfaces, sizeof(interfaces), 15.f * 1.25, &m_config, ranges);
		c_menu::get().fwefe444 = io.Fonts->AddFontFromMemoryTTF(MontserratMedium, sizeof(MontserratMedium), 17, NULL, io.Fonts->GetGlyphRangesCyrillic());
		ImGui::GetIO().Water = c_menu::get().fwefe444;

		c_menu::get().ico_menu = io.Fonts->AddFontFromMemoryTTF((void*)icomenu, sizeof(icomenu), 20.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());

		c_menu::get().OpenSans = io.Fonts->AddFontFromMemoryTTF((void*)OpenSans_Bold, sizeof(OpenSans_Bold), 14.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().ESP = io.Fonts->AddFontFromMemoryTTF((void*)ESPS, sizeof(ESPS), 8, &m_config, ranges);
		c_menu::get().OpenSansBold = io.Fonts->AddFontFromMemoryTTF((void*)OpenSans_Bold, sizeof(OpenSans_Bold), 18.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().weapons = io.Fonts->AddFontFromMemoryTTF((void*)weapons_font, sizeof(weapons_font), 14.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().new_incons = io.Fonts->AddFontFromMemoryTTF((void*)weapons_font, sizeof(weapons_font), 32.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().Timers = io.Fonts->AddFontFromMemoryTTF((void*)timers, sizeof(timers), 20, &m_config, ranges);
		if (c_menu::get().inferno == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(pDevice, inferno, sizeof(inferno), 32, 32, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &c_menu::get().inferno);

		c_menu::get().TacticSans_Icon_Tab = io.Fonts->AddFontFromMemoryTTF((void*)TacticSans_Icon_Tab_font, sizeof(TacticSans_Icon_Tab_font), 20.f, &m_config, ranges);

		c_menu::get().tab_icons = io.Fonts->AddFontFromMemoryTTF((void*)clarityfont, sizeof(clarityfont), 20.0f, &m_config, ranges);
		c_menu::get().icon = io.Fonts->AddFontFromMemoryTTF((void*)icons, sizeof(icons), 67.f, &m_config, ranges);
		c_menu::get().icons = io.Fonts->AddFontFromMemoryTTF((void*)icons, sizeof(icons), 47.f, &m_config, ranges);

		ImGui_ImplDX9_CreateDeviceObjects();
		d3d_init = true;
	}

	long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region)
	{
		static auto original = directx_hook->get_func_address <PresentFn>(17);
		g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

		if (!d3d_init)
			GUI_Init(device);

		IDirect3DVertexDeclaration9* vertex_dec;
		device->GetVertexDeclaration(&vertex_dec);

		IDirect3DVertexShader9* vertex_shader;
		device->GetVertexShader(&vertex_shader);

		c_menu::get().device = device;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_Render->BeginScene(device);
		g_Render->EndScene();

		c_menu::get().draw(menu_open);

		otheresp::get().spread_crosshair(device);

		misc::get().indicators();
		misc::get().watermark();
		misc::get().spectators_list();
		misc::get().keybinds();

		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		device->SetVertexShader(vertex_shader);
		device->SetVertexDeclaration(vertex_dec);

		return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
	}


	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directx_hook->get_func_address<EndSceneResetFn>(16);

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);

		if (SUCCEEDED(hr))
			ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}

	DWORD original_getforeignfallbackfontname;
	DWORD original_setupbones;
	DWORD original_doextrabonesprocessing;
	DWORD original_standardblendingrules;
	DWORD original_updateclientsideanimation;
	DWORD original_physicssimulate;
	DWORD original_modifyeyeposition;
	DWORD oGetAngles;
	DWORD original_oClampBonesInBBox;
	DWORD otrans;
	DWORD original_calcviewmodelbob;
	DWORD original_processinterpolatedlist;
	DWORD original_clmove;

	vmthook* player_hook;
	vmthook* directx_hook;
	vmthook* client_hook;
	vmthook* clientstate_hook;
	vmthook* engine_hook;
	vmthook* clientmode_hook;
	vmthook* inputinternal_hook;
	vmthook* renderview_hook;
	vmthook* panel_hook;
	vmthook* modelcache_hook;
	vmthook* materialsys_hook;
	vmthook* modelrender_hook;
	vmthook* surface_hook;
	vmthook* bspquery_hook;
	vmthook* prediction_hook;
	vmthook* trace_hook;
	vmthook* filesystem_hook;

	C_HookedEvents hooked_events;
}

void __fastcall hooks::hooked_setkeycodestate(void* thisptr, void* edx, ButtonCode_t code, bool bDown)
{
	static auto original_fn = inputinternal_hook->get_func_address <SetKeyCodeState_t>(91);

	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, bDown);
}

void __fastcall hooks::hooked_setmousecodestate(void* thisptr, void* edx, ButtonCode_t code, MouseCodeState_t state)
{
	static auto original_fn = inputinternal_hook->get_func_address <SetMouseCodeState_t>(92);

	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, state);
}


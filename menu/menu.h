#pragma once
#include "../includes.hpp"

; class c_menu : public singleton<c_menu>
{
public:
	void draw(bool is_open);
	void menu_setup(ImGuiStyle& style);

	float dpi_scale = 1.f;
	float color_buffer[4] = { 1.f, 1.f, 1.f, 1.f };

	ImFont* futura_small;
	ImFont* futura_large;
	ImFont* ico_menu;

	ImFont* TacticSans_Icon_Tab;

	ImFont* tab_icons;
	ImFont* icons;
	ImFont* icon;

	ImFont* OpenSans;
	ImFont* ESP;
	ImFont* OpenSansBold;
	ImFont* weapons;
	ImFont* new_incons;
	ImFont* Timers;
	IDirect3DTexture9* inferno = nullptr;
	ImFont* g_pinfoFont;
	ImFont* weapon_icons;
	ImFont* interfaces;
	ImFont* fwefe444;

	IDirect3DTexture9* logo = nullptr;
	IDirect3DDevice9* device;

private:
	struct
	{
		ImVec2 WindowPadding;
		float  WindowRounding;
		ImVec2 WindowMinSize;
		float  ChildRounding;
		float  PopupRounding;
		ImVec2 FramePadding;
		float  FrameRounding;
		ImVec2 ItemSpacing;
		ImVec2 ItemInnerSpacing;
		ImVec2 TouchExtraPadding;
		float  IndentSpacing;
		float  ColumnsMinSpacing;
		float  ScrollbarSize;
		float  ScrollbarRounding;
		float  GrabMinSize;
		float  GrabRounding;
		float  TabRounding;
		float  TabMinWidthForUnselectedCloseButton;
		ImVec2 DisplayWindowPadding;
		ImVec2 DisplaySafeAreaPadding;
		float  MouseCursorScale;
	} styles;

	bool update_dpi = false;
	bool update_scripts = false;
	void dpi_resize(float scale_factor, ImGuiStyle& style);

	int active_tab_index;
	ImGuiStyle style;
	int width = 850, height = 560;
	float child_height;

	float preview_alpha = 1.f;

	int active_tab;

	int rage_section;
	int legit_section;
	int visuals_section;
	int players_section;
	int misc_section;
	int settings_section;

	int current_profile = 0;

	std::string preview = crypt_str("None");
	
	void rage();
	void legit();
	void anti();
	void visuals();
	void players();
	void world();
	void misc();
	void skins();
	void cfg();
	void lua();

	void tabs();
};
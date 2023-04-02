#include <ShlObj_core.h>
#include <unordered_map>
#include "menu.h"
#include "../menu/ImGui/code_editor.h"
#include "../constchars.h"
#include "../features/misc/logs.h"
#include "widgets.hpp"

#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)

std::vector <std::string> files;
std::vector <std::string> scripts;

auto s = ImVec2{}, p = ImVec2{};

auto selected_script = 0;

static auto menu_setupped = false;
static auto should_update = true;

static auto type = 0;

int weapon;
int tab;
int category;
int Rage_Tab;
int Skins;

auto BeginChildPos = ImVec2(77, 16);
auto BeginChildSize = ImVec2(693, 533);

IDirect3DTexture9* all_skins[36];

std::string get_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return crypt_str("weapon_knife");
			case 1: return crypt_str("weapon_bayonet");
			case 2: return crypt_str("weapon_knife_css");
			case 3: return crypt_str("weapon_knife_skeleton");
			case 4: return crypt_str("weapon_knife_outdoor");
			case 5: return crypt_str("weapon_knife_cord");
			case 6: return crypt_str("weapon_knife_canis");
			case 7: return crypt_str("weapon_knife_flip");
			case 8: return crypt_str("weapon_knife_gut");
			case 9: return crypt_str("weapon_knife_karambit");
			case 10: return crypt_str("weapon_knife_m9_bayonet");
			case 11: return crypt_str("weapon_knife_tactical");
			case 12: return crypt_str("weapon_knife_falchion");
			case 13: return crypt_str("weapon_knife_survival_bowie");
			case 14: return crypt_str("weapon_knife_butterfly");
			case 15: return crypt_str("weapon_knife_push");
			case 16: return crypt_str("weapon_knife_ursus");
			case 17: return crypt_str("weapon_knife_gypsy_jackknife");
			case 18: return crypt_str("weapon_knife_stiletto");
			case 19: return crypt_str("weapon_knife_widowmaker");
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return crypt_str("ct_gloves");
			case 1: return crypt_str("studded_bloodhound_gloves");
			case 2: return crypt_str("t_gloves");
			case 3: return crypt_str("ct_gloves");
			case 4: return crypt_str("sporty_gloves");
			case 5: return crypt_str("slick_gloves");
			case 6: return crypt_str("leather_handwraps");
			case 7: return crypt_str("motorcycle_gloves");
			case 8: return crypt_str("specialist_gloves");
			case 9: return crypt_str("studded_hydra_gloves");
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return crypt_str("knife");
		case 1: return crypt_str("gloves");
		case 2: return crypt_str("weapon_ak47");
		case 3: return crypt_str("weapon_aug");
		case 4: return crypt_str("weapon_awp");
		case 5: return crypt_str("weapon_cz75a");
		case 6: return crypt_str("weapon_deagle");
		case 7: return crypt_str("weapon_elite");
		case 8: return crypt_str("weapon_famas");
		case 9: return crypt_str("weapon_fiveseven");
		case 10: return crypt_str("weapon_g3sg1");
		case 11: return crypt_str("weapon_galilar");
		case 12: return crypt_str("weapon_glock");
		case 13: return crypt_str("weapon_m249");
		case 14: return crypt_str("weapon_m4a1_silencer");
		case 15: return crypt_str("weapon_m4a1");
		case 16: return crypt_str("weapon_mac10");
		case 17: return crypt_str("weapon_mag7");
		case 18: return crypt_str("weapon_mp5sd");
		case 19: return crypt_str("weapon_mp7");
		case 20: return crypt_str("weapon_mp9");
		case 21: return crypt_str("weapon_negev");
		case 22: return crypt_str("weapon_nova");
		case 23: return crypt_str("weapon_hkp2000");
		case 24: return crypt_str("weapon_p250");
		case 25: return crypt_str("weapon_p90");
		case 26: return crypt_str("weapon_bizon");
		case 27: return crypt_str("weapon_revolver");
		case 28: return crypt_str("weapon_sawedoff");
		case 29: return crypt_str("weapon_scar20");
		case 30: return crypt_str("weapon_ssg08");
		case 31: return crypt_str("weapon_sg556");
		case 32: return crypt_str("weapon_tec9");
		case 33: return crypt_str("weapon_ump45");
		case 34: return crypt_str("weapon_usp_silencer");
		case 35: return crypt_str("weapon_xm1014");
		default: return crypt_str("unknown");
		}
	}
}

IDirect3DTexture9* get_skin_preview(const char* weapon_name, const std::string& skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	std::string vpk_path;

	if (strcmp(weapon_name, crypt_str("unknown")) && strcmp(weapon_name, crypt_str("knife")) && strcmp(weapon_name, crypt_str("gloves")))
	{
		if (skin_name.empty() || skin_name == crypt_str("default"))
			vpk_path = crypt_str(std::string("resource/flash/econ/weapons/base_weapons/").append(weapon_name).append(".png"));
		else
			vpk_path = crypt_str(std::string("resource/flash/econ/default_generated/").append(weapon_name).append("_").append(skin_name).append("_light_large.png"));
	}
	else
	{
		if (!strcmp(weapon_name, crypt_str("knife")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
		else if (!strcmp(weapon_name, crypt_str("gloves")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else if (!strcmp(weapon_name, crypt_str("unknown")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");
	}
	const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len];

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		std::string vpk_path;

		if (strstr(weapon_name, crypt_str("bloodhound")) != NULL || strstr(weapon_name, crypt_str("hydra")) != NULL)
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else
			vpk_path = crypt_str(std::string("resource/flash/econ/weapons/base_weapons/").append(weapon_name).append(".png"));

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len];

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}

void c_menu::menu_setup(ImGuiStyle& style)
{
	ImGui::StyleColorsClassic();
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	ImGui::SetNextWindowBgAlpha(min(style.Alpha, 0.94f));

	styles.WindowPadding = style.WindowPadding;
	styles.WindowRounding = style.WindowRounding;
	styles.WindowMinSize = style.WindowMinSize;
	styles.ChildRounding = style.ChildRounding;
	styles.PopupRounding = style.PopupRounding;
	styles.FramePadding = style.FramePadding;
	styles.FrameRounding = style.FrameRounding;
	styles.ItemSpacing = style.ItemSpacing;
	styles.ItemInnerSpacing = style.ItemInnerSpacing;
	styles.TouchExtraPadding = style.TouchExtraPadding;
	styles.IndentSpacing = style.IndentSpacing;
	styles.ColumnsMinSpacing = style.ColumnsMinSpacing;
	styles.ScrollbarSize = style.ScrollbarSize;
	styles.ScrollbarRounding = style.ScrollbarRounding;
	styles.GrabMinSize = style.GrabMinSize;
	styles.GrabRounding = style.GrabRounding;
	styles.TabRounding = style.TabRounding;
	styles.TabMinWidthForUnselectedCloseButton = style.TabMinWidthForUnselectedCloseButton;
	styles.DisplayWindowPadding = style.DisplayWindowPadding;
	styles.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
	styles.MouseCursorScale = style.MouseCursorScale;

	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); i++)
		if (!all_skins[i])
			all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), g_cfg.skins.skinChanger.at(i).skin_name, device);

	menu_setupped = true;
}

void c_menu::dpi_resize(float scale_factor, ImGuiStyle& style)
{
	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	if (styles.TabMinWidthForUnselectedCloseButton != FLT_MAX)
		style.TabMinWidthForUnselectedCloseButton = (styles.TabMinWidthForUnselectedCloseButton * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}

void lua_loaded()
{
	c_lua::get().load_script(selected_script);
	c_lua::get().refresh_scripts();

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	eventlogs::get().add(crypt_str(std::string("Loaded ").append(scripts.at(selected_script)).append(" script")), false);
}

void Refresh_scripts()
{
	c_lua::get().refresh_scripts();
	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}
	eventlogs::get().add(crypt_str("Refresh scripts"), false);
}

void Reload_scripts()
{
	c_lua::get().reload_all_scripts();
	c_lua::get().refresh_scripts();

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}
	eventlogs::get().add(crypt_str(std::string("Reload").append(scripts.at(selected_script)).append(" script")), false);
}

void Unload_script()
{
	c_lua::get().unload_script(selected_script);
	c_lua::get().refresh_scripts();

	eventlogs::get().add(crypt_str(std::string("Unloaded ").append(scripts.at(selected_script)).append(" script")), false);
}

void Open_lua()
{
	std::string folder = crypt_str("C:\\Heaven\\Scripts\\");
	ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

std::string get_config_dir()
{
	std::string folder;
	static TCHAR path[MAX_PATH];

	folder = crypt_str("C:\\Heaven\\");

	CreateDirectory(folder.c_str(), NULL);
	return folder;
}

void load_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), false);
	c_lua::get().unload_all_scripts();

	for (auto& script : g_cfg.scripts.scripts)
		c_lua::get().load_script(c_lua::get().get_script_id(script));

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); ++i)
		all_skins[i] = nullptr;

	g_cfg.scripts.scripts.clear();

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), true);
	cfg_manager->config_files();
	eventlogs::get().add(crypt_str(std::string("Loaded ").append(files.at(g_cfg.selected_config)).append(" config")), false);
}

void save_config()
{
	if (cfg_manager->files.empty())
		return;

	g_cfg.scripts.scripts.clear();

	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto script = c_lua::get().scripts.at(i);

		if (c_lua::get().loaded.at(i))
			g_cfg.scripts.scripts.emplace_back(script);
	}

	cfg_manager->save(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();
	eventlogs::get().add(crypt_str(std::string("Saved ").append(files.at(g_cfg.selected_config)).append(" config")), false);
}

void remove_config()
{
	if (cfg_manager->files.empty())
		return;

	eventlogs::get().add(crypt_str(std::string("Removed ").append(files.at(g_cfg.selected_config)).append(" config")), false);

	cfg_manager->remove(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	files = cfg_manager->files;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

void Open_configs()
{
	std::string folder = crypt_str("C:\\Heaven\\Configs\\");
	ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void add_config()
{
	auto empty = true;

	for (auto current : g_cfg.new_config_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		g_cfg.new_config_name = crypt_str("config");

	eventlogs::get().add(crypt_str(std::string("Added ").append(g_cfg.new_config_name).append(" config")), false);

	if (g_cfg.new_config_name.find(crypt_str(".cfg")) == std::string::npos)
		g_cfg.new_config_name += crypt_str(".cfg");

	cfg_manager->save(g_cfg.new_config_name);
	cfg_manager->config_files();

	g_cfg.selected_config = cfg_manager->files.size() - 1;
	files = cfg_manager->files;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 4, 3);
}

bool LabelClick2(const char* label, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

	if (pressed || hovered)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(33 / 255.f, 33 / 255.f, 33 / 255.f, g_ctx.gui.pop_anim * 0.85f));
	if (label_size.x > 0.0f)

		ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);

	if (pressed || hovered)
		ImGui::PopStyleColor();

	return pressed;

}

bool draw_lua_button(const char* label, const char* label_id, bool load, bool save, int curr_config, bool create = false)
{
	bool pressed = false;
	ImGui::SetCursorPosX(8);
	if (ImGui::PlusButton(label, 0, ImVec2(240, 26), label_id, ImColor(25, 25, 25, 225), (25, 25, 25, 225)))
		selected_script = curr_config;

	static char config_name[36] = "\0";
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupRounding, 4);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(33 / 255.f, 33 / 255.f, 33 / 255.f, g_ctx.gui.pop_anim * 0.85f));
	if (ImGui::BeginPopup(label_id, ImGuiWindowFlags_NoMove))
	{
		ImGui::SetNextItemWidth(min(g_ctx.gui.pop_anim, 0.01f) * ImGui::GetFrameHeight() * 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_ctx.gui.pop_anim);

		if (!create)
		{
			if (LabelClick2(crypt_str("Load"), label_id))
				lua_loaded();

			if (LabelClick2(crypt_str("Unload"), label_id))
				Unload_script();

			if (LabelClick2(crypt_str("Reload"), label_id))
				Reload_scripts();
		}
		else
		{
			ImGui::SetCursorPosX(8);
			if (ImGui::CustomButton(crypt_str("Refresh"), crypt_str("##REFRESH_SCRIPTS"), ImVec2(193, 26 * 1)))
				Refresh_scripts();
			ImGui::SetCursorPosX(8);
			if (ImGui::CustomButton(crypt_str("Open lua folder"), crypt_str("##OPEN_LUA"), ImVec2(193, 26 * 1)))
				Open_lua();
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(1);

	return pressed;
}

bool draw_config_button(const char* label, const char* label_id, bool load, bool save, int curr_config, bool create = false)
{
	bool pressed = false;
	ImGui::SetCursorPosX(8);
	if (ImGui::PlusButton(label, 0, ImVec2(240, 26), label_id, ImColor(25, 25, 25, 225), (25, 25, 25, 225)))g_cfg.selected_config = curr_config;

	static char config_name[36] = "\0";
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupRounding, 4);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(33 / 255.f, 33 / 255.f, 33 / 255.f, g_ctx.gui.pop_anim * 0.85f));
	if (ImGui::BeginPopup(label_id, ImGuiWindowFlags_NoMove))
	{
		ImGui::SetNextItemWidth(min(g_ctx.gui.pop_anim, 0.01f) * ImGui::GetFrameHeight() * 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_ctx.gui.pop_anim);
		if (!create)
		{
			if (LabelClick2(crypt_str("Load"), label_id))
				load_config();

			if (LabelClick2(crypt_str("Save"), label_id))
				save_config();

			if (LabelClick2(crypt_str("Delete"), label_id))
				remove_config();

			if (LabelClick2(crypt_str("Open configs folder"), label_id))
				Open_configs();
		}
		else
		{
			ImGui::SetCursorPosX(8);
			ImGui::Text("Input new config name");
			ImGui::SetCursorPosX(8);
			ImGui::PushItemWidth(193);
			ImGui::InputText(crypt_str("##configname"), config_name, sizeof(config_name));
			ImGui::PopItemWidth();

			ImGui::SetCursorPosX(8);
			auto new_label = std::string("Add ").append(config_name);
			if (ImGui::CustomButton(new_label.c_str(), crypt_str("##CONFIG__CREATE"), ImVec2(193, 26 * 1)))
			{
				g_cfg.new_config_name = config_name;
				add_config();
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(1);
	return pressed;
}

__forceinline void padding(float x, float y)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x * c_menu::get().dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y * c_menu::get().dpi_scale);
}

void draw_combo(const char* name, int& variable, const char* labels[], int count)
{
	ImGui::SetCursorPosX(13);
	ImGui::Text(name);

	ImGui::Combo(std::string(crypt_str("##COMBO__") + std::string(name)).c_str(), &variable, labels, count);
}

void draw_combo(const char* name, int& variable, bool (*items_getter)(void*, int, const char**), void* data, int count)
{
	ImGui::SetCursorPosX(13);
	ImGui::Text(name);

	ImGui::Combo(std::string(crypt_str("##COMBO__") + std::string(name)).c_str(), &variable, items_getter, data, count);
}

void draw_multicombo(std::string name, std::vector<int>& variable, const char* labels[], int count, std::string& preview)
{
	padding(-3, -6);
	ImGui::SetCursorPosX(13);

	ImGui::Text((crypt_str(" ") + name).c_str());

	padding(0, -5);

	auto hashname = crypt_str("##") + name;

	for (auto i = 0, j = 0; i < count; i++)
	{
		if (variable[i])
		{
			if (j)
				preview += crypt_str(", ") + (std::string)labels[i];
			else
				preview = labels[i];

			j++;
		}
	}
	ImGui::Spacing();

	if (ImGui::BeginCombo(hashname.c_str(), preview.c_str()))
	{
		ImGui::Spacing();
		ImGui::BeginGroup();
		{
			for (auto i = 0; i < count; i++)
				ImGui::Selectable(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);
		}
		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::EndCombo();
	}

	preview = crypt_str("None");
}

bool LabelClick(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (*v)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(25 / 255.f, 25 / 255.f, 25 / 225.f, 225.f));
	if (label_size.x > 0.0f)

		ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);

	if (*v)
		ImGui::PopStyleColor();

	return pressed;

}

void draw_keybind(const char* label, key_bind* key_bind, const char* unique_id)
{
	if (key_bind->key == KEY_ESCAPE)
		key_bind->key = KEY_NONE;

	auto clicked = false;
	auto text = (std::string)m_inputsys()->ButtonCodeToString(key_bind->key);

	if (key_bind->key <= KEY_NONE || key_bind->key >= KEY_MAX)
		text = crypt_str("None");

	if (hooks::input_shouldListen && hooks::input_receivedKeyval == &key_bind->key)
	{
		clicked = true;
		text = crypt_str("...");
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * c_menu::get().dpi_scale;

	if (ImGui::KeybindButton(text.c_str(), label, unique_id, clicked))
		clicked = true;


	if (clicked)
	{
		hooks::input_shouldListen = true;
		hooks::input_receivedKeyval = &key_bind->key;
	}

	static auto hold = false, toggle = false;

	switch (key_bind->mode)
	{
	case HOLD:
		hold = true;
		toggle = false;
		break;
	case TOGGLE:
		toggle = true;
		hold = false;
		break;
	}

	if (g_cfg.scripts.developer_mode && ImGui::IsItemHovered())
	{
		for (auto& item : cfg_manager->items)
		{
			if (key_bind == item->pointer)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
				ImGui::SetTooltip(item->name.c_str());
				ImGui::PopStyleVar();
				break;
			}
		}
	}

	if (ImGui::BeginPopup(unique_id))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6 * c_menu::get().dpi_scale);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Hold")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);

		if (LabelClick(crypt_str("Hold"), &hold, unique_id))
		{
			if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else
			{
				toggle = false;
				key_bind->mode = HOLD;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Toggle")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9 * c_menu::get().dpi_scale);

		if (LabelClick(crypt_str("Toggle"), &toggle, unique_id))
		{
			if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void c_menu::rage()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Rage", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("AimBot", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				if (g_cfg.ragebot.enable)
					g_cfg.legitbot.enabled = false;
				g_cfg.ragebot.zeus_bot = true;
				g_cfg.ragebot.knife_bot = true;

				ImGui::Checkbox(crypt_str("Enabled"), &g_cfg.ragebot.enable);
				const char* rage_weapons[8] = { crypt_str("Revolver/Deagle"), crypt_str("Pistols"), crypt_str("SMGs"), crypt_str("Rifles"), crypt_str("Auto"), crypt_str("Scout"), crypt_str("AWP"), crypt_str("Heavy") };

				ImGui::Combo(crypt_str("Rage weapon"), &hooks::rage_weapon, rage_weapons, ARRAYSIZE(rage_weapons));
				ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.ragebot.field_of_view, 1, 180);
				ImGui::Checkbox(crypt_str("Silent aim"), &g_cfg.ragebot.silent_aim);
				ImGui::Checkbox(crypt_str("Compensate recoil"), &g_cfg.ragebot.remove_recoil);
				ImGui::Checkbox(crypt_str("Automatic awall"), &g_cfg.ragebot.autowall);
				ImGui::Checkbox(crypt_str("Automatic fire"), &g_cfg.ragebot.autoshoot);
				ImGui::Checkbox(crypt_str("Automatic scope"), &g_cfg.ragebot.autoscope);
			}
			widgets::end_child();

			widgets::begin_child("Other", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Static point scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale);
				if (g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale)
				{
					ImGui::SliderFloat(crypt_str("Head scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale ? crypt_str("%.2f") : crypt_str("None"));
					ImGui::SliderFloat(crypt_str("Body scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale ? crypt_str("%.2f") : crypt_str("None"));
				}

				ImGui::Checkbox(crypt_str("Enable max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses);

				if (g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses)
					ImGui::SliderInt(crypt_str("Max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses_amount, 0, 6);
			}
			widgets::end_child();

			widgets::begin_child("Auto peek", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_keybind(crypt_str("Auto peek"), &g_cfg.misc.automatic_peek, crypt_str("##AUTOPEEK__HOTKEY"));
				ImGui::ColorEdit(crypt_str("Automatic peek color"), &g_cfg.misc.automatic_peek_color, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
				ImGui::SliderInt(crypt_str("Auto peek speed"), &g_cfg.misc.automatic_peek_speed, 1, 10);
				ImGui::SliderInt(crypt_str("Auto peek radius"), &g_cfg.misc.automatic_peek_radius, 10, 20);
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("General", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_multicombo(crypt_str("Hitboxes"), g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes, hitboxes, ARRAYSIZE(hitboxes), preview);

				ImGui::Checkbox(crypt_str("Hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance);
				if (g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance)
					ImGui::SliderInt(crypt_str("Hitchance amount"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance_amount, 1, 100);

				if (g_cfg.ragebot.double_tap && hooks::rage_weapon <= 4)
				{
					ImGui::Checkbox(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance);
					if (g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance)
						ImGui::SliderInt(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance_amount, 1, 100);
				}

				draw_keybind(crypt_str("Damage override"), &g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key, crypt_str("##HOTKEY__DAMAGE_OVERRIDE"));

				if (g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE && g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
					ImGui::SliderInt(crypt_str("Minimum override damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_override_damage, 1, 120);

				ImGui::SliderInt(crypt_str("Minimum damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_visible_damage, 1, 120);

				if (g_cfg.ragebot.autowall)
					ImGui::SliderInt(crypt_str("Minimum wall damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_damage, 1, 120);
			}
			widgets::end_child();

			widgets::begin_child("Accuracy", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Automatic stop"), &g_cfg.ragebot.weapon[hooks::rage_weapon].autostop);

				if (g_cfg.ragebot.weapon[hooks::rage_weapon].autostop)
					draw_multicombo(crypt_str("Modifiers"), g_cfg.ragebot.weapon[hooks::rage_weapon].autostop_modifiers, autostop_modifiers, ARRAYSIZE(autostop_modifiers), preview);

				draw_keybind(crypt_str("Force safe points"), &g_cfg.ragebot.safe_point_key, crypt_str("##HOKEY_FORCE_SAFE_POINTS"));
				draw_keybind(crypt_str("Force body aim"), &g_cfg.ragebot.body_aim_key, crypt_str("##HOKEY_FORCE_BODY_AIM"));
			}
			widgets::end_child();

			widgets::begin_child("Exploits", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Double tap"), &g_cfg.ragebot.double_tap);

				if (g_cfg.ragebot.double_tap)
					draw_keybind(crypt_str("Double tap key"), &g_cfg.ragebot.double_tap_key, crypt_str("##HOTKEY_DT"));

				ImGui::Checkbox(crypt_str("Hide shots"), &g_cfg.antiaim.hide_shots);

				if (g_cfg.antiaim.hide_shots)
					draw_keybind(crypt_str("Hide shots key"), &g_cfg.antiaim.hide_shots_key, crypt_str("##HOTKEY_HIDESHOTS"));
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::legit()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Legit", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Legit", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				if (g_cfg.legitbot.enabled)
					g_cfg.ragebot.enable = false;
				g_cfg.ragebot.zeus_bot = false;
				g_cfg.ragebot.knife_bot = false;

				ImGui::Checkbox(crypt_str("Friendly fire"), &g_cfg.legitbot.friendly_fire);
				ImGui::Checkbox(crypt_str("Automatic pistols"), &g_cfg.legitbot.autopistol);

				ImGui::Checkbox(crypt_str("Automatic scope"), &g_cfg.legitbot.autoscope);

				if (g_cfg.legitbot.autoscope)
					ImGui::Checkbox(crypt_str("Unscope after shot"), &g_cfg.legitbot.unscope);

				ImGui::Checkbox(crypt_str("Snipers in zoom only"), &g_cfg.legitbot.sniper_in_zoom_only);

				ImGui::Checkbox(crypt_str("Aim if in air"), &g_cfg.legitbot.do_if_local_in_air);
				ImGui::Checkbox(crypt_str("Aim if flashed"), &g_cfg.legitbot.do_if_local_flashed);
				ImGui::Checkbox(crypt_str("Aim thru smoke"), &g_cfg.legitbot.do_if_enemy_in_smoke);

				draw_keybind(crypt_str("Automatic fire key"), &g_cfg.legitbot.autofire_key, crypt_str("##HOTKEY_AUTOFIRE_LGBT_KEY"));
				ImGui::SliderInt(crypt_str("Automatic fire delay"), &g_cfg.legitbot.autofire_delay, 0, 12, (!g_cfg.legitbot.autofire_delay ? crypt_str("None") : (g_cfg.legitbot.autofire_delay == 1 ? crypt_str("%d tick") : crypt_str("%d ticks"))));

				draw_combo(crypt_str("RCS type"), g_cfg.legitbot.weapon[hooks::legit_weapon].rcs_type, RCSType, ARRAYSIZE(RCSType));
				ImGui::SliderFloat(crypt_str("RCS amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].rcs, 0.f, 100.f, crypt_str("%.0f%%"));

				if (g_cfg.legitbot.weapon[hooks::legit_weapon].rcs > 0)
				{
					ImGui::SliderFloat(crypt_str("RCS custom FOV"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov ? crypt_str("None") : crypt_str("%.2f")));
					ImGui::SliderFloat(crypt_str("RCS Custom smooth"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth, 0.f, 12.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth ? crypt_str("None") : crypt_str("%.1f")));
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("Settings", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				const char* hitbox_legit[3] = { crypt_str("Closest"), crypt_str("Head"), crypt_str("Body") };
				draw_combo(crypt_str("Hitbox"), g_cfg.legitbot.weapon[hooks::legit_weapon].priority, hitbox_legit, ARRAYSIZE(hitbox_legit));
				ImGui::SliderFloat(crypt_str("Maximum FOV amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov, 0.f, 30.f, crypt_str("%.2f"));
				ImGui::SliderFloat(crypt_str("Silent FOV"), &g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov ? crypt_str("None") : crypt_str("%.2f")));
				ImGui::SliderFloat(crypt_str("Smooth amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].smooth, 1.f, 12.f, crypt_str("%.1f"));
				ImGui::SliderInt(crypt_str("Automatic wall damage"), &g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg, 0, 100, (!g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg ? crypt_str("None") : crypt_str("%d")));
				ImGui::SliderInt(crypt_str("Automatic fire hitchance"), &g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance, 0, 100, (!g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance ? crypt_str("None") : crypt_str("%d")));
				ImGui::SliderFloat(crypt_str("Target switch delay"), &g_cfg.legitbot.weapon[hooks::legit_weapon].target_switch_delay, 0.f, 1.f);
				ImGui::Checkbox(crypt_str("Automatic stop"), &g_cfg.legitbot.weapon[hooks::legit_weapon].auto_stop);
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::anti()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##AntiAim", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("AntiAim", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				static auto type = 0;
				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.antiaim.enable);

				draw_keybind("Invert desync", &g_cfg.antiaim.flip_desync, "##HOTKEY_INVERT_DESYNC");
				draw_keybind("Manual back", &g_cfg.antiaim.manual_back, "##HOTKEY_INVERT_BACK");
				draw_keybind("Manual left", &g_cfg.antiaim.manual_left, "##HOTKEY_INVERT_LEFT");
				draw_keybind("Manual right", &g_cfg.antiaim.manual_right, "##HOTKEY_INVERT_RIGHT");
				if (g_cfg.antiaim.manual_back.key > KEY_NONE && g_cfg.antiaim.manual_back.key < KEY_MAX || g_cfg.antiaim.manual_left.key > KEY_NONE && g_cfg.antiaim.manual_left.key < KEY_MAX || g_cfg.antiaim.manual_right.key > KEY_NONE && g_cfg.antiaim.manual_right.key < KEY_MAX)
				{
					ImGui::Checkbox(crypt_str("Manuals indicator"), &g_cfg.antiaim.flip_indicator);
					ImGui::ColorEdit(crypt_str("Manuals indicator color"), &g_cfg.antiaim.flip_indicator_color, ALPHA);
				}
			}
			widgets::end_child();

			widgets::begin_child("FakeLag", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Fake-lag"), &g_cfg.antiaim.fakelag);
				if (g_cfg.antiaim.fakelag)
				{
					draw_combo(crypt_str("Fake-lag type"), g_cfg.antiaim.fakelag_type, fakelags, ARRAYSIZE(fakelags));
					ImGui::SliderInt(crypt_str("Limit"), &g_cfg.antiaim.fakelag_amount, 1, 14);

					draw_multicombo(crypt_str("Fake-lag triggers"), g_cfg.antiaim.fakelag_enablers, lagstrigger, ARRAYSIZE(lagstrigger), preview);

					auto enabled_fakelag_triggers = false;

					for (auto i = 0; i < ARRAYSIZE(lagstrigger); i++)
						if (g_cfg.antiaim.fakelag_enablers[i])
							enabled_fakelag_triggers = true;

					if (enabled_fakelag_triggers)
						ImGui::SliderInt(crypt_str("Triggers limit"), &g_cfg.antiaim.triggers_fakelag_amount, 1, 14);
				}
			}
			widgets::end_child();

			widgets::begin_child("Other", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Fake duck"), &g_cfg.misc.noduck);
				if (&g_cfg.misc.noduck)
					draw_keybind(crypt_str("Faked duck key"), &g_cfg.misc.fakeduck_key, crypt_str("##FAKEDUCK__HOTKEY"));
				draw_keybind(crypt_str("Slow-walk"), &g_cfg.misc.slowwalk_key, crypt_str("##SLOWWALK__HOTKEY"));
				const char* legs[] =
				{
					"Avoid slide",
					"Slide",
					"Break"
				};
				draw_combo("Leg movement", g_cfg.misc.slidewalk, legs, ARRAYSIZE(legs));
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("Angles", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_combo(crypt_str("Pitch"), g_cfg.antiaim.pitch, pitch, ARRAYSIZE(pitch));
				draw_combo(crypt_str("Yaw"), g_cfg.antiaim.yaw, yaw, ARRAYSIZE(yaw));
				draw_combo(crypt_str("Base angles"), g_cfg.antiaim.base_angle, baseangle, ARRAYSIZE(baseangle));

				if (g_cfg.antiaim.yaw)
				{
					ImGui::SliderInt(g_cfg.antiaim.yaw == 1 ? crypt_str("Jitter range") : crypt_str("Spin range"), &g_cfg.antiaim.range, 1, 180);

					if (g_cfg.antiaim.yaw == 2)
						ImGui::SliderInt(crypt_str("Spin speed"), &g_cfg.antiaim.speed, 1, 15);
				}

				draw_combo(crypt_str("Desync"), g_cfg.antiaim.desync, desync, ARRAYSIZE(desync));


				if (g_cfg.antiaim.desync)
				{
					if (type == ANTIAIM_STAND)
					{
						draw_combo(crypt_str("LBY type"), g_cfg.antiaim.lby_type, lby_type, ARRAYSIZE(lby_type));
					}

					if (type != ANTIAIM_STAND || !g_cfg.antiaim.lby_type)
					{
						ImGui::SliderInt(crypt_str("Desync range"), &g_cfg.antiaim.desync_range, 1, 100, "%.0f%%");
						ImGui::SliderInt(crypt_str("Inverted desync range"), &g_cfg.antiaim.inverted_desync_range, 1, 100, "%.0f%%");
						ImGui::SliderInt(crypt_str("Body lean"), &g_cfg.antiaim.body_lean, 0, 100);
						ImGui::SliderInt(crypt_str("Inverted body lean"), &g_cfg.antiaim.inverted_body_lean, 0, 100);
					}
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::players()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Players", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Players", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.player.enable);
				const char* team_types[3] = { crypt_str("Enemy"), crypt_str("Team"), crypt_str("Local") };
				ImGui::Combo(crypt_str("Team type"), &g_cfg.player.team_type, team_types, ARRAYSIZE(team_types));

				if (g_cfg.player.team_type == 0)
					category = ENEMY;
				if (g_cfg.player.team_type == 1)
					category = TEAM;
				if (g_cfg.player.team_type == 2)
					category = LOCAL;

				if (category == ENEMY)
				{
					ImGui::Checkbox(crypt_str("OOF arrows"), &g_cfg.player.arrows);
					ImGui::ColorEdit(crypt_str("OOF arrows color"), &g_cfg.player.arrows_color, ALPHA);

					if (g_cfg.player.arrows)
					{
						ImGui::SliderInt(crypt_str("Arrows distance"), &g_cfg.player.distance, 0, 100);
						ImGui::SliderInt(crypt_str("Arrows size"), &g_cfg.player.size, 0, 100);
					}
				}

				ImGui::Checkbox(crypt_str("Bounding box"), &g_cfg.player.type[category].box);
				ImGui::ColorEdit(crypt_str("Bounding box color"), &g_cfg.player.type[category].box_color, ALPHA);

				ImGui::Checkbox(crypt_str("Name"), &g_cfg.player.type[category].name);
				ImGui::ColorEdit(crypt_str("Name color"), &g_cfg.player.type[category].name_color, ALPHA);

				ImGui::Checkbox(crypt_str("Health bar"), &g_cfg.player.type[category].health);
				ImGui::ColorEdit(crypt_str("Health bar color"), &g_cfg.player.type[category].health_color, ALPHA);

				for (auto i = 0, j = 0; i < ARRAYSIZE(flags); i++)
				{
					if (g_cfg.player.type[category].flags[i])
					{
						if (j)
							preview += crypt_str(", ") + (std::string)flags[i];
						else
							preview = flags[i];

						j++;
					}
				}

				draw_multicombo(crypt_str("Flags"), g_cfg.player.type[category].flags, flags, ARRAYSIZE(flags), preview);
				draw_multicombo(crypt_str("Weapon"), g_cfg.player.type[category].weapon, weaponplayer, ARRAYSIZE(weaponplayer), preview);

				if (g_cfg.player.type[category].weapon[WEAPON_ICON] || g_cfg.player.type[category].weapon[WEAPON_TEXT])
				{
					ImGui::ColorEdit(crypt_str("Weapon Color"), &g_cfg.player.type[category].weapon_color, ALPHA);
				}

				ImGui::Checkbox(crypt_str("Ammo bar"), &g_cfg.player.type[category].ammo);
				ImGui::ColorEdit(crypt_str("Ammo bar color"), &g_cfg.player.type[category].ammobar_color, ALPHA);

				ImGui::Checkbox(crypt_str("Footsteps"), &g_cfg.player.type[category].footsteps);
				ImGui::ColorEdit(crypt_str("Footsteps Color"), &g_cfg.player.type[category].footsteps_color, ALPHA);

				if (g_cfg.player.type[category].footsteps)
				{
					ImGui::SliderInt(crypt_str("Thickness"), &g_cfg.player.type[category].thickness, 0, 10);
					ImGui::SliderInt(crypt_str("Radius"), &g_cfg.player.type[category].radius, 50, 500);
				}

				if (category == ENEMY || category == TEAM)
				{
					ImGui::Checkbox(crypt_str("Snap lines"), &g_cfg.player.type[category].snap_lines);
					ImGui::ColorEdit(crypt_str("Snap color"), &g_cfg.player.type[category].snap_lines_color, ALPHA);
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("General", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				if (category == LOCAL)
					draw_combo(crypt_str("Type"), g_cfg.player.local_chams_type, local_chams_type, ARRAYSIZE(local_chams_type));

				if (category != LOCAL || !g_cfg.player.local_chams_type)
					draw_multicombo(crypt_str("Chams"), g_cfg.player.type[category].chams, g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] ? chamsvisact : chamsvis, g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] ? ARRAYSIZE(chamsvisact) : ARRAYSIZE(chamsvis), preview);

				if (g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] || category == LOCAL && g_cfg.player.local_chams_type)
				{
					if (category == LOCAL && g_cfg.player.local_chams_type == 1)
					{
						ImGui::Checkbox(crypt_str("Enable desync chams"), &g_cfg.player.fake_chams_enable);
						ImGui::Checkbox(crypt_str("Visualize fake-lag"), &g_cfg.player.visualize_lag);
						ImGui::Checkbox(crypt_str("Layered"), &g_cfg.player.layered);

						draw_combo(crypt_str("Player chams material"), g_cfg.player.fake_chams_type, chamstype, ARRAYSIZE(chamstype));

						ImGui::ColorEdit(crypt_str("Fake chams color"), &g_cfg.player.fake_chams_color, ALPHA);

						ImGui::Checkbox(crypt_str("Transparency in scope"), &g_cfg.player.transparency_in_scope);

						if (g_cfg.player.transparency_in_scope)
							ImGui::SliderFloat(crypt_str("Alpha"), &g_cfg.player.transparency_in_scope_amount, 0.0f, 1.0f);
					}
					else if (category == LOCAL && g_cfg.player.local_chams_type == 2)
					{

						ImGui::Checkbox(crypt_str("Enable"), &g_cfg.esp.arms_chams);
						ImGui::ColorEdit(crypt_str("Arms Color"), &g_cfg.esp.arms_chams_color, ALPHA);
						draw_combo(crypt_str("Material"), g_cfg.esp.arms_chams_type, chamstype, ARRAYSIZE(chamstype));
						ImGui::Checkbox(crypt_str("Add glow overlay"), &g_cfg.esp.arms_double_material);
						ImGui::ColorEdit(crypt_str("Arms double material color"), &g_cfg.esp.arms_double_material_color, ALPHA);
					}
					else if (category == LOCAL && g_cfg.player.local_chams_type == 3)
					{
						ImGui::Checkbox(crypt_str("Enable"), &g_cfg.esp.weapon_chams);
						ImGui::ColorEdit(crypt_str("Weapon chams colors"), &g_cfg.esp.weapon_chams_color, ALPHA);
						draw_combo(crypt_str("Material"), g_cfg.esp.weapon_chams_type, chamstype, ARRAYSIZE(chamstype));
						ImGui::Checkbox(crypt_str("Add glow overlay"), &g_cfg.esp.weapon_double_material);
						ImGui::ColorEdit(crypt_str("Weapon double material color"), &g_cfg.esp.weapon_double_material_color, ALPHA);
					}
					else
					{
						draw_combo(crypt_str("Player chams material"), g_cfg.player.type[category].chams_type, chamstype, ARRAYSIZE(chamstype));

						if (g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE])
						{
							ImGui::ColorEdit(crypt_str("Chams Visible"), &g_cfg.player.type[category].chams_color, ALPHA);
						}

						if (g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[category].chams[PLAYER_CHAMS_INVISIBLE])
						{
							ImGui::ColorEdit(crypt_str("Chams Invisible"), &g_cfg.player.type[category].xqz_color, ALPHA);
						}

						if (category == ENEMY)
						{
							if (category == 0 || ENEMY || category == 1 || TEAM || category == 2 || LOCAL)
							{
								ImGui::Checkbox(crypt_str("Glow"), &g_cfg.player.type[category].glow);
								ImGui::ColorEdit(crypt_str("Glow color"), &g_cfg.player.type[category].glow_color, ALPHA);

								if (g_cfg.player.type[category].glow)
									draw_combo(crypt_str("Glow type"), g_cfg.player.type[category].glow_type, glowtype, ARRAYSIZE(glowtype));
							}
						}
					}
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::visuals()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Visuals", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Visuals", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.player.enable);
				ImGui::Checkbox(crypt_str("Rain"), &g_cfg.esp.rain);
				ImGui::Checkbox(crypt_str("Gravity ragdolls"), &g_cfg.misc.ragdolls);
				ImGui::Checkbox(crypt_str("Full bright"), &g_cfg.esp.bright);

				draw_combo(crypt_str("Skybox"), g_cfg.esp.skybox, skybox, ARRAYSIZE(skybox));

				ImGui::ColorEdit(crypt_str("Skybox Color"), &g_cfg.esp.skybox_color, NOALPHA);

				ImGui::Checkbox(crypt_str("Color modulation"), &g_cfg.esp.nightmode);

				if (g_cfg.esp.nightmode)
				{
					ImGui::ColorEdit(crypt_str("World Color"), &g_cfg.esp.world_color, ALPHA);
					ImGui::ColorEdit(crypt_str("Props Color"), &g_cfg.esp.props_color, ALPHA);
				}
			}
			widgets::end_child();

			widgets::begin_child("Trail", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Combo("Trail type", &g_cfg.misc.trail_types, "None\0Particles\0Line\0");

				if (g_cfg.misc.trail_types == 1)
					ImGui::Checkbox(crypt_str("Movement trail"), &g_cfg.misc.movement_trails);

				if (g_cfg.misc.trail_types == 2)
				{
					ImGui::Checkbox(crypt_str("Movement trail"), &g_cfg.misc.movement_trails);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##movement_trail"), &g_cfg.misc.trail_color, ALPHA);

					ImGui::Checkbox(crypt_str("Rainbow trail"), &g_cfg.misc.rainbow_trails);
				}
			}
			widgets::end_child();

			widgets::begin_child("Render", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_multicombo(crypt_str("Removals"), g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);

				if (g_cfg.esp.removals[REMOVALS_ZOOM])
					ImGui::Checkbox(crypt_str("Fix zoom sensivity"), &g_cfg.esp.fix_zoom_sensivity);

				ImGui::Checkbox(crypt_str("Disable panorama blur"), &g_cfg.esp.disablepanoramablur);
				ImGui::Checkbox(crypt_str("Bomb indicator"), &g_cfg.esp.bomb_timer);

				ImGui::Checkbox(crypt_str("Damage marker"), &g_cfg.esp.damage_marker);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##Damagemarkercolor"), &g_cfg.esp.damage_marker_color, ALPHA);

				ImGui::Checkbox(crypt_str("Sniper crosshair "), &g_cfg.esp.sniper_crosshair);
				ImGui::Checkbox(crypt_str("Penetration crosshair"), &g_cfg.esp.penetration_reticle);
			}
			widgets::end_child();

			widgets::begin_child("World", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("World modulation"), &g_cfg.esp.world_modulation);
				if (g_cfg.esp.world_modulation)
				{
					ImGui::SliderFloat(crypt_str("Bloom"), &g_cfg.esp.bloom, 0.0f, 750.0f);
					ImGui::SliderFloat(crypt_str("Exposure"), &g_cfg.esp.exposure, 0.0f, 2000.0f);
					ImGui::SliderFloat(crypt_str("Ambient"), &g_cfg.esp.ambient, 0.0f, 1500.0f);
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("Grenade", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Grenade prediction"), &g_cfg.esp.grenade_prediction);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##grenpredcolor"), &g_cfg.esp.grenade_prediction_color, ALPHA);
				ImGui::ColorEdit(crypt_str("Grenade prediction can hit"), &g_cfg.esp.grenade_prediction_color_can_hit, ALPHA);

				if (g_cfg.esp.grenade_prediction)
				{
					ImGui::Checkbox(crypt_str("On click"), &g_cfg.esp.on_click);
					ImGui::ColorEdit(crypt_str("Tracer Color"), &g_cfg.esp.grenade_prediction_tracer_color, ALPHA);
				}

				ImGui::Checkbox(crypt_str("Grenade proximity warning"), &g_cfg.esp.grenade_proximity_warning);

				if (g_cfg.esp.grenade_proximity_warning)
				{
					ImGui::ColorEdit(crypt_str("Granade Warning Color"), &g_cfg.esp.grenade_proximity_warning_progress_color, ALPHA);

					ImGui::Checkbox(crypt_str("Offscreen warning"), &g_cfg.esp.offscreen_proximity);
				}
			}
			widgets::end_child();

			widgets::begin_child("Timer", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Molotov timer"), &g_cfg.esp.molotov_timer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##molotovtimer"), &g_cfg.esp.molotov_timer_color, ALPHA);

				ImGui::Checkbox(crypt_str("Smoke timer"), &g_cfg.esp.smoke_timer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##smoketimer"), &g_cfg.esp.smoke_timer_color, ALPHA);
			}
			widgets::end_child();

			widgets::begin_child("Tracer", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Client bullet impacts"), &g_cfg.esp.client_bullet_impacts);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##clientbulletimpacts"), &g_cfg.esp.client_bullet_impacts_color, ALPHA);

				ImGui::Checkbox(crypt_str("Server bullet impacts"), &g_cfg.esp.server_bullet_impacts);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##serverbulletimpacts"), &g_cfg.esp.server_bullet_impacts_color, ALPHA);

				ImGui::Checkbox(crypt_str("Local bullet tracers"), &g_cfg.esp.bullet_tracer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##bulltracecolor"), &g_cfg.esp.bullet_tracer_color, ALPHA);

				ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &g_cfg.esp.enemy_bullet_tracer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##enemybulltracecolor"), &g_cfg.esp.enemy_bullet_tracer_color, ALPHA);
			}
			widgets::end_child();

			widgets::begin_child("Viewmodel", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox("Viewmodel in scope", &g_cfg.esp.viewmodel_in_scope);

				ImGui::Checkbox(crypt_str("Rare animations"), &g_cfg.skins.rare_animations);
				ImGui::SliderInt(crypt_str("Viewmodel field of view"), &g_cfg.esp.viewmodel_fov, 0, 100);
				ImGui::SliderInt(crypt_str("Viewmodel X"), &g_cfg.esp.viewmodel_x, -50, 50);
				ImGui::SliderInt(crypt_str("Viewmodel Y"), &g_cfg.esp.viewmodel_y, -50, 50);
				ImGui::SliderInt(crypt_str("Viewmodel Z"), &g_cfg.esp.viewmodel_z, -50, 50);
			}
			widgets::end_child();

			widgets::begin_child("Fog", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Fog modulation"), &g_cfg.esp.fog);
				if (g_cfg.esp.fog)
				{
					ImGui::SliderInt(crypt_str("Distance"), &g_cfg.esp.fog_distance, 0, 2500);
					ImGui::SliderInt(crypt_str("Density"), &g_cfg.esp.fog_density, 0, 100);
					ImGui::ColorEdit(crypt_str("Fog Color"), &g_cfg.esp.fog_color, NOALPHA);
				}
			}
			widgets::end_child();

			widgets::begin_child("Thirdperson", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_keybind(crypt_str("Thirdperson"), &g_cfg.misc.thirdperson_toggle, crypt_str("##TPKEY__HOTKEY"));

				if (g_cfg.misc.thirdperson_toggle.key > KEY_NONE && g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
				{
					ImGui::SliderInt(crypt_str("Thirdperson distance"), &g_cfg.misc.thirdperson_distance, 40, 300);
					ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.esp.fov, 0, 100);
				}

				ImGui::Checkbox(crypt_str("Thirdperson when spectating"), &g_cfg.misc.thirdperson_when_spectating);
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::world()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Visuals", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Visuals", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.player.enable);
				ImGui::Checkbox(crypt_str("Rain"), &g_cfg.esp.rain);
				ImGui::Checkbox(crypt_str("Gravity ragdolls"), &g_cfg.misc.ragdolls);
				ImGui::Checkbox(crypt_str("Full bright"), &g_cfg.esp.bright);

				draw_combo(crypt_str("Skybox"), g_cfg.esp.skybox, skybox, ARRAYSIZE(skybox));

				ImGui::ColorEdit(crypt_str("Skybox Color"), &g_cfg.esp.skybox_color, NOALPHA);

				ImGui::Checkbox(crypt_str("Color modulation"), &g_cfg.esp.nightmode);

				if (g_cfg.esp.nightmode)
				{
					ImGui::ColorEdit(crypt_str("World Color"), &g_cfg.esp.world_color, ALPHA);
					ImGui::ColorEdit(crypt_str("Props Color"), &g_cfg.esp.props_color, ALPHA);
				}
			}
			widgets::end_child();

			widgets::begin_child("Trail", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Combo("Trail type", &g_cfg.misc.trail_types, "None\0Particles\0Line\0");

				if (g_cfg.misc.trail_types == 1)
					ImGui::Checkbox(crypt_str("Movement trail"), &g_cfg.misc.movement_trails);

				if (g_cfg.misc.trail_types == 2)
				{
					ImGui::Checkbox(crypt_str("Movement trail"), &g_cfg.misc.movement_trails);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##movement_trail"), &g_cfg.misc.trail_color, ALPHA);

					ImGui::Checkbox(crypt_str("Rainbow trail"), &g_cfg.misc.rainbow_trails);
				}
			}
			widgets::end_child();

			widgets::begin_child("Render", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_multicombo(crypt_str("Removals"), g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);

				if (g_cfg.esp.removals[REMOVALS_ZOOM])
					ImGui::Checkbox(crypt_str("Fix zoom sensivity"), &g_cfg.esp.fix_zoom_sensivity);

				ImGui::Checkbox(crypt_str("Disable panorama blur"), &g_cfg.esp.disablepanoramablur);
				ImGui::Checkbox(crypt_str("Bomb indicator"), &g_cfg.esp.bomb_timer);

				ImGui::Checkbox(crypt_str("Damage marker"), &g_cfg.esp.damage_marker);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##Damagemarkercolor"), &g_cfg.esp.damage_marker_color, ALPHA);

				ImGui::Checkbox(crypt_str("Sniper crosshair "), &g_cfg.esp.sniper_crosshair);
				ImGui::Checkbox(crypt_str("Penetration crosshair"), &g_cfg.esp.penetration_reticle);
			}
			widgets::end_child();

			widgets::begin_child("World", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("World modulation"), &g_cfg.esp.world_modulation);
				if (g_cfg.esp.world_modulation)
				{
					ImGui::SliderFloat(crypt_str("Bloom"), &g_cfg.esp.bloom, 0.0f, 750.0f);
					ImGui::SliderFloat(crypt_str("Exposure"), &g_cfg.esp.exposure, 0.0f, 2000.0f);
					ImGui::SliderFloat(crypt_str("Ambient"), &g_cfg.esp.ambient, 0.0f, 1500.0f);
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("Grenade", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Grenade prediction"), &g_cfg.esp.grenade_prediction);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##grenpredcolor"), &g_cfg.esp.grenade_prediction_color, ALPHA);

				if (g_cfg.esp.grenade_prediction)
				{
					ImGui::Checkbox(crypt_str("On click"), &g_cfg.esp.on_click);
					ImGui::ColorEdit(crypt_str("Tracer Color"), &g_cfg.esp.grenade_prediction_tracer_color, ALPHA);
				}

				ImGui::Checkbox(crypt_str("Grenade proximity warning"), &g_cfg.esp.grenade_proximity_warning);

				if (g_cfg.esp.grenade_proximity_warning)
				{
					ImGui::ColorEdit(crypt_str("Granade Warning Color"), &g_cfg.esp.grenade_proximity_warning_progress_color, ALPHA);

					ImGui::Checkbox(crypt_str("Offscreen warning"), &g_cfg.esp.offscreen_proximity);
				}
			}
			widgets::end_child();

			widgets::begin_child("Timer", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Molotov timer"), &g_cfg.esp.molotov_timer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##molotovtimer"), &g_cfg.esp.molotov_timer_color, ALPHA);

				ImGui::Checkbox(crypt_str("Smoke timer"), &g_cfg.esp.smoke_timer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##smoketimer"), &g_cfg.esp.smoke_timer_color, ALPHA);
			}
			widgets::end_child();

			widgets::begin_child("Tracer", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Client bullet impacts"), &g_cfg.esp.client_bullet_impacts);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##clientbulletimpacts"), &g_cfg.esp.client_bullet_impacts_color, ALPHA);

				ImGui::Checkbox(crypt_str("Server bullet impacts"), &g_cfg.esp.server_bullet_impacts);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##serverbulletimpacts"), &g_cfg.esp.server_bullet_impacts_color, ALPHA);

				ImGui::Checkbox(crypt_str("Local bullet tracers"), &g_cfg.esp.bullet_tracer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##bulltracecolor"), &g_cfg.esp.bullet_tracer_color, ALPHA);

				ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &g_cfg.esp.enemy_bullet_tracer);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##enemybulltracecolor"), &g_cfg.esp.enemy_bullet_tracer_color, ALPHA);
			}
			widgets::end_child();

			widgets::begin_child("Viewmodel", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox("Viewmodel in scope", &g_cfg.esp.viewmodel_in_scope);

				ImGui::Checkbox(crypt_str("Rare animations"), &g_cfg.skins.rare_animations);
				ImGui::SliderInt(crypt_str("Viewmodel field of view"), &g_cfg.esp.viewmodel_fov, 0, 100);
				ImGui::SliderInt(crypt_str("Viewmodel X"), &g_cfg.esp.viewmodel_x, -50, 50);
				ImGui::SliderInt(crypt_str("Viewmodel Y"), &g_cfg.esp.viewmodel_y, -50, 50);
				ImGui::SliderInt(crypt_str("Viewmodel Z"), &g_cfg.esp.viewmodel_z, -50, 50);
			}
			widgets::end_child();

			widgets::begin_child("Fog", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Fog modulation"), &g_cfg.esp.fog);
				if (g_cfg.esp.fog)
				{
					ImGui::SliderInt(crypt_str("Distance"), &g_cfg.esp.fog_distance, 0, 2500);
					ImGui::SliderInt(crypt_str("Density"), &g_cfg.esp.fog_density, 0, 100);
					ImGui::ColorEdit(crypt_str("Fog Color"), &g_cfg.esp.fog_color, NOALPHA);
				}
			}
			widgets::end_child();

			widgets::begin_child("Thirdperson", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				draw_keybind(crypt_str("Thirdperson"), &g_cfg.misc.thirdperson_toggle, crypt_str("##TPKEY__HOTKEY"));

				if (g_cfg.misc.thirdperson_toggle.key > KEY_NONE && g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
				{
					ImGui::SliderInt(crypt_str("Thirdperson distance"), &g_cfg.misc.thirdperson_distance, 40, 300);
					ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.esp.fov, 0, 100);
				}

				ImGui::Checkbox(crypt_str("Thirdperson when spectating"), &g_cfg.misc.thirdperson_when_spectating);
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::misc()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Misc", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Misc", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Keybinds list"), &g_cfg.menu.keybinds);
				ImGui::Checkbox(crypt_str("Spectators list"), &g_cfg.misc.spectators_list);
				ImGui::Checkbox(crypt_str("Clantag spammer"), &g_cfg.misc.clantag_spammer);
				ImGui::Checkbox(crypt_str("Rank Reveal"), &g_cfg.misc.rank_reveal);
				ImGui::Checkbox(crypt_str("Kill effect"), &g_cfg.esp.kill_effect);

				if (g_cfg.esp.kill_effect)
					ImGui::SliderFloat(crypt_str("Kill effect duration"), &g_cfg.esp.kill_effect_duration, 0.0f, 3.0f);

				ImGui::Checkbox(crypt_str("Hitsound"), &g_cfg.esp.hitsound);
				draw_multicombo(crypt_str("Logs"), g_cfg.misc.events_to_log, events, ARRAYSIZE(events), preview);
				draw_multicombo(crypt_str("Logs output"), g_cfg.misc.log_output, events_output, ARRAYSIZE(events_output), preview);
			}

			widgets::end_child();

			widgets::begin_child("Settings", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Developer mode"), &g_cfg.scripts.developer_mode);
				ImGui::Checkbox(crypt_str("Allow HTTP requests"), &g_cfg.scripts.allow_http);
				ImGui::Checkbox(crypt_str("Allow files read or write"), &g_cfg.scripts.allow_file);
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("Help", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Automatic jump"), &g_cfg.misc.bunnyhop);
				draw_combo(crypt_str("Automatic strafes"), g_cfg.misc.airstrafe, strafes, ARRAYSIZE(strafes));
				ImGui::Checkbox(crypt_str("Crouch in air"), &g_cfg.misc.crouch_in_air);
				ImGui::Checkbox(crypt_str("Fast stop"), &g_cfg.misc.fast_stop);

				draw_keybind(crypt_str("Edge jump"), &g_cfg.misc.edge_jump, crypt_str("##EDGEJUMP__HOTKEY"));
			}
			widgets::end_child();

			widgets::begin_child("Buybot", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Enable buybot"), &g_cfg.misc.buybot_enable);
				if (g_cfg.misc.buybot_enable)
				{
					draw_combo(crypt_str("Pistols"), g_cfg.misc.buybot2, secwep, ARRAYSIZE(secwep));
					draw_combo(crypt_str("Snipers"), g_cfg.misc.buybot1, mainwep, ARRAYSIZE(mainwep));
					draw_multicombo(crypt_str("Other"), g_cfg.misc.buybot3, grenades, ARRAYSIZE(grenades), preview);
				}
			}
			widgets::end_child();

			widgets::begin_child("Screen", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				ImGui::Checkbox(crypt_str("Unlock inventory access"), &g_cfg.misc.inventory_access);
				ImGui::Checkbox(crypt_str("Preserve killfeed"), &g_cfg.esp.preserve_killfeed);
				ImGui::SliderFloat(crypt_str("Aspect amount"), &g_cfg.misc.aspect_ratio_amount, 1.0f, 2.0f);
			}

			widgets::end_child();
		}

		ImGui::EndGroup();
		padding(0, 15);
	}

	ImGui::EndChild();
}

void c_menu::skins()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Skins", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		if (Skins == 0)
		{
			static bool drugs = false;
			const char* skins_weapon[36] = {
				crypt_str("knife"),
				crypt_str("gloves"),
				crypt_str("ak47"),
				crypt_str("aug"),
				crypt_str("awp"),
				crypt_str("cz75a"),
				crypt_str("deagle"),
				crypt_str("elite"),
				crypt_str("famas"),
				crypt_str("fiveseven"),
				crypt_str("g3sg1"),
				crypt_str("galilar"),
				crypt_str("glock"),
				crypt_str("m249"),
				crypt_str("m4a1_silencer"),
				crypt_str("m4a1"),
				crypt_str("mac10"),
				crypt_str("mag7"),
				crypt_str("mp5sd"),
				crypt_str("mp7"),
				crypt_str("mp9"),
				crypt_str("negev"),
				crypt_str("nova"),
				crypt_str("hkp2000"),
				crypt_str("p250"),
				crypt_str("p90"),
				crypt_str("bizon"),
				crypt_str("revolver"),
				crypt_str("sawedoff"),
				crypt_str("scar20"),
				crypt_str("ssg08"),
				crypt_str("sg556"),
				crypt_str("tec9"),
				crypt_str("ump45"),
				crypt_str("usp_silencer"),
				crypt_str("xm1014")
			};
			static bool need_update[36];

			if (!all_skins[current_profile] || need_update[current_profile])
			{
				all_skins[current_profile] = get_skin_preview(get_wep(current_profile, (current_profile == 0 || current_profile == 1) ? g_cfg.skins.skinChanger.at(current_profile).definition_override_vector_index : -1, current_profile == 0).c_str(), g_cfg.skins.skinChanger.at(current_profile).skin_name, device);
				need_update[current_profile] = false;
			}

			auto& selected_entry = g_cfg.skins.skinChanger[current_profile];
			selected_entry.itemIdIndex = current_profile;
			static char search_skins[64] = "\0";
			static auto item_index = selected_entry.paint_kit_vector_index;

			ImGui::BeginGroup();
			{
				widgets::begin_child("Main", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
				{
					ImGui::Combo(crypt_str("Weapons"), &current_profile, skins_weapon, ARRAYSIZE(skins_weapon));

					if (current_profile == 0)
					{
						if (ImGui::Combo(crypt_str("Knife"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
							{
								*out_text = game_data::knife_names[idx].name;
						return true;
							}, nullptr, IM_ARRAYSIZE(game_data::knife_names)))
							need_update[current_profile] = true;
					}
					else if (current_profile == 1)
					{
						if (ImGui::Combo(crypt_str("Glove"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
							{
								*out_text = game_data::glove_names[idx].name;
						return true;
							}, nullptr, IM_ARRAYSIZE(game_data::glove_names)))
						{
							item_index = 0;
							need_update[current_profile] = true;
						}
					}
					else
						selected_entry.definition_override_vector_index = 0;

					if (current_profile != 1)
					{
						if (ImGui::InputText(crypt_str("Search"), search_skins, sizeof(search_skins)))
							item_index = -1;
					}

					auto main_kits = current_profile == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits;
					auto display_index = 0;

					SkinChanger::displayKits = main_kits;

					if (current_profile == 1)
					{
						for (auto i = 0; i < main_kits.size(); i++)
						{
							auto main_name = main_kits.at(i).name;

							for (auto i = 0; i < main_name.size(); i++)
								if (iswalpha((main_name.at(i))))
									main_name.at(i) = towlower(main_name.at(i));

							char search_name[64];

							if (!strcmp(game_data::glove_names[selected_entry.definition_override_vector_index].name, crypt_str("Hydra")))
								strcpy_s(search_name, sizeof(search_name), crypt_str("Bloodhound"));
							else
								strcpy_s(search_name, sizeof(search_name), game_data::glove_names[selected_entry.definition_override_vector_index].name);

							for (auto i = 0; i < sizeof(search_name); i++)
								if (iswalpha(search_name[i]))
									search_name[i] = towlower(search_name[i]);

							if (main_name.find(search_name) != std::string::npos)
							{
								SkinChanger::displayKits.at(display_index) = main_kits.at(i);
								display_index++;
							}
						}

						SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
					}
					else
					{
						if (strcmp(search_skins, crypt_str("")))
						{
							for (auto i = 0; i < main_kits.size(); i++)
							{
								auto main_name = main_kits.at(i).name;

								for (auto i = 0; i < main_name.size(); i++)
									if (iswalpha(main_name.at(i)))
										main_name.at(i) = towlower(main_name.at(i));

								char search_name[64];
								strcpy_s(search_name, sizeof(search_name), search_skins);

								for (auto i = 0; i < sizeof(search_name); i++)
									if (iswalpha(search_name[i]))
										search_name[i] = towlower(search_name[i]);

								if (main_name.find(search_name) != std::string::npos)
								{
									SkinChanger::displayKits.at(display_index) = main_kits.at(i);
									display_index++;
								}
							}

							SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
						}
						else
							item_index = selected_entry.paint_kit_vector_index;
					}

					if (!SkinChanger::displayKits.empty())
					{
						if (ImGui::ListBox(crypt_str("##PAINTKITS"), &item_index, [](void* data, int idx, const char** out_text)
							{
								while (SkinChanger::displayKits.at(idx).name.find(crypt_str("С‘")) != std::string::npos)
								SkinChanger::displayKits.at(idx).name.replace(SkinChanger::displayKits.at(idx).name.find(crypt_str("С‘")), 2, crypt_str("Рµ"));

						*out_text = SkinChanger::displayKits.at(idx).name.c_str();
						return true;
							}, nullptr, SkinChanger::displayKits.size(), SkinChanger::displayKits.size() > 9 ? 9 : SkinChanger::displayKits.size()) || !all_skins[current_profile])
						{
							SkinChanger::scheduleHudUpdate();
							need_update[current_profile] = true;

							auto i = 0;

							while (i < main_kits.size())
							{
								if (main_kits.at(i).id == SkinChanger::displayKits.at(item_index).id)
								{
									selected_entry.paint_kit_vector_index = i;
									break;
								}

								i++;
							}

						}
					}
				}
				widgets::end_child();
			}
			ImGui::EndGroup();
			ImGui::SameLine(0, 15);
			ImGui::BeginGroup();
			{
				widgets::begin_child("General", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
				{
					if (ImGui::InputInt(crypt_str("Seed"), &selected_entry.seed, 1, 100))
						SkinChanger::scheduleHudUpdate();

					if (ImGui::InputInt(crypt_str("StatTrak"), &selected_entry.stat_trak, 1, 15))
						SkinChanger::scheduleHudUpdate();
					if (current_profile != 1)
					{
						if (!g_cfg.skins.custom_name_tag[current_profile].empty())
							strcpy_s(selected_entry.custom_name, sizeof(selected_entry.custom_name), g_cfg.skins.custom_name_tag[current_profile].c_str());
						if (ImGui::InputText(crypt_str("Name Tag"), selected_entry.custom_name, sizeof(selected_entry.custom_name)))
						{
							g_cfg.skins.custom_name_tag[current_profile] = selected_entry.custom_name;
							SkinChanger::scheduleHudUpdate();
						}
					}
					if (ImGui::SliderFloat(crypt_str("Wear"), &selected_entry.wear, 0.0f, 1.0f))
						drugs = true;
					else if (drugs)
					{
						SkinChanger::scheduleHudUpdate();
						drugs = false;
					}
					if (ImGui::Combo(crypt_str("Quality"), &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = game_data::quality_names[idx].name;
					return true;
						}, nullptr, IM_ARRAYSIZE(game_data::quality_names)))
						SkinChanger::scheduleHudUpdate();

						ImGui::Checkbox(crypt_str("Force rare animations"), &g_cfg.skins.rare_animations);

						//ImGui::ImageButton(all_skins[current_profile], ImVec2(256, 214));
				}
				widgets::end_child();
				selected_entry.update();
			}
			ImGui::EndGroup();
		}

		if (Skins == 1)
		{
			ImGui::BeginGroup();
			{
				widgets::begin_child("Players", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
				{
					draw_combo(crypt_str("Player models"), g_cfg.player.player_models, player_models, ARRAYSIZE(player_models));
				}
				widgets::end_child();
			}
			ImGui::EndGroup();
		}
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::cfg()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Configs", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Configs", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				cfg_manager->config_files();
				files = cfg_manager->files;

				for (auto& current : files)
					if (current.size() > 2)
						current.erase(current.size() - 4, 4);

				draw_config_button("Add new config", "##Add new config", false, false, false, true);
				for (int i = 0; i < files.size(); i++)
				{
					g_cfg.selected_config = i;
					bool load, save = false;
					draw_config_button(files.at(i).c_str(), files.at(i).c_str(), load, save, i);
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::lua()
{
	ImGui::SetCursorPos(BeginChildPos);
	ImGui::BeginChild("##Lua", BeginChildSize, false, ImGuiWindowFlags_NoBackground);
	{
		ImGui::BeginGroup();
		{
			widgets::begin_child("Lua", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				if (should_update)
				{
					should_update = false;
					scripts = c_lua::get().scripts;

					for (auto& current : scripts)
					{
						if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
							current.erase(current.size() - 5, 5);
						else if (current.size() >= 4)
							current.erase(current.size() - 4, 4);
					}
				}

				ImGui::SetCursorPos(ImVec2(40, 15));
				ImGui::BeginChild("##lua", ImVec2(300, 570), false, ImGuiWindowFlags_NoBackground);
				{
					draw_lua_button("Scripts", "#scripts", false, false, false, true);
					for (int i = 0; i < scripts.size(); i++)
					{
						selected_script = i;
						bool load, save = false;
						draw_lua_button(scripts.at(i).c_str(), scripts.at(i).c_str(), load, save, i);
					}
				}
				ImGui::EndChild();
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		ImGui::SameLine(0, 15);
		ImGui::BeginGroup();
		{
			widgets::begin_child("General", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
			{
				for (auto& current : c_lua::get().scripts)
				{
					auto& items = c_lua::get().items.at(c_lua::get().get_script_id(current));

					for (auto& item : items)
					{
						std::string item_name;

						auto first_point = false;
						auto item_str = false;

						for (auto& c : item.first)
						{
							if (c == '.')
							{
								if (first_point)
								{
									item_str = true;
									continue;
								}
								else
									first_point = true;
							}

							if (item_str)
								item_name.push_back(c);
						}

						switch (item.second.type)
						{
						case NEXT_LINE:
							break;
						case CHECK_BOX:
							ImGui::Checkbox((item_name).c_str(), &item.second.check_box_value);
							break;
						case COMBO_BOX:
							draw_combo((item_name).c_str(), item.second.combo_box_value, [](void* data, int idx, const char** out_text)
								{
									auto labels = (std::vector <std::string>*)data;
							*out_text = labels->at(idx).c_str();
							return true;
								}, &item.second.combo_box_labels, item.second.combo_box_labels.size());
							break;
						case SLIDER_INT:
							ImGui::SliderInt((item_name).c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max);
							break;
						case SLIDER_FLOAT:
							ImGui::SliderFloat((item_name).c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max);
							break;
						case COLOR_PICKER:
							ImGui::ColorEdit((item_name).c_str(), &item.second.color_picker_value, ALPHA, true);
							break;
						}
					}
				}
			}
			widgets::end_child();
		}
		ImGui::EndGroup();
		padding(0, 15);
	}
	ImGui::EndChild();
}

void c_menu::tabs()
{
	ImGui::SetCursorPos(ImVec2(-2, 71));
	ImGui::BeginGroup();
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 10 });
		ImGui::PushFont(icons);               if (ImGui::tab("RageTab", "C", !tab, { -6,-2 })) tab = 0;      ImGui::PopFont();
		ImGui::PushFont(icons);               if (ImGui::tab("LegitTab", "D", tab == 1, { -5,-2 })) tab = 1; ImGui::PopFont();
		ImGui::PushFont(tab_icons);           if (ImGui::tab("AntiaimTab", "C", tab == 2)) tab = 2;          ImGui::PopFont();
		ImGui::PushFont(TacticSans_Icon_Tab); if (ImGui::tab("PlayersTab", "i", tab == 3)) tab = 3;          ImGui::PopFont();
		ImGui::PushFont(tab_icons);           if (ImGui::tab("VisualsTab", "D", tab == 4)) tab = 4;          ImGui::PopFont();
		ImGui::PushFont(icons);               if (ImGui::tab("WorldTab", "B", tab == 5, { -5,-2 })) tab = 5; ImGui::PopFont();
		ImGui::PushFont(icons);               if (ImGui::tab("MiscTab", "F", tab == 6, { -4,-3 })) tab = 6;  ImGui::PopFont();
		ImGui::PushFont(tab_icons);           if (ImGui::tab("SkinsTab", "A", tab == 7)) tab = 7;            ImGui::PopFont();
		ImGui::PushFont(icons);               if (ImGui::tab("CfgTab", "G", tab == 8, { -5,-2 })) tab = 8;   ImGui::PopFont();
		ImGui::PushFont(icons);               if (ImGui::tab("LuaTab", "H", tab == 9, { -4,-3 })) tab = 9;   ImGui::PopFont();
		ImGui::PopStyleVar();
	}
	ImGui::EndGroup();

	switch (tab)
	{
	case 0: rage(); break;
	case 1: legit(); break;
	case 2: anti(); break;
	case 3: players(); break;
	case 4: visuals(); break;
	case 5: world(); break;
	case 6: misc(); break;
	case 7: skins(); break;
	case 8: cfg(); break;
	case 9: lua(); break;
	}
}

void c_menu::draw(bool is_open)
{
	static auto w = 0, h = 0, current_h = 0;
	m_engine()->GetScreenSize(w, current_h);

	if (h != current_h)
	{
		if (h)
			update_scripts = true;

		h = current_h;
		update_dpi = true;
	}

	static float m_alpha = 0.0002f;
	m_alpha = math::clamp(m_alpha + (3.f * ImGui::GetIO().DeltaTime * (is_open ? 1.f : -1.f)), 0.0001f, 1.f);

	if (m_alpha <= 0.0001f)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

	if (!menu_setupped)
		menu_setup(ImGui::GetStyle());

	ImGui::StyleColorsDark();
	ImGui::Begin("menu", 0, ImGuiWindowFlags_NoDecoration);
	{
		ImGui::SetWindowSize({ 765, 550 });

		s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
		p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 61, p.y + 550), ImGui::GetColorU32(ImGuiCol_ChildBg), ImGui::GetStyle().WindowRounding, ImDrawCornerFlags_Left);
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x + 61, p.y), ImVec2(p.x + 62, p.y + 550), ImGui::GetColorU32(ImGuiCol_Border));
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 61), ImVec2(p.x + 62, p.y + 62), ImGui::GetColorU32(ImGuiCol_Border));
		}
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Scheme));
		ImGui::PushFont(icon);
		ImGui::SetCursorPos(ImVec2(9, -6));
		ImGui::Text("A");
		ImGui::PopStyleColor();
		ImGui::PopFont();

		tabs();
	}
	ImGui::End();
}
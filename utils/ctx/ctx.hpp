#pragma once
#include "..\..\menu\ImGui\imgui.h"
#include "..\..\includes.hpp"
#include "..\..\utils\crypt_str\crypt_str.h"

class player_t;
class weapon_t;
class CUserCmd;

struct shot_info
{
	bool should_log = false;

	std::string target_name = crypt_str("None");
	std::string result = crypt_str("None");
	bool spread_another_hitbox = false;
	float simulation_time = 0;

	std::string client_hitbox = crypt_str("None");
	std::string server_hitbox = crypt_str("None");

	int client_damage = 0;
	int server_damage = 0;

	int hitchance = 0;
	int backtrack_ticks = 0;

	Vector aim_point = ZERO;
};

struct aim_shot
{
	bool start = false;
	bool end = false;
	bool impacts = false;
	bool latency = false;
	bool hurt_player = false;

	bool impact_hit_player = false;
	bool occlusion = false;
	bool client_hitbox_int = 0;

	int last_target = -1;
	int side = -1;
	int fire_tick = INT_MAX;
	int event_fire_tick = INT_MAX;

	shot_info shot_info;
};

struct command
{
	bool is_outgoing = false;
	bool is_used = false;
	int previous_command_number = 0;
	int command_number = 0;
};

struct correction_data
{
	int command_number = 0;
	int tickcount = 0;
	int choked_commands = 0;
};


class ctx_t
{
	CUserCmd* m_pcmd = nullptr;
public:

	struct addresses
	{
		uint64_t allow_extrapolation;
		uint64_t smoke_count;
		uint64_t clear_notices;
		uint64_t set_abs_angles;
		uint64_t set_abs_origin;
		uint64_t set_abs_velocity;
		uint64_t lookup_bone;
		uint64_t attachment_helper;
		uint64_t has_c4;
		uint64_t invalidate_bone_cache;
		uint64_t invalidate_physics_recursive;
		uint64_t unknown_function;
		uint64_t physics_run_think;
		uint64_t sequence_activity;
		uint64_t prediction_random_seed;
		uint64_t prediction_player;
		uint64_t post_think_v_physics;
		uint64_t simulate_player_simulated_entities;
		uint64_t rank_reveal;
		uint64_t clip_ray_to_hitbox;
		uint64_t clip_trace_to_players;
		uint64_t write_user_cmd;
		uint64_t clmove_choke_clamp;
		uint64_t set_clantag;
		uint64_t find_hud_element;
		uint64_t game_hud;
		uint64_t is_breakable;
		uint64_t trace_filter_simple;
		uint64_t trace_filter_skip_two_entities;
		uint64_t load_skybox;
		uint64_t line_goes_through_smoke;
		uint64_t update_clientside_animations;
		uint64_t reset_animation_state;
		uint64_t update_animation_state;
		uint64_t create_animation_state;
		uint64_t equip;
		uint64_t enable_bone_cache_invalidation;
		uint64_t get_layer_sequence_cycle_rate;
		uint64_t key_values;
		uint64_t load_from_buffer;
		uint64_t last_command;
		uint64_t is_self_animating;
		uint64_t get_shotgun_spread;
	} addresses;

	struct Convars
	{
		ConVar* developer;

		ConVar* sensitivity;

		ConVar* m_pitch;
		ConVar* m_yaw;

		ConVar* r_drawmodelstatsoverlay;
		ConVar* r_3dsky;
		ConVar* r_drawspecificstaticprop;
		ConVar* r_shadows;
		ConVar* r_modelAmbientMin;
		ConVar* r_jiggle_bones;

		ConVar* con_filter_enable;
		ConVar* con_filter_text;

		ConVar* ff_damage_reduction_bullets;
		ConVar* ff_damage_bullet_penetration;

		ConVar* mat_fullbright;

		ConVar* viewmodel_fov;
		ConVar* viewmodel_offset_x;
		ConVar* viewmodel_offset_y;
		ConVar* viewmodel_offset_z;

		ConVar* fog_override;
		ConVar* fog_start;
		ConVar* fog_end;
		ConVar* fog_maxdensity;
		ConVar* fog_color;

		ConVar* mp_c4timer;
		ConVar* mp_damage_scale_ct_head;
		ConVar* mp_damage_scale_ct_body;
		ConVar* mp_damage_scale_t_head;
		ConVar* mp_damage_scale_t_body;

		ConVar* weapon_recoil_scale;
		ConVar* weapon_accuracy_nospread;
		ConVar* weapon_debug_spread_show;
		ConVar* weapon_molotov_maxdetonateslope;
		ConVar* weapon_accuracy_shotgun_spread_patterns;

		ConVar* molotov_throw_detonate_time;

		ConVar* cl_forwardspeed;
		ConVar* cl_sidespeed;
		ConVar* cl_upspeed;
		ConVar* cl_updaterate;
		ConVar* cl_interp_ratio;
		ConVar* cl_interp;
		ConVar* cl_mouseenable;

		ConVar* cl_csm_static_prop_shadows;
		ConVar* cl_csm_shadows;
		ConVar* cl_csm_world_shadows;
		ConVar* cl_foot_contact_shadows;
		ConVar* cl_csm_viewmodel_shadows;
		ConVar* cl_csm_rope_shadows;
		ConVar* cl_csm_sprite_shadows;

		ConVar* sv_cheats;
		ConVar* sv_minupdaterate;
		ConVar* sv_maxupdaterate;
		ConVar* sv_client_min_interp_ratio;
		ConVar* sv_client_max_interp_ratio;
		ConVar* sv_skyname;
		ConVar* sv_maxunlag;
		ConVar* sv_gravity;
		ConVar* sv_jump_impulse;
		ConVar* sv_clip_penetration_traces_to_players;

	} convars;

	struct gui_helpers
	{
		bool open_pop;
		float pop_anim;
	}gui;



	struct Globals
	{
		struct Animations
		{
			bool m_update_bones = false;
			bool m_update_animations = false;

			struct Local
			{
				bool m_real_matrix_ret = false;

				matrix3x4_t m_fake_matrix[MAXSTUDIOBONES];
				matrix3x4_t m_real_matrix[MAXSTUDIOBONES];
			} local;

		} animations;

		bool loaded_script = false;
		bool focused_on_input = false;
		bool double_tap_fire = false;
		bool double_tap_aim = false;
		bool double_tap_aim_check = false;
		bool fired_shot = false;
		bool force_send_packet = false;
		bool exploits = false;
		bool scoped = false;
		bool autowalling = false;
		bool setuping_bones = false;
		bool updating_animation = false;
		bool aimbot_working = false;
		bool revolver_working = false;
		bool slowwalking = false;
		bool change_materials = false;
		bool drawing_ragdoll = false;
		bool in_thirdperson = true;
		bool fakeducking = false;
		bool should_choke_packet = false;
		bool should_send_packet = false;
		bool bomb_timer_enable = false;
		bool backup_model = false;
		bool reset_net_channel = false;
		bool in_createmove = false;
		bool should_remove_smoke = false;
		bool should_update_beam_index = false;
		bool should_clear_death_notices = false;
		bool should_update_playerresource = false;
		bool should_update_gamerules = false;
		bool should_check_skybox = false;
		bool should_update_radar = false;
		bool updating_skins = false;
		bool should_update_weather = false;
		bool m_bIsLocalPeek;
		bool m_bInDiffensiveDt;
		Vector fixed_viewangles = ZERO;
		bool override_velmod = false;
		float weap_inaccuracy = 0.0f;

		struct new_dt_t
		{
			int next_tickbase_shift = 0;
			int tickbase_shift = 0;
			int charge_ticks = 0;
			bool recharging = false;
			bool shifting = false;
		}new_dt;

		int shift_time = 0;
		int framerate = 0;
		int ping = 0;
		int ticks_allowed = 0;
		int ticks_choke = 0;
		int next_tickbase_shift = 0;
		int tickbase_shift = 0;
		int fixed_tickbase = 0;
		int backup_tickbase = 0;
		int current_weapon = 0;
		int last_aimbot_shot = 0;
		int bomb_carrier = 0;
		int kills = 0;
		int should_buy = 0;
		int fired_shots[65];
		int missed_shots[65];

		float next_lby_update = 0.0f;
		float last_lby_move = 0.0f;
		float inaccuracy = 0.0f;
		float spread = 0.0f;
		float last_velocity_modifier = 0.0f;
		float original_forwardmove = 0.0f;
		float original_sidemove = 0.0f;
		int m_ragebot_shot_nr = 0;
		//dt

		int out_sequence_nr = 0;

		int current_tickcount = 0;
		int tickrate = 0;
		int tochargeamount = 0;

		bool should_recharge = false;

		int shift_ticks = 0;

		bool shot_command;

		int last_velocity_modifier_tick = 0;

		int tocharge = 0;

		bool shifting = false;

		int teleport_amount = 0;

		bool trigger_teleport = false;

		std::string time = crypt_str("unknown");

		struct
		{
			bool m_bIsPeeking = false;
			bool m_bIsPrevPeek = false;
			bool started_peeking_Defensive;
		} m_Peek;

		Vector original;

		Vector eye_pos = ZERO;
		Vector start_position = ZERO;
		Vector dormant_origin[65];

		matrix3x4_t prediction_matrix[MAXSTUDIOBONES];
		matrix3x4_t fake_matrix[MAXSTUDIOBONES];

		IClientNetworkable* m_networkable = nullptr;
		weapon_t* weapon = nullptr;
		std::vector <int> choked_number;
		std::deque <command> commands;
		std::deque <correction_data> data;
		std::vector <std::string> events;

		ImVec4 menu_color;
	} globals;

	std::vector <std::string> signatures;
	std::vector <int> indexes;

#if RELEASE
	std::string username;
#else
	std::string username = crypt_str("Developer");
#endif

	std::string last_font_name;

	std::vector <aim_shot> shots;

	bool available();

	bool send_packet = false;

	void set_command(CUserCmd* cmd)
	{
		m_pcmd = cmd;
	}

	player_t* local(player_t* e = nullptr, bool initialization = false);
	CUserCmd* get_command();
};

extern ctx_t g_ctx;
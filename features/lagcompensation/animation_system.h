#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

enum
{
	MAIN,
	NONE,
	FIRST,
	SECOND,
	LOW_FIRST,
	LOW_SECOND,
	LOW_FIRST1,
	LOW_SECOND1
};

enum resolve_type
{
	RESOLVE_STAND,
	RESOLVE_MOVE,
	RESOLVE_WALK,
	RESOLVE_LBY,
	RESOLVE_AIR,

};

struct matrixes
{
	matrix3x4_t main[MAXSTUDIOBONES];
	matrix3x4_t zero[MAXSTUDIOBONES];
	matrix3x4_t first[MAXSTUDIOBONES];
	matrix3x4_t second[MAXSTUDIOBONES];
};

class adjust_data;

enum resolver_side
{
	RESOLVER_ORIGINAL,
	RESOLVER_ZERO,
	RESOLVER_FIRST,
	RESOLVER_SECOND,
	RESOLVER_LOW_FIRST,
	RESOLVER_LOW_SECOND,
};

enum modes
{
	AIR,
	SLOW_WALKING,
	MOVING,
	STANDING,
	FREESTANDING,
	NO_MODE
};

enum get_side_move
{
	LEFT,
	RIGHT,
	NO_SIDE
};

class resolver
{
	player_t* player = nullptr;
	adjust_data* player_record = nullptr;
	adjust_data* previous_record = nullptr;

	bool side = false;
	bool fake = false;

	float lock_side = 0.0f;
	float original_goal_feet_yaw = 0.0f;
	float original_pitch = 0.0f;

public:
	AnimationLayer resolver_layers[4][13];
	void initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch);
	resolver_side FreeStand(player_t* e);
	bool IsFakewalking(player_t* entity);
	void reset();
	bool Saw(player_t* entity);
	resolver_side TraceSide(player_t* entity);
	void NoSpreadResolver();
	void resolve_yaw();
	float resolve_pitch();
	bool bruteforced_sides[8];
	resolver_side last_side = RESOLVER_ORIGINAL;
};

enum resolver_type
{
	ORIGINAL,
	BRUTEFORCE,
	LBY,
	LAYERS,
	TRACE,
	DIRECTIONAL,
	ANIM_s,
	ANIM_m,
	ANIM_l,
	LOCKED_SIDE,
	HISTORY_SIDE,
	SHOT
};

enum resolver_history
{
	HISTORY_UNKNOWN = -1,
	HISTORY_ORIGINAL,
	HISTORY_ZERO,
	HISTORY_DEFAULT,
	HISTORY_LOW
};

class adjust_data //-V730
{
public:
	player_t* player;
	int i;
	AnimationLayer right_layers[13];
	AnimationLayer left_layers[13];
	AnimationLayer center_layers[13];
	AnimationLayer layers[13];
	matrixes matrixes_data;
	float exit_angle = 0;
	resolver_type type;
	resolver_side side;
	modes curMode;
	get_side_move curSide;


	bool invalid;
	bool immune;
	bool dormant;
	bool bot;
	bool shot;

	int flags;
	int bone_count;

	float simulation_time;
	float duck_amount;
	float lby;
	int m_ChokedTicks;

	Vector angles;
	Vector abs_angles;
	Vector velocity;
	Vector origin;
	Vector mins;
	Vector maxs;

	adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		player = nullptr;
		i = -1;

		type = ORIGINAL;
		side = RESOLVER_ORIGINAL;

		invalid = false;
		immune = false;
		dormant = false;
		bot = false;
		shot = false;

		flags = 0;
		bone_count = 0;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		lby = 0.0f;

		angles.Zero();
		abs_angles.Zero();
		velocity.Zero();
		origin.Zero();
		mins.Zero();
		maxs.Zero();
	}

	adjust_data(player_t* e, bool store = true)
	{
		type = ORIGINAL;
		side = RESOLVER_ORIGINAL;

		invalid = false;
		store_data(e, store);
	}

	void store_data(player_t* e, bool store = true)
	{
		if (!e->is_alive())
			return;

		player = e;
		i = player->EntIndex();

		if (store)
		{
			memcpy(layers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
			memcpy(matrixes_data.main, player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
		}

		immune = player->m_bGunGameImmunity() || player->m_fFlags() & FL_FROZEN;
		dormant = player->IsDormant();

		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		bot = player_info.fakeplayer;
		shot = player->m_hActiveWeapon() && (player->m_hActiveWeapon()->m_fLastShotTime() == player->m_flSimulationTime());

		flags = player->m_fFlags();
		bone_count = player->m_CachedBoneData().Count();

		simulation_time = player->m_flSimulationTime();
		duck_amount = player->m_flDuckAmount();
		lby = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->m_vecOrigin();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	void adjust_player()
	{
		if (!valid(false))
			return;

		memcpy(player->get_animlayers(), layers, player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(player->m_CachedBoneData().Base(), matrixes_data.main, player->m_CachedBoneData().Count() * sizeof(matrix3x4_t)); //-V807

		player->m_fFlags() = flags;
		player->m_CachedBoneData().m_Size = bone_count;

		player->m_flSimulationTime() = simulation_time;
		player->m_flDuckAmount() = duck_amount;
		player->m_flLowerBodyYawTarget() = lby;

		player->m_angEyeAngles() = angles;
		player->set_abs_angles(abs_angles);
		player->m_vecVelocity() = velocity;
		player->m_vecOrigin() = origin;
		player->set_abs_origin(origin);
		player->GetCollideable()->OBBMins() = mins;
		player->GetCollideable()->OBBMaxs() = maxs;
	}

	bool valid(bool extra_checks = true);
};

class optimized_adjust_data
{
public:
	int i;
	player_t* player;

	float simulation_time;
	float duck_amount;
	float speed;

	Vector angles;
	Vector origin;

	bool shot;

	optimized_adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		i = 0;
		player = nullptr;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		shot = false;
		angles.Zero();
		origin.Zero();
	}
};

extern std::deque <adjust_data> player_records[65];
struct player_settings
{
	__int64 id;
	resolver_history res_type;
	bool faking;
	int neg;
	int pos;

	player_settings(__int64 id, resolver_history res_type, bool faking, int left, int right) noexcept : id(id), res_type(res_type), faking(faking), neg(neg), pos(pos) { }
};
class lagcompensation : public singleton <lagcompensation>
{
public:
	std::vector<player_settings> player_sets;

	void fsn(ClientFrameStage_t stage);
	void update_animation(player_t* m_pPlayer);
	bool valid(int i, player_t* e);
	//void pandora_extrapolation(player_t* player, Vector& origin, Vector& velocity, int& flags, bool on_ground);
	void update_player_animations(player_t* e);
	//void PVSFix(ClientFrameStage_t stage);

	resolver player_resolver[65];

	//BoneArray* m_Matrix;
	//BoneArray* m_res;
	//bool has_prev_record;

	bool is_dormant[65];
	float previous_goal_feet_yaw[65];
	bool flags;
};
#include "local_animations.h"
#include "..\misc\misc.h"
#include "..\exploit\exploit.h"

void local_animations::store_cm_animation_data(CUserCmd* cmd)
{
	local_data.cm_stored_viewangles = cmd->m_viewangles;
	local_data.cm_stored_commandnumber = cmd->m_command_number;
}

void local_animations::update_fake_animations(CUserCmd* cmd, bool* send_packet)
{
	// do these yourself I can't be bothered.
}
inline float Approach(float target, float value, float speed) ///ÛÛÛÛÛÛÛÛ ÌÀÒÈÌÀÒÈÊÀ Â ÀÍÈÌÔÛÊÑÈ 
{
	float delta = target - value;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}
void local_animations::update_local_animations(CUserCmd* cmd, bool* send_packet)
{
	static bool lock_viewangles = false;
	static Vector target_angle = local_data.cm_stored_viewangles;
	static Vector non_shot_target_angle = local_data.cm_stored_viewangles;

	g_ctx.local()->get_animlayers()[3].m_flWeight = 0.0f;
	g_ctx.local()->get_animlayers()[3].m_flCycle = 0.0f;
	g_ctx.local()->get_animlayers()[12].m_flCycle = 0.0f;

	if (g_ctx.globals.m_ragebot_shot_nr == local_data.cm_stored_commandnumber)
	{
		lock_viewangles = true;
		target_angle = local_data.cm_stored_viewangles;
	}

	if (!lock_viewangles)
		target_angle = local_data.cm_stored_viewangles;

	auto animstate = g_ctx.local()->get_animation_state();

	if (!animstate || !g_ctx.local()->is_alive() || g_ctx.local()->IsDormant())
	{
		m_real_spawntime = 0;
		return;
	}

	g_ctx.local()->m_flThirdpersonRecoil() = (g_ctx.local()->m_aimPunchAngle() * m_cvar()->FindVar(crypt_str("weapon_recoil_scale"))->GetFloat()).x;

	AnimationLayer old_anim_layers[13];
	std::memcpy(old_anim_layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

	if (g_ctx.local()->m_flSpawnTime() != m_real_spawntime || animstate->m_pBasePlayer != g_ctx.local())
	{
		g_ctx.globals.updating_animation = true;
		g_ctx.local()->update_clientside_animation();
		g_ctx.globals.updating_animation = false;

		std::memcpy(g_ctx.local()->get_animlayers(), old_anim_layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
		m_real_spawntime = g_ctx.local()->m_flSpawnTime();
	}

	const auto old_render_angles = g_ctx.local()->get_render_angles();
	const auto old_pose_params = g_ctx.local()->m_flPoseParameter();

	animstate->m_flLastUpdateIncrement = fmaxf(m_globals()->m_curtime - animstate->m_flLastUpdateTime, 0.f);

	g_ctx.local()->get_render_angles() = ((exploit::get().hide_shots_key) && !g_ctx.globals.fakeducking && lock_viewangles) ? non_shot_target_angle : target_angle;

	if (animstate->m_pWeapon != animstate->m_pWeaponLast)
	{
		for (int i = 0; i < 13; i++)
		{
			AnimationLayer pLayer = g_ctx.local()->get_animlayers()[i];

			pLayer.m_pStudioHdr = NULL;
			pLayer.m_nDispatchSequence = -1;
			pLayer.m_nDispatchSequence_2 = -1;
		}
	}

	float flNewDuckAmount;
	if (animstate)
	{
		flNewDuckAmount = math::clamp(g_ctx.local()->m_flDuckAmount() + animstate->m_flDuckAdditional, 0.0f, 1.0f);
		flNewDuckAmount = Approach(flNewDuckAmount, animstate->m_flAnimDuckAmount, animstate->m_flLastUpdateIncrement * 6.0f);
		flNewDuckAmount = math::clamp(flNewDuckAmount, 0.0f, 1.0f);
	}
	animstate->m_flAnimDuckAmount = flNewDuckAmount;

	g_ctx.local()->m_nSequence() = 0;
	g_ctx.local()->m_flPlaybackRate() = 0.0f;
	g_ctx.local()->m_flCycle() = 0.0f;

	if (g_ctx.local()->m_flCycle() != 0.0f)
	{
		g_ctx.local()->m_flCycle() = 0.0f;
		g_ctx.local()->invalidate_physics_recursive(0x8);
	}

	animstate->m_flMoveWeight = 0;

	g_ctx.globals.updating_animation = true;
	g_ctx.local()->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	g_ctx.local()->force_bone_rebuild();
	g_ctx.local()->setup_local_bones(nullptr, BONE_USED_BY_ANYTHING);


	g_ctx.local()->get_animation_state()->m_flStrafeChangeCycle = old_anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flCycle;
	g_ctx.local()->get_animation_state()->m_flStrafeChangeWeight = old_anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight;
	g_ctx.local()->get_animation_state()->m_nStrafeSequence = old_anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_nSequence;
	g_ctx.local()->get_animation_state()->m_flPrimaryCycle = old_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flCycle;
	g_ctx.local()->get_animation_state()->m_flMoveWeight = old_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight;
	g_ctx.local()->get_animation_state()->m_flAccelerationWeight = old_anim_layers[ANIMATION_LAYER_LEAN].m_flWeight;


	for (int i = 0; i < 13; ++i)
	{
		AnimationLayer layer = g_ctx.local()->get_animlayers()[i];
		if (!layer.m_nSequence && layer.m_pOwner && layer.m_flWeight != 0.0f)
		{
			((player_t*)layer.m_pOwner)->invalidate_physics_recursive(BOUNDS_CHANGED);
			layer.m_flWeight = 0.0f;
		}
	}

	if (*send_packet == false)
	{
		std::memcpy(g_ctx.local()->get_animlayers(), old_anim_layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
		return;
	}

	if (!lock_viewangles)
		non_shot_target_angle = local_data.cm_stored_viewangles;

	memcpy(m_real_poses, g_ctx.local()->m_flPoseParameter().data(), 24 * sizeof(float));
	m_real_angles = g_ctx.local()->get_animation_state()->m_flGoalFeetYaw;
	memcpy(m_real_layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

	g_ctx.local()->m_flPoseParameter() = old_pose_params;
	memcpy(g_ctx.local()->get_animlayers(), old_anim_layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	g_ctx.local()->get_render_angles() = old_render_angles;

	lock_viewangles = false;
}

void local_animations::build_local_bones(player_t* local)
{
	float pose_parameter[24];
	std::memcpy(pose_parameter, g_ctx.local()->m_flPoseParameter().data(), 24 * sizeof(float));

	local->force_bone_rebuild();

	std::memcpy(g_ctx.local()->m_flPoseParameter().data(), m_real_poses, 24 * sizeof(float));
	local->set_abs_angles(Vector(0, m_real_angles, 0));
	std::memcpy(g_ctx.local()->get_animlayers(), m_real_layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

	local->setup_local_bones(nullptr, BONE_USED_BY_ANYTHING);//local->setup_local_bones(BONE_USED_BY_HITBOX);
	std::memcpy(g_ctx.local()->m_flPoseParameter().data(), pose_parameter, 24 * sizeof(float));
}
/*#include "local_animations.h"

void local_animations::run(ClientFrameStage_t stage)
{
	if (!fakelag::get().condition && key_binds::get().get_key_bind_state(20))
	{
		if (stage == FRAME_NET_UPDATE_END)
		{
			fake_server_update = false;

			if (g_ctx.local()->m_flSimulationTime() != fake_simulation_time) //-V550 //-V807
			{
				fake_server_update = true;
				fake_simulation_time = g_ctx.local()->m_flSimulationTime();
			}

			g_ctx.local()->get_animlayers()[3].m_flWeight = 0.0f;
			g_ctx.local()->get_animlayers()[3].m_flCycle = 0.0f;
			g_ctx.local()->get_animlayers()[12].m_flWeight = 0.0f;

			update_fake_animations();
		}
		else if (stage == FRAME_RENDER_START)
		{
			auto animstate = g_ctx.local()->get_animation_state(); //-V807

			if (!animstate)
				return;

			real_server_update = false;

			if (g_ctx.local()->m_flSimulationTime() != real_simulation_time) //-V550
			{
				real_server_update = true;
				real_simulation_time = g_ctx.local()->m_flSimulationTime();
			}

			g_ctx.local()->get_animlayers()[3].m_flWeight = 0.0f;
			g_ctx.local()->get_animlayers()[3].m_flCycle = 0.0f;
			g_ctx.local()->get_animlayers()[12].m_flWeight = 0.0f;

			update_local_animations(animstate);
		}
	}
	else if (stage == FRAME_RENDER_START)
	{
		auto animstate = g_ctx.local()->get_animation_state(); //-V807

		if (!animstate)
			return;

		real_server_update = false;
		fake_server_update = false;

		if (g_ctx.local()->m_flSimulationTime() != real_simulation_time || g_ctx.local()->m_flSimulationTime() != fake_simulation_time) //-V550
		{
			real_server_update = fake_server_update = true;
			real_simulation_time = fake_simulation_time = g_ctx.local()->m_flSimulationTime();
		}

		g_ctx.local()->get_animlayers()[3].m_flWeight = 0.0f;
		g_ctx.local()->get_animlayers()[3].m_flCycle = 0.0f;
		g_ctx.local()->get_animlayers()[12].m_flWeight = 0.0f;

		update_fake_animations();
		update_local_animations(animstate);
	}
}

void local_animations::update_prediction_animations()
{
	auto alloc = !local_data.prediction_animstate;
	auto change = !alloc && handle != &g_ctx.local()->GetRefEHandle(); //-V807
	auto reset = !alloc && !change && g_ctx.local()->m_flSpawnTime() != spawntime; //-V550

	if (change)
		m_memalloc()->Free(local_data.prediction_animstate);

	if (reset)
	{
		util::reset_state(local_data.prediction_animstate);
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (alloc || change)
	{
		local_data.prediction_animstate = (c_baseplayeranimationstate*)m_memalloc()->Alloc(sizeof(c_baseplayeranimationstate));

		if (local_data.prediction_animstate)
			util::create_state(local_data.prediction_animstate, g_ctx.local());

		handle = (CBaseHandle*)&g_ctx.local()->GetRefEHandle();
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (!alloc && !change && !reset)
	{
		float pose_parameter[24]; //-V688
		memcpy(pose_parameter, &g_ctx.local()->m_flPoseParameter(), 24 * sizeof(float));

		AnimationLayer layers[15]; //-V688
		memcpy(layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

		local_data.prediction_animstate->m_pBasePlayer = g_ctx.local();
		util::update_state(local_data.prediction_animstate, ZERO);

		g_ctx.local()->setup_bones_fixed(g_ctx.globals.prediction_matrix, BONE_USED_BY_HITBOX);

		memcpy(&g_ctx.local()->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
		memcpy(g_ctx.local()->get_animlayers(), layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	}
}

void local_animations::update_fake_animations()
{
	auto alloc = !local_data.animstate;
	auto change = !alloc && handle != &g_ctx.local()->GetRefEHandle(); //-V807
	auto reset = !alloc && !change && g_ctx.local()->m_flSpawnTime() != spawntime; //-V550

	if (change)
		m_memalloc()->Free(local_data.animstate);

	if (reset)
	{
		util::reset_state(local_data.animstate);
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (alloc || change)
	{
		local_data.animstate = (c_baseplayeranimationstate*)m_memalloc()->Alloc(sizeof(c_baseplayeranimationstate));

		if (local_data.animstate)
			util::create_state(local_data.animstate, g_ctx.local());

		handle = (CBaseHandle*)&g_ctx.local()->GetRefEHandle();
		spawntime = g_ctx.local()->m_flSpawnTime();
	}

	if (!alloc && !change && !reset && fake_server_update)
	{
		float pose_parameter[24]; //-V688
		memcpy(pose_parameter, &g_ctx.local()->m_flPoseParameter(), 24 * sizeof(float));

		AnimationLayer layers[15]; //-V688
		memcpy(layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));

		auto backup_frametime = m_globals()->m_frametime; //-V807
		auto backup_curtime = m_globals()->m_curtime;

		m_globals()->m_frametime = m_globals()->m_intervalpertick;
		m_globals()->m_curtime = g_ctx.local()->m_flSimulationTime();

		local_data.animstate->m_pBasePlayer = g_ctx.local();
		util::update_state(local_data.animstate, local_animations::get().local_data.fake_angles);

		local_data.animstate->m_bLanding = false;
		local_data.animstate->m_bLandedOnGroundThisFrame = 0.0f;
		local_data.animstate->m_flLeftGroundHeight = 1.0f;

		g_ctx.local()->setup_bones_fixed(g_ctx.globals.fake_matrix, BONE_USED_BY_ANYTHING);
		local_data.visualize_lag = g_cfg.player.visualize_lag;

		if (!local_data.visualize_lag)
		{
			for (auto& i : g_ctx.globals.fake_matrix)
			{
				i[0][3] -= g_ctx.local()->GetRenderOrigin().x; //-V807
				i[1][3] -= g_ctx.local()->GetRenderOrigin().y;
				i[2][3] -= g_ctx.local()->GetRenderOrigin().z;
			}
		}

		m_globals()->m_frametime = backup_frametime;
		m_globals()->m_curtime = backup_curtime;

		memcpy(&g_ctx.local()->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
		memcpy(g_ctx.local()->get_animlayers(), layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	}
}

void local_animations::update_local_animations(c_baseplayeranimationstate* animstate)
{
	if (tickcount != m_globals()->m_tickcount) //-V550
	{
		tickcount = m_globals()->m_tickcount;
		memcpy(layers, g_ctx.local()->get_animlayers(), g_ctx.local()->animlayer_count() * sizeof(AnimationLayer)); //-V807

		if (local_data.animstate)
			animstate->m_flAnimDuckAmount = local_data.animstate->m_flAnimDuckAmount;

		animstate->m_iLastClientSideAnimationUpdateFramecount = 0;
		util::update_state(animstate, local_animations::get().local_data.fake_angles);

		if (real_server_update)
		{
			abs_angles = animstate->m_flGoalFeetYaw;
			memcpy(pose_parameter, &g_ctx.local()->m_flPoseParameter(), 24 * sizeof(float));
		}
	}
	else
		animstate->m_iLastClientSideAnimationUpdateFramecount = m_globals()->m_framecount;

	animstate->m_flGoalFeetYaw = antiaim::get().condition(g_ctx.get_command()) ? abs_angles : local_animations::get().local_data.real_angles.y;
	g_ctx.local()->set_abs_angles(Vector(0.0f, abs_angles, 0.0f));

	memcpy(g_ctx.local()->get_animlayers(), layers, g_ctx.local()->animlayer_count() * sizeof(AnimationLayer));
	memcpy(&g_ctx.local()->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
}*/
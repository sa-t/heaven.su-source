#include "..\hooks.hpp"
#include "..\..\features\ragebot\antiaim.h"
#include "..\..\features\visuals\other_esp.h"
#include "..\..\features\misc\fakelag.h"
#include "..\..\features\misc\prediction_system.h"
#include "..\..\features\ragebot\aim.h"
#include "..\..\features\legitbot\legitbot.h"
#include "..\..\features\misc\bunnyhop.h"
#include "..\..\features\misc\airstrafe.h"
#include "..\..\features\misc\spammers.h"
#include "..\..\features\fakewalk\slowwalk.h"
#include "..\..\features\misc\misc.h"
#include "..\..\features\misc\logs.h"
#include "..\..\features\visuals\GrenadePrediction.h"
#include "..\..\features\ragebot\knifebot.h"
#include "..\..\features\ragebot\zeusbot.h"
#include "..\..\features\lagcompensation\local_animations.h"
#include "..\..\features\lagcompensation\animation_system.h"
#include "../../features/exploit/exploit.h"

using CreateMove_t = void(__stdcall*)(int, float, bool);

void __stdcall hooks::hooked_createmove(int sequence_number, float input_sample_frametime, bool active, bool& send_packet)
{
	static auto original_fn = client_hook->get_func_address <CreateMove_t>(22);
	original_fn(sequence_number, input_sample_frametime, active);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	auto m_pcmd = m_input()->GetUserCmd(sequence_number);
	auto verified = m_input()->GetVerifiedUserCmd(sequence_number);

	g_ctx.globals.in_createmove = false;
	Vector wish_yaw = m_pcmd->m_viewangles;

	if (!m_pcmd)
		return;

	if (!m_pcmd->m_command_number)
		return;

	if (original_fn)
	{
		m_prediction()->SetLocalViewAngles(m_pcmd->m_viewangles);
		m_engine()->SetViewAngles(m_pcmd->m_viewangles);
	}

	if (!g_ctx.available())
		return;

	misc::get().rank_reveal();
	spammers::get().clan_tag();

	if (!g_ctx.local()->is_alive())
		return;

	uintptr_t* frame_ptr;
	__asm mov frame_ptr, ebp;

	g_ctx.globals.weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!g_ctx.globals.weapon)
		return;

	g_ctx.globals.in_createmove = true;
	g_ctx.set_command(m_pcmd);

	if (menu_open && g_ctx.globals.focused_on_input)
	{
		m_pcmd->m_buttons = 0;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
	}


	if (g_ctx.globals.should_recharge)
	{
		m_pcmd->m_tickcount = INT_MAX;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;

		verified->m_cmd = *m_pcmd;
		verified->m_crc = m_pcmd->GetChecksum();
		return;
	}

	if (g_ctx.globals.ticks_allowed < 16 && (exploit::get().double_tap_enabled && exploit::get().double_tap_key || exploit::get().hide_shots_enabled && exploit::get().hide_shots_key))
		g_ctx.globals.should_recharge = true;

	g_ctx.globals.backup_tickbase = g_ctx.local()->m_nTickBase();

	if (g_ctx.globals.next_tickbase_shift)
		g_ctx.globals.fixed_tickbase = g_ctx.local()->m_nTickBase() - g_ctx.globals.next_tickbase_shift;
	else
		g_ctx.globals.fixed_tickbase = g_ctx.globals.backup_tickbase;

	const auto bk = g_ctx.local()->m_flThirdpersonRecoil();

	const auto movestate = g_ctx.local()->m_iMoveState();
	const auto iswalking = g_ctx.local()->m_bIsWalking();

	g_ctx.local()->m_iMoveState() = 0;
	g_ctx.local()->m_bIsWalking() = false;

	auto m_forward = m_pcmd->m_buttons & IN_FORWARD;
	auto m_back = m_pcmd->m_buttons & IN_BACK;
	auto m_right = m_pcmd->m_buttons & IN_MOVERIGHT;
	auto m_left = m_pcmd->m_buttons & IN_MOVELEFT;
	auto m_walking = m_pcmd->m_buttons & IN_SPEED;

	bool m_walk_state = m_walking ? true : false;

	if (m_pcmd->m_buttons & IN_DUCK || g_ctx.local()->m_bDucking() || g_ctx.local()->m_fFlags() & FL_DUCKING)
		m_walk_state = false;
	else if (m_walking)
	{
		float m_max_speed = g_ctx.local()->GetMaxPlayerSpeed() * 0.52f;

		if (m_max_speed + 25.f > g_ctx.local()->m_vecVelocity().Length())
			g_ctx.local()->m_bIsWalking() = true;
	}

	auto move_buttons_pressed = m_pcmd->m_buttons & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT | IN_RUN);

	bool holding_forward_and_back;
	bool holding_right_and_left;

	if (!m_forward)
		holding_forward_and_back = false;
	else
		holding_forward_and_back = m_back;

	if (!m_right)
		holding_right_and_left = false;
	else
		holding_right_and_left = m_left;

	if (move_buttons_pressed)
	{
		if (holding_forward_and_back)
		{
			if (holding_right_and_left)
				g_ctx.local()->m_iMoveState() = 0;
			else if (m_right || m_left)
				g_ctx.local()->m_iMoveState() = 2;
			else
				g_ctx.local()->m_iMoveState() = 0;
		}
		else
		{
			if (holding_forward_and_back)
				g_ctx.local()->m_iMoveState() = 0;
			else if (m_back || m_forward)
				g_ctx.local()->m_iMoveState() = 2;
			else
				g_ctx.local()->m_iMoveState() = 0;
		}
	}

	if (g_ctx.local()->m_iMoveState() == 2 && m_walk_state)
		g_ctx.local()->m_iMoveState() = 1;

	local_animations::get().update_local_animations(m_pcmd, &g_ctx.send_packet);

	g_ctx.local()->m_iMoveState() = movestate;
	g_ctx.local()->m_bIsWalking() = iswalking;
	g_ctx.local()->m_flThirdpersonRecoil() = bk;

	if (menu_open)
	{
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;
	}

	if (m_pcmd->m_buttons & IN_ATTACK2 && g_cfg.ragebot.enable && g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		m_pcmd->m_buttons &= ~IN_ATTACK2;

	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->can_fire(true))
	{
		if (m_pcmd->m_buttons & IN_ATTACK && !g_ctx.globals.weapon->is_non_aim() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			m_pcmd->m_buttons &= ~IN_ATTACK;
		else if ((m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2) && (g_ctx.globals.weapon->is_knife() || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_HEALTHSHOT)
		{
			if (m_pcmd->m_buttons & IN_ATTACK)
				m_pcmd->m_buttons &= ~IN_ATTACK;

			if (m_pcmd->m_buttons & IN_ATTACK2)
				m_pcmd->m_buttons &= ~IN_ATTACK2;
		}
	}

	if (m_pcmd->m_buttons & IN_FORWARD && m_pcmd->m_buttons & IN_BACK)
	{
		m_pcmd->m_buttons &= ~IN_FORWARD;
		m_pcmd->m_buttons &= ~IN_BACK;
	}

	if (m_pcmd->m_buttons & IN_MOVELEFT && m_pcmd->m_buttons & IN_MOVERIGHT)
	{
		m_pcmd->m_buttons &= ~IN_MOVELEFT;
		m_pcmd->m_buttons &= ~IN_MOVERIGHT;
	}

	g_ctx.send_packet = true;
	g_ctx.globals.tickbase_shift = 0;
	g_ctx.globals.double_tap_fire = false;
	g_ctx.globals.force_send_packet = false;
	g_ctx.globals.exploits = exploit::get().double_tap_key || exploit::get().hide_shots_key;
	g_ctx.globals.current_weapon = g_ctx.globals.weapon->get_weapon_group(g_cfg.ragebot.enable);
	g_ctx.globals.slowwalking = false;
	g_ctx.globals.original_forwardmove = m_pcmd->m_forwardmove;
	g_ctx.globals.original_sidemove = m_pcmd->m_sidemove;
	g_ctx.globals.tickrate = 1 / m_globals()->m_intervalpertick;

	antiaim::get().breaking_lby = false;
	misc::get().fast_stop(m_pcmd);

	if (g_cfg.misc.bunnyhop)
		bunnyhop::get().create_move();

	misc::get().SlideWalk(m_pcmd);
	misc::get().NoDuck(m_pcmd);

	misc::get().AutoCrouch(m_pcmd);
	GrenadePrediction::get().Tick(m_pcmd->m_buttons);

	if (g_cfg.misc.crouch_in_air && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		m_pcmd->m_buttons |= IN_DUCK;

	engine_prediction::m_misc.m_bOverrideModifier = true;
	engine_prediction::update();
	engine_prediction::predict(m_pcmd, g_ctx.local());
	//local_animations::get().update_prediction_animations();

	g_ctx.globals.eye_pos = g_ctx.local()->get_shoot_position();

	if (g_cfg.misc.airstrafe)
		airstrafe::get().create_move(m_pcmd);

	if (key_binds::get().get_key_bind_state(19) && engineprediction::get().backup_data.flags & FL_ONGROUND && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		m_pcmd->m_buttons |= IN_JUMP;

	if (key_binds::get().get_key_bind_state(21))
		slowwalk::get().create_move(m_pcmd);

	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->is_non_aim() && engineprediction::get().backup_data.flags & FL_ONGROUND && g_ctx.local()->m_fFlags() & FL_ONGROUND)
		slowwalk::get().create_move(m_pcmd, 0.95f + 0.003125f * (16 - m_clientstate()->iChokedCommands));

	if (!g_ctx.globals.should_recharge)
		fakelag::get().Createmove();

	g_ctx.globals.aimbot_working = false;
	g_ctx.globals.revolver_working = false;

	auto backup_velocity = g_ctx.local()->m_vecVelocity();
	auto backup_abs_velocity = g_ctx.local()->m_vecAbsVelocity();

	g_ctx.local()->m_vecVelocity() = engineprediction::get().backup_data.velocity;
	g_ctx.local()->m_vecAbsVelocity() = engineprediction::get().backup_data.velocity;

	g_ctx.globals.weapon->update_accuracy_penality();

	g_ctx.local()->m_vecVelocity() = backup_velocity;
	g_ctx.local()->m_vecAbsVelocity() = backup_abs_velocity;

	g_ctx.globals.inaccuracy = g_ctx.globals.weapon->get_inaccuracy();
	g_ctx.globals.spread = g_ctx.globals.weapon->get_spread();

	aim::get().run(m_pcmd);
	legit_bot::get().createmove(m_pcmd);

	zeusbot::get().run(m_pcmd);
	knifebot::get().run(m_pcmd);

	misc::get().automatic_peek(m_pcmd, wish_yaw.y);

	antiaim::get().desync_angle = 0.0f;
	antiaim::get().create_move(m_pcmd);

	if (m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 6) //-V648
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}
	else if (!m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 16) //-V648
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_send_packet)
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_choke_packet)
	{
		g_ctx.globals.should_choke_packet = false;
		g_ctx.globals.should_send_packet = true;
		g_ctx.send_packet = false;
	}

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
		{
			static auto weapon_recoil_scale = m_cvar()->FindVar(crypt_str("weapon_recoil_scale"));

			if (g_cfg.ragebot.enable)
				m_pcmd->m_viewangles -= g_ctx.local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat();

			if (!g_ctx.globals.fakeducking)
			{
				g_ctx.globals.force_send_packet = true;
				g_ctx.globals.should_choke_packet = true;
				g_ctx.send_packet = true;
				fakelag::get().started_peeking = false;
			}

			aim::get().last_shoot_position = g_ctx.globals.eye_pos;
			g_ctx.globals.m_ragebot_shot_nr = m_pcmd->m_command_number;

			if (!double_tap_aim_check)
				g_ctx.globals.double_tap_aim = false;
		}
	}
	else if (g_ctx.globals.weapon->is_knife() && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2))
	{
		if (!g_ctx.globals.fakeducking) {
			g_ctx.globals.force_send_packet = true;
			g_ctx.globals.should_choke_packet = true;
			g_ctx.send_packet = true;
			fakelag::get().started_peeking = false;
		}

		g_ctx.globals.m_ragebot_shot_nr = m_pcmd->m_command_number;
	}

	if (g_ctx.globals.fakeducking)
		g_ctx.globals.force_send_packet = g_ctx.send_packet;

	for (auto& backup : aim::get().backup)
		backup.adjust_player();

	auto backup_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (exploit::get().double_tap(m_pcmd))
		exploit::get().hide_shots(m_pcmd, false);
	else
	{
		g_ctx.globals.ticks_allowed = backup_ticks_allowed;
		exploit::get().hide_shots(m_pcmd, true);
	}

	ClientFrameStage_t stage;

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (!double_tap_aim_check && m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
			g_ctx.globals.double_tap_aim = false;
	}

	if (m_globals()->m_tickcount - g_ctx.globals.last_aimbot_shot > 16) //-V807
	{
		g_ctx.globals.double_tap_aim = false;
		g_ctx.globals.double_tap_aim_check = false;
	}

	engine_prediction::restore(g_ctx.local());
	engine_prediction::m_misc.m_bOverrideModifier = false;

	if (g_ctx.globals.loaded_script)
		for (auto current : c_lua::get().hooks.getHooks(crypt_str("on_createmove")))
			current.func();

	if (g_cfg.misc.anti_untrusted)
		math::normalize_angles(m_pcmd->m_viewangles);
	else
		m_pcmd->m_viewangles.y = math::normalize_yaw(m_pcmd->m_viewangles.y);

	util::movement_fix(wish_yaw, m_pcmd);

	if (g_ctx.globals.should_recharge)
		g_ctx.send_packet = true;

	if (g_ctx.globals.ticks_choke)
	{
		g_ctx.send_packet = g_ctx.globals.force_send_packet;
		--g_ctx.globals.ticks_choke;
	}

	auto& correct = g_ctx.globals.data.emplace_front();

	correct.command_number = m_pcmd->m_command_number;
	correct.choked_commands = m_clientstate()->iChokedCommands + 1;
	correct.tickcount = m_globals()->m_tickcount;

	if (g_ctx.send_packet)
		g_ctx.globals.choked_number.clear();
	else
		g_ctx.globals.choked_number.emplace_back(correct.command_number);

	while (g_ctx.globals.data.size() > g_ctx.globals.tickrate)
		g_ctx.globals.data.pop_back();

	auto& out = g_ctx.globals.commands.emplace_back();

	out.is_outgoing = g_ctx.send_packet;
	out.is_used = false;
	out.command_number = m_pcmd->m_command_number;
	out.previous_command_number = 0;

	while (g_ctx.globals.commands.size() > g_ctx.globals.tickrate)
		g_ctx.globals.commands.pop_front();

	if (!g_ctx.send_packet && !m_gamerules()->m_bIsValveDS())
	{
		auto net_channel = m_clientstate()->pNetChannel;

		if (net_channel->m_nChokedPackets > 0 && !(net_channel->m_nChokedPackets % 4))
		{
			auto backup_choke = net_channel->m_nChokedPackets;
			net_channel->m_nChokedPackets = 0;

			net_channel->send_datagram();
			--net_channel->m_nOutSequenceNr;

			net_channel->m_nChokedPackets = backup_choke;
		}
	}

	if (g_ctx.send_packet && g_ctx.globals.should_send_packet)
		g_ctx.globals.should_send_packet = false;

	if (g_cfg.misc.buybot_enable && g_ctx.globals.should_buy)
	{
		--g_ctx.globals.should_buy;

		if (!g_ctx.globals.should_buy)
		{
			std::string buy;

			switch (g_cfg.misc.buybot1)
			{
			case 1:
				buy += crypt_str("buy g3sg1; ");
				break;
			case 2:
				buy += crypt_str("buy awp; ");
				break;
			case 3:
				buy += crypt_str("buy ssg08; ");
				break;
			}

			switch (g_cfg.misc.buybot2)
			{
			case 1:
				buy += crypt_str("buy elite; ");
				break;
			case 2:
				buy += crypt_str("buy deagle; buy revolver; ");
				break;
			}

			if (g_cfg.misc.buybot3[BUY_ARMOR])
				buy += crypt_str("buy vesthelm; buy vest; ");

			if (g_cfg.misc.buybot3[BUY_TASER])
				buy += crypt_str("buy taser; ");

			if (g_cfg.misc.buybot3[BUY_GRENADES])
				buy += crypt_str("buy molotov; buy hegrenade; buy smokegrenade; ");

			if (g_cfg.misc.buybot3[BUY_DEFUSER])
				buy += crypt_str("buy defuser; ");

			m_engine()->ExecuteClientCmd(buy.c_str());
		}
	}

	local_animations::get().store_cm_animation_data(m_pcmd);
	g_ctx.globals.fixed_viewangles = m_pcmd->m_viewangles;

	g_ctx.globals.in_createmove = false;
	send_packet = g_ctx.send_packet;

	verified->m_cmd = *m_pcmd;
	verified->m_crc = m_pcmd->GetChecksum();
}

__declspec(naked) void __stdcall hooks::hooked_createmove_proxy(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebx
		push esp
		push dword ptr[esp + 20]
		push dword ptr[esp + 0Ch + 8]
		push dword ptr[esp + 10h + 4]
		call hooked_createmove
		pop ebx
		retn 0Ch
	}
}
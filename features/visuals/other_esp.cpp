#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "..\lagcompensation\local_animations.h"
#include "../../menu/menu.h"
#include "../../utils/Render/Render.h"

#include "../exploit/exploit.h"

bool can_penetrate(weapon_t* weapon)
{
	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return false;

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f)
		return false;

	auto eye_pos = g_ctx.globals.eye_pos;
	auto hits = 1;
	auto damage = (float)weapon_info->iDamage;
	auto penetration_power = weapon_info->flPenetration;

	static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

	return autowall::get().handle_bullet_penetration(weapon_info, trace, eye_pos, direction, hits, damage, penetration_power, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat(), trace.hit_entity);
}

void otheresp::penetration_reticle()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.penetration_reticle)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto color = Color::Red;

	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color::Green;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	trace_t enterTrace;
	CTraceFilter filter;
	Ray_t ray;
	auto weapon_info = weapon->get_csweapon_info();
	if (!weapon_info)
		return;
	Vector viewangles; m_engine()->GetViewAngles(viewangles);
	Vector direction; math::angle_vectors(viewangles, direction);
	Vector start = g_ctx.globals.eye_pos;
	auto m_flMaxRange = weapon_info->flRange * 2;
	Vector end = start + (direction * m_flMaxRange);

	filter.pSkip = g_ctx.local();
	ray.Init(start, end);
	m_trace()->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &enterTrace);

	float anglez = math::dot_product(Vector(0, 0, 1), enterTrace.plane.normal);
	float invanglez = math::dot_product(Vector(0, 0, -1), enterTrace.plane.normal);
	float angley = math::dot_product(Vector(0, 1, 0), enterTrace.plane.normal);
	float invangley = math::dot_product(Vector(0, -1, 0), enterTrace.plane.normal);
	float anglex = math::dot_product(Vector(1, 0, 0), enterTrace.plane.normal);
	float invanglex = math::dot_product(Vector(-1, 0, 0), enterTrace.plane.normal);

	if (anglez > 0.5 || invanglez > 0.5)
		g_Render->filled_rect_world(enterTrace.endpos, Vector2D(3, 3), Color(color.r(), color.g(), color.b(), 100), 0);
	else if (angley > 0.5 || invangley > 0.5)
		g_Render->filled_rect_world(enterTrace.endpos, Vector2D(3, 3), Color(color.r(), color.g(), color.b(), 100), 1);
	else if (anglex > 0.5 || invanglex > 0.5)
		g_Render->filled_rect_world(enterTrace.endpos, Vector2D(3, 3), Color(color.r(), color.g(), color.b(), 100), 2);
}

void otheresp::hitmarker_paint()
{
	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		Vector world;

		if (math::world_to_screen(hitmarker.point, world))
		{
			auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
			hitmarker.hurt_color.SetAlpha(alpha);

			auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

			g_Render->DrawLine(world.x + 2 + offset, world.y - 2 - offset, world.x + 6 + offset, world.y - 6 - offset, Color(255, 255, 255));
			g_Render->DrawLine(world.x + 2 + offset, world.y + 2 + offset, world.x + 6 + offset, world.y + 6 + offset, Color(255, 255, 255));
			g_Render->DrawLine(world.x - 2 - offset, world.y + 2 + offset, world.x - 6 - offset, world.y + 6 + offset, Color(255, 255, 255));
			g_Render->DrawLine(world.x - 2 - offset, world.y - 2 - offset, world.x - 6 - offset, world.y - 6 - offset, Color(255, 255, 255));
		}
	}
}

void otheresp::damage_marker_paint()
{
	for (auto i = 0; i < m_globals()->m_maxclients; i++)
	{
		if (damage_marker[i].hurt_time + 1.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;
			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5);
			damage_marker[i].hurt_color.SetAlpha(alpha);

			g_Render->DrawString(screen.x, screen.y -= 0.5 * (127.5 - alpha), damage_marker[i].hurt_color, render2::centered_x | render2::centered_y | render2::outline, c_menu::get().OpenSansBold, "- %i", damage_marker[i].damage);
		}
	}
}

void otheresp::draw_indicators()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto h = height - 325;

	for (auto& indicator : m_indicators)
	{
		render::get().text(fonts[INDICATORFONT], 27, h, indicator.m_color, HFONT_CENTERED_Y, indicator.m_text.c_str());
		h -= 25;
	}
	
	m_indicators.clear();
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device);

void otheresp::spread_crosshair(LPDIRECT3DDEVICE9 device)
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.show_spread)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	int w, h;
	m_engine()->GetScreenSize(w, h);

	draw_circe((float)w * 0.5f, (float)h * 0.5f, g_ctx.globals.inaccuracy * 500.0f, 50, D3DCOLOR_RGBA(g_cfg.esp.show_spread_color.r(), g_cfg.esp.show_spread_color.g(), g_cfg.esp.show_spread_color.b(), g_cfg.esp.show_spread_color.a()), D3DCOLOR_RGBA(0, 0, 0, 0), device);
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2 = nullptr;
	std::vector <CUSTOMVERTEX2> circle(resolution + 2);

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0.0f;

	circle[0].rhw = 1.0f;
	circle[0].color = color2;

	for (auto i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - radius * sin(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0.0f;

		circle[i].rhw = 1.0f;
		circle[i].color = color;
	}

	device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX2), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, nullptr);

	if (!g_pVB2)
		return;

	void* pVertices;

	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX2), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX2));
	g_pVB2->Unlock();

	device->SetTexture(0, nullptr);
	device->SetPixelShader(nullptr);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX2));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);

	g_pVB2->Release();
}

void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();
	auto color_main = g_cfg.misc.automatic_peek_color;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 0.1;

	if (key_binds::get().get_key_bind_state(18)) {

		render::get().Draw3DCircleGradient(position, 32, color_main, alpha);
	}

}
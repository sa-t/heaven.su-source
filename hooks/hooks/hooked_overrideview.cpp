#include "..\hooks.hpp"
#include "..\..\features\visuals\GrenadePrediction.h"
#include "..\..\features\misc\fakelag.h"
#include "..\..\features\lagcompensation\local_animations.h"

using OverrideView_t = void(__stdcall*)(CViewSetup*);

void thirdperson(bool fakeducking);

void __stdcall hooks::hooked_overrideview(CViewSetup* viewsetup)
{
    static auto original_fn = clientmode_hook->get_func_address <OverrideView_t>(18);
    g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

    if (!viewsetup)
        return original_fn(viewsetup);

    if (g_ctx.local())
    {
        static auto fakeducking = false;

        if (!fakeducking && g_ctx.globals.fakeducking)
            fakeducking = true;
        else if (fakeducking && !g_ctx.globals.fakeducking && (!g_ctx.local()->get_animation_state()->m_flAnimDuckAmount || g_ctx.local()->get_animation_state()->m_flAnimDuckAmount == 1.0f))
            fakeducking = false;

        if (!g_ctx.local()->is_alive())
            fakeducking = false;

        auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

        if (weapon)
        {
            if (!g_ctx.local()->m_bIsScoped() && g_cfg.player.enable)
                viewsetup->fov += g_cfg.esp.fov;
            else if (g_cfg.esp.removals[REMOVALS_ZOOM] && g_cfg.player.enable)
            {
                if (weapon->m_zoomLevel() == 1)
                    viewsetup->fov = 90.0f + (float)g_cfg.esp.fov;
                else
                    viewsetup->fov += (float)g_cfg.esp.fov;
            }
        }
        else if (g_cfg.player.enable)
            viewsetup->fov += g_cfg.esp.fov;

        if (weapon)
        {
            auto viewmodel = (entity_t*)m_entitylist()->GetClientEntityFromHandle(g_ctx.local()->m_hViewModel());

            if (viewmodel)
            {
                auto eyeAng = viewsetup->angles;
                eyeAng.z -= (float)g_cfg.esp.viewmodel_roll;

                viewmodel->set_abs_angles(eyeAng);
            }

            if (weapon->is_grenade() && g_cfg.esp.grenade_prediction && g_cfg.player.enable)
                GrenadePrediction::get().View(viewsetup, weapon);
        }

        if (g_cfg.player.enable && (g_cfg.misc.thirdperson_toggle.key > KEY_NONE && g_cfg.misc.thirdperson_toggle.key < KEY_MAX || g_cfg.misc.thirdperson_when_spectating))
            thirdperson(fakeducking);
        else
        {
            g_ctx.globals.in_thirdperson = false;
            m_input()->bCameraInThirdPerson = false;
        }

        original_fn(viewsetup);

        if (fakeducking)
        {
            viewsetup->origin = g_ctx.local()->GetAbsOrigin() + Vector(0.0f, 0.0f, m_gamemovement()->GetPlayerViewOffset(false).z + 0.064f);

            if (m_input()->bCameraInThirdPerson)
            {
                auto camera_angles = Vector(m_input()->vecCameraOffset.x, m_input()->vecCameraOffset.y, 0.0f);
                auto camera_forward = ZERO;

                math::angle_vectors(camera_angles, camera_forward);
                math::VectorMA(viewsetup->origin, -m_input()->vecCameraOffset.z, camera_forward, viewsetup->origin);
            }
        }
    }
    else
        return original_fn(viewsetup);
}
void Thirdperson_Init(bool fakeducking, float progress)
{

    static float current_fraction = 0.0f;

    auto distance = ((float)g_cfg.misc.thirdperson_distance) * progress;

    Vector angles, inverse_angles;

    m_engine()->GetViewAngles(angles);

    m_engine()->GetViewAngles(inverse_angles);

    inverse_angles.z = distance;

    Vector forward, right, up;

    math::angle_vectors(inverse_angles, &forward, &right, &up);

    auto eye_pos = fakeducking ? g_ctx.local()->GetAbsOrigin() + m_gamemovement()->GetPlayerViewOffset(false) : g_ctx.local()->GetAbsOrigin() + g_ctx.local()->m_vecViewOffset();

    auto offset = eye_pos + forward * -distance + right + up;

    CTraceFilterWorldOnly filter;

    trace_t tr;

    m_trace()->TraceRay(Ray_t(eye_pos, offset, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 16.0f)), 131083, &filter, &tr);

    if (current_fraction > tr.fraction)

        current_fraction = tr.fraction;

    else if (current_fraction > 0.9999f)

        current_fraction = 1.0f;

    current_fraction = math::interpolate(current_fraction, tr.fraction, m_globals()->m_frametime * 10.0f);

    angles.z = distance * current_fraction;

    m_input()->vecCameraOffset = angles;
}

void thirdperson(bool fakeducking)
{
    {
        static float progress;

        static bool in_transition;

        static auto in_thirdperson = false;

        if (!in_thirdperson && g_ctx.globals.in_thirdperson)
        {
            in_thirdperson = true;
        }

        else if (in_thirdperson && !g_ctx.globals.in_thirdperson)
            in_thirdperson = false;

        if (g_ctx.local()->is_alive() && in_thirdperson)
        {
            in_transition = false;

            if (!m_input()->bCameraInThirdPerson)
            {

                m_input()->bCameraInThirdPerson = true;

            }

        }
        else
        {
            progress -= m_globals()->m_frametime * 8.f + (progress / 100);
            progress = std::clamp(progress, 0.f, 1.f);

            if (!progress)
                m_input()->bCameraInThirdPerson = false;

            else
            {

                in_transition = true;

                m_input()->bCameraInThirdPerson = true;

            }

        }

        if (m_input()->bCameraInThirdPerson && !in_transition)
        {

            progress += m_globals()->m_frametime * 8.f + (progress / 100);

            progress = std::clamp(progress, 0.f, 1.f);

        }

        Thirdperson_Init(fakeducking, progress);
    }

    {
        static auto require_reset = false;

        if (g_ctx.local()->is_alive())
        {
            require_reset = false;
            return;
        }

        if (g_cfg.misc.thirdperson_when_spectating)
        {
            if (require_reset)
                g_ctx.local()->m_iObserverMode() = OBS_MODE_CHASE;

            if (g_ctx.local()->m_iObserverMode() == OBS_MODE_IN_EYE)
                require_reset = true;


        }
    }
}
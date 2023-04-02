// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"
float EyeAngles[64] = { 0 };

bool was_first_bruteforce = false;
bool was_second_bruteforce = false;

float GetBackwardYaw(player_t* player)
{
    return math::calculate_angle(g_ctx.local()->GetAbsOrigin(), player->GetAbsOrigin()).y;
}

int DetectStand(player_t* player)
{
    Vector src3D, dst3D, forward, right, up, src, dst;
    float back_two, right_two, left_two;
    trace_t tr;
    Ray_t ray, ray2, ray3, ray4, ray5;
    CTraceFilter filter;

    math::angle_vectors(Vector(0, GetBackwardYaw(player), 0), &forward, &right, &up);

    filter.pSkip = player;
    src3D = player->get_shoot_position();
    dst3D = src3D + (forward * 384); //Might want to experiment with other numbers, incase you don't know what the number does, its how far the trace will go. Lower = shorter.

    ray.Init(src3D, dst3D);
    m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
    back_two = (tr.endpos - tr.startpos).Length();

    ray2.Init(src3D + right * 30, dst3D + right * 30);
    m_trace()->TraceRay(ray2, MASK_SHOT, &filter, &tr);
    right_two = (tr.endpos - tr.startpos).Length();

    ray3.Init(src3D - right * 30, dst3D - right * 30);
    m_trace()->TraceRay(ray3, MASK_SHOT, &filter, &tr);
    left_two = (tr.endpos - tr.startpos).Length();

    if (left_two > right_two)
        return -1;
    else if (right_two > left_two)
        return 1;
    else
        return 0;
}

float GetForwardYaw(player_t* player)
{
    return math::normalize_yaw(GetBackwardYaw(player) - 180.f);
}

resolver_side resolver::FreeStand(player_t* e)
{

    float angle = math::normalize_yaw(e->m_angEyeAngles().y);

    bool forward = fabsf(math::normalize_yaw(angle - GetForwardYaw(e))) < 90.f;

    auto misses = g_ctx.globals.missed_shots[e->EntIndex()];

    int S = DetectStand(e);

    bool HIGHDELTA = misses >= 3;


    switch (misses % 2)
    {
    case 0:
        switch (last_side)
        {
        case RESOLVER_SECOND:
            return RESOLVER_FIRST;
            break;
        case RESOLVER_FIRST:
            return RESOLVER_SECOND;
            break;
        default:
            if (S == 1)
            {
                if (forward)
                    return RESOLVER_SECOND;
                else
                    return RESOLVER_FIRST;
            }
            else
            {
                if (forward)
                    return RESOLVER_FIRST;
                else
                    return RESOLVER_SECOND;
            }
            break;
        }
        break;
    case 1:
        switch (last_side)
        {
        case RESOLVER_SECOND:
            return RESOLVER_SECOND;
            break;
        case RESOLVER_FIRST:
            return RESOLVER_FIRST;
            break;
        default:
            if (S == 1)
            {
                if (forward)
                    return RESOLVER_FIRST;
                else
                    return RESOLVER_SECOND;
            }
            else
            {
                if (forward)
                    return RESOLVER_SECOND;
                else
                    return RESOLVER_FIRST;
            }
            break;
        }
        break;
    }

}

bool resolver::IsFakewalking(player_t* entity)
{
    bool
        bFakewalking = false,
        stage1 = false,            // stages needed cause we are iterating all layers, eitherwise won't work :)
        stage2 = false,
        stage3 = false;

    for (int i = 0; i < entity->animlayer_count(); i++)
    {
        if (player_record->layers[i].m_nSequence == 26 && player_record->layers[i].m_flWeight < 0.4f)
            stage1 = true;
        if (player_record->layers[i].m_nSequence == 7 && player_record->layers[i].m_flWeight > 0.001f)
            stage2 = true;
        if (player_record->layers[i].m_nSequence == 2 && player_record->layers[i].m_flWeight == 0)
            stage3 = true;
    }

    if (stage1 && stage2)
        bFakewalking = (stage3 || (player->m_fFlags() & FL_DUCKING)); // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2   
    else
        bFakewalking = false;

    return bFakewalking;
}

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
    player = e;
    player_record = record;

    original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
    original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset()
{
    player = nullptr;
    player_record = nullptr;

    side = false;
    fake = false;

    was_first_bruteforce = false;
    was_second_bruteforce = false;

    original_goal_feet_yaw = 0.0f;
    original_pitch = 0.0f;
}

bool CanSeeHitbox(player_t* entity, int HITBOX)
{
    return g_ctx.local()->CanSeePlayer(entity, entity->hitbox_position(HITBOX));
}

struct {
    float movinglby[65];
}records;

bool resolver::Saw(player_t* entity)
{
    if (!(CanSeeHitbox(entity, HITBOX_HEAD) && CanSeeHitbox(entity, HITBOX_LEFT_FOOT) && CanSeeHitbox(entity, HITBOX_RIGHT_FOOT))
        || (CanSeeHitbox(entity, HITBOX_HEAD) && CanSeeHitbox(entity, HITBOX_LEFT_FOOT) && CanSeeHitbox(entity, HITBOX_RIGHT_FOOT)))
        return false;

    return true;
}

resolver_side resolver::TraceSide(player_t* entity)
{
    auto first_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
    auto second_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());
    auto main_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.main), player, g_ctx.local());
    if (main_visible)
    {
        if (first_visible)
            return RESOLVER_SECOND;
        else if (second_visible)
            return  RESOLVER_FIRST;
    }
    else
    {
        if (first_visible)
            return  RESOLVER_FIRST;
        else if (second_visible)
            return  RESOLVER_SECOND;
    }

    return RESOLVER_ORIGINAL;
}

bool ValidPitch(player_t* entity)
{
    int pitch = entity->m_angEyeAngles().x;
    return pitch == 0 || pitch > 90 || pitch < -90;
}

void resolver::NoSpreadResolver()
{
    if (last_side != RESOLVER_ORIGINAL)
    {
        switch (last_side)
        {
        case RESOLVER_ORIGINAL:
            g_ctx.globals.missed_shots[player->EntIndex()] = 0;
            fake = true;
            break;
        case RESOLVER_ZERO:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_LOW_FIRST;

            was_first_bruteforce = false;
            was_second_bruteforce = false;
            return;
        case RESOLVER_FIRST:
            player_record->type = BRUTEFORCE;
            player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;

            was_first_bruteforce = true;
            return;
        case RESOLVER_SECOND:
            player_record->type = BRUTEFORCE;
            player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;

            was_second_bruteforce = true;
            return;
        case RESOLVER_LOW_FIRST:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_LOW_SECOND;
            return;
        case RESOLVER_LOW_SECOND:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_FIRST;
            return;
        }
    }
    switch (g_ctx.globals.missed_shots[player->EntIndex()])
    {
    case 0:
        player_record->side = RESOLVER_SECOND;
        break;

    case 1:
        player_record->side = RESOLVER_FIRST;
        break;

    case 2:
        player_record->side = RESOLVER_LOW_FIRST;
        break;

    case 3:
        player_record->side = RESOLVER_LOW_SECOND;
        break;

    case 4:
        player_record->side = RESOLVER_FIRST;
        break;

    case 5:
        player_record->side = RESOLVER_SECOND;
        break;
    }
    if (g_ctx.globals.missed_shots[player->EntIndex()] == 5)
        g_ctx.globals.missed_shots[player->EntIndex()] = 0;
}



void resolver::resolve_yaw()
{

    player_info_t player_info;

    if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
        return;

    if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum()) //-V807
    {
        player_record->side = RESOLVER_ORIGINAL;
        return;
    }

    if (aim::get().last_target[player->EntIndex()].record.type != LBY)
    {
        switch (last_side)
        {
        case RESOLVER_ORIGINAL:
            g_ctx.globals.missed_shots[player->EntIndex()] = 0;
            fake = true;
            break;
        case RESOLVER_ZERO:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_LOW_FIRST;

            was_first_bruteforce = false;
            was_second_bruteforce = false;
            return;
        case RESOLVER_FIRST:
            player_record->type = BRUTEFORCE;
            player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;

            was_first_bruteforce = true;
            return;
        case RESOLVER_SECOND:
            player_record->type = BRUTEFORCE;
            player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;

            was_second_bruteforce = true;
            return;
        case RESOLVER_LOW_FIRST:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_LOW_SECOND;
            return;
        case RESOLVER_LOW_SECOND:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_FIRST;
            return;
        }
    }

    auto animstate = player->get_animation_state();

    if (!animstate)
    {
        player_record->side = RESOLVER_ORIGINAL;
        return;
    }

    if (!ValidPitch(player))
    {
        if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
            player_record->side = RESOLVER_ORIGINAL;
        else
            player_record->side = FreeStand(player);
        return;
    }



    if (fabs(original_pitch) > 90.0f)
        fake = true;
    else if (!fake)
    {
        player_record->side = RESOLVER_ORIGINAL;
        return;
    }

    if (player->m_vecVelocity().Length2D() > 50)
        records.movinglby[player->EntIndex()] = animstate->m_flGoalFeetYaw;


    auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
    auto valid_lby = true;

    if (animstate->m_flVelocityLengthXY > 0.1f || fabs(animstate->m_flVelocityLengthZ) > 100.f)
        valid_lby = animstate->m_flDurationMoving < 0.22f;

    auto spread = m_cvar()->FindVar("weapon_accuracy_nospread")->GetInt();
    if (spread == 1)
    {
        NoSpreadResolver();
        return;
    }

    if (player->m_vecVelocity().Length2D() > 235 || !player->m_fFlags() & FL_ONGROUND)
        player_record->side = RESOLVER_ORIGINAL;
    else if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
        player_record->side = RESOLVER_SECOND;
    else if (Saw(player))
    {
        if (TraceSide(player) != RESOLVER_ORIGINAL)
            player_record->side = TraceSide(player);
        player_record->type = TRACE;
    }
    else if (fabs(delta) > 30.0f && valid_lby)
    {
        if (g_ctx.globals.missed_shots[player->EntIndex()])
            delta = -delta;

        if (delta > 30.0f)
        {
            player_record->type = LBY;
            player_record->side = RESOLVER_FIRST;
        }
        else if (delta < -30.0f)
        {
            player_record->type = LBY;
            player_record->side = RESOLVER_SECOND;
        }
    }
    else
    {
        auto trace = false;
        if (m_globals()->m_curtime - lock_side > 2.0f)
        {
            auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
            auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

            if (first_visible != second_visible)
            {
                trace = true;
                side = second_visible;
                lock_side = m_globals()->m_curtime;
            }
            else
            {
                auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
                auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

                if (fabs(first_position - second_position) > 1.0f)
                    side = first_position > second_position;
            }
        }
        else
            trace = true;

        if (side)
        {
            player_record->type = trace ? TRACE : DIRECTIONAL;
            player_record->side = RESOLVER_FIRST;
        }
        else
        {
            player_record->type = trace ? TRACE : DIRECTIONAL;
            player_record->side = RESOLVER_SECOND;
        }
    }
}

float resolver::resolve_pitch()
{
    return original_pitch;
}
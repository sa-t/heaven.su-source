#include "..\..\includes.hpp"

struct Local_data
{
    bool visualize_lag = false;

    c_baseplayeranimationstate* prediction_animstate = nullptr;
    c_baseplayeranimationstate* animstate = nullptr;

    Vector cm_stored_viewangles = ZERO;
    int cm_stored_commandnumber = 0;

};

class local_animations : public singleton <local_animations>
{
    bool real_server_update = false;
    bool fake_server_update = false;

    float real_simulation_time = 0.0f;
    float fake_simulation_time = 0.0f;

    CBaseHandle* handle = nullptr;

    float spawntime = 0.0f;
    float tickcount = 0.0f;

    float abs_angles = 0.0f;
    float pose_parameter[24];
    AnimationLayer layers[15];
public:
    Local_data local_data;

    //void run(ClientFrameStage_t stage);
        float m_real_poses[24];
    float m_real_angles = 0.0f;
    AnimationLayer m_real_layers[13];

    float m_real_spawntime = 0.0f;
public:
    void store_cm_animation_data(CUserCmd* cmd);

    void update_fake_animations(CUserCmd* cmd, bool* send_packet);

    void update_local_animations(CUserCmd* cmd, bool* send_packet);

    void build_local_bones(player_t* local);
};
#include "spammers.h"

void spammers::clan_tag()
{
    auto apply = [](const char* tag) -> void
    {
        using Fn = int(__fastcall*)(const char*, const char*);
        static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

        fn(tag, tag);
    };

    static auto removed = false;

    if (!g_cfg.misc.clantag_spammer && !removed)
    {
        removed = true;
        apply(crypt_str(""));
        return;
    }

    if (g_cfg.misc.clantag_spammer)
    {
        auto nci = m_engine()->GetNetChannelInfo();

        if (!nci)
            return;

        static auto time = 1;

        auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)m_globals()->m_tickcount;
        auto intervals = 0.5f / m_globals()->m_intervalpertick;

        auto main_time = (int)(ticks / intervals) % 15;

        if (main_time != time && !m_clientstate()->iChokedCommands)
        {
            auto tag = crypt_str("");

			switch (main_time)
			{
			case 0:
				tag = crypt_str(u8"s ");
				break;
			case 1:
				tag = crypt_str(u8"sh ");
				break;
			case 2:
				tag = crypt_str(u8"shi ");
				break;
			case 3:
				tag = crypt_str(u8"shit ");
				break;
			case 4:
				tag = crypt_str(u8"shitw ");
				break;
			case 5:
				tag = crypt_str(u8"shitwa ");
				break;
			case 6:
				tag = crypt_str(u8"shitwar ");
				break;
			case 7:
				tag = crypt_str(u8"shitware ");
				break;
			case 8:
				tag = crypt_str(u8"shitwar ");
				break;
			case 9:
				tag = crypt_str(u8"shitwa ");
				break;
			case 10:
				tag = crypt_str(u8"shitw ");
				break;
			case 11:
				tag = crypt_str(u8"shit ");
				break;
			case 12:
				tag = crypt_str(u8"shi ");
				break;
			case 13:
				tag = crypt_str(u8"sh ");
				break;
			case 14:
				tag = crypt_str(u8"s ");
				break;
			}

            apply(tag);
            time = main_time;
        }

        removed = false;
    }
}
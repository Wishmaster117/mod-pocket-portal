/* Credits: Micrah/Milestorme Module creator */

#include "Battleground.h"
#include "Configuration/Config.h"
#include "Item.h"
#include "Map.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "GameTime.h"

enum NPC
{
    NPC = 128,
};

uint64 lastUse = 0;

class pocket_portal : public ItemScript
{
public:
    pocket_portal() : ItemScript("pocket_portal") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sConfigMgr->GetOption<bool>("PocketPortal.Enable", true))
            return false;

        if (player->duel || player->GetMap()->IsBattleArena() || player->InBattleground() ||
            player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH) || player->isDead() ||
            player->IsInCombat() || player->IsInFlight() ||
            player->HasStealthAura() || player->HasInvisibilityAura())
        {
            player->SendSystemMessage("You can't use the Pocket Portal right now.");
            return false;
        }

        uint32 COOLDOWN_MS = 30 * IN_MILLISECONDS;
        uint64 now = GameTime::GetGameTimeMS().count();

        if (lastUse > 0 && now < lastUse + COOLDOWN_MS)
                {
                    uint32 secondsLeft = (lastUse + COOLDOWN_MS - now) / IN_MILLISECONDS;
                    player->SendSystemMessage("Pocket Portal is on cooldown. Try again in " + std::to_string(secondsLeft) + " seconds.");
                    return false;
                }
        player->SummonCreature(
            NPC,
            player->GetPositionX() + 1.5f,
            player->GetPositionY() + 1.5f,
            player->GetPositionZ(),
            player->GetOrientation(),
            TEMPSUMMON_TIMED_DESPAWN, COOLDOWN_MS
        );

        lastUse = now;
        return true;
    }
};

void AddSC_pocket_portal()
{
    new pocket_portal();
}


/* Credits: Micrah/Milestorme Module creator */

#include "Battleground.h"
#include "Config.h"
#include "Creature.h"
#include "GameTime.h"
#include "Item.h"
#include "Map.h"
#include "Player.h"
#include "Unit.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"

enum PocketPortalNPC
{
    NPC_POCKET_PORTAL = 128,
    EMOTE_DANCE = 10
};

class pocket_portal : public ItemScript
{
public:
    pocket_portal() : ItemScript("pocket_portal") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
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

        uint32 const COOLDOWN_MS = sConfigMgr->GetOption<int32>("Portal.NpcDuration", 60) * IN_MILLISECONDS;

        uint64 now = GameTime::GetGameTimeMS().count();

        if (m_lastUse > 0 && now < m_lastUse + COOLDOWN_MS)
                {
                    uint32 secondsLeft = (m_lastUse + COOLDOWN_MS - now) / IN_MILLISECONDS;
                    player->SendSystemMessage("Pocket Portal is on cooldown. Try again in " + std::to_string(secondsLeft) + " seconds.");
                    return false;
                }
        Creature* npc = player->SummonCreature(
            NPC_POCKET_PORTAL,
            player->GetPositionX() + 1.5f,
            player->GetPositionY() + 1.5f,
            player->GetPositionZ() + 0.15f,
            player->GetOrientation(),
            TEMPSUMMON_TIMED_DESPAWN, COOLDOWN_MS
        );
        if (npc) {
            npc->SetFacingToObject(player);
            npc->HandleEmoteCommand(EMOTE_DANCE); // Doesn't work :(
        }

        m_lastUse = now;
        return true;
    }
private:
    uint64 m_lastUse = 0;
};

void AddSC_pocket_portal()
{
    new pocket_portal();
}


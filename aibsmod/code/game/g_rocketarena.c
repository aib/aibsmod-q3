#include "g_local.h"

void ra_register_hit(gentity_t *attacker, gentity_t *inflictor, gentity_t *target)
{
	if (!Q_stricmp(inflictor->classname, "grenade")) { //air grenade
		//play "impressive" on player
		attacker->client->ps.persistant[PERS_IMPRESSIVE_COUNT]++;

		//add the sprite over the player's head
		attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		attacker->client->ps.eFlags |= EF_AWARD_IMPRESSIVE;
		attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;
	} else { //air rocket
		//play "excellent" on player
		attacker->client->ps.persistant[PERS_EXCELLENT_COUNT]++;

		//add the sprite over the player's head
		attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		attacker->client->ps.eFlags |= EF_AWARD_EXCELLENT;
		attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;
	}

	if (!Q_stricmp(inflictor->classname, "rocket")) {
		G_Printf("%s hit %s with a rocket, flight time: %0.1f\n",
			attacker->client->pers.netname,
			target->client->pers.netname,
			((level.time - inflictor->spawnTime) / 1000.0f)
		);
		G_Printf("Target velocity at the time: %f %f %f\n", target->client->ps.velocity[0], target->client->ps.velocity[1], target->client->ps.velocity[2]);
	} else {
		G_Printf("%s hit %s with %s.\n", attacker->client->pers.netname, target->client->pers.netname, inflictor->classname);
	}

	if (target->s.groundEntityNum != ENTITYNUM_NONE)
		return;

	if (target->rocketHits == 0)
		target->rocketHitter = attacker->s.clientNum;

	target->rocketHits++;

	G_Printf("Registering hit.\n");
}

void ra_hit_ground(gentity_t *ent)
{
//	player_die(ent, NULL, &g_entities[ent->rocketHitter], 100, MOD_ROCKET);
	G_Damage(ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_ROCKET);
}

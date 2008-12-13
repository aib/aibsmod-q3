#include "g_local.h"

void ra_register_hit(gentity_t *attacker, gentity_t *inflictor, gentity_t *target)
{
	float targetVelocity;
	float flightTime;

	//Don't register ground hits
	if (target->s.groundEntityNum != ENTITYNUM_NONE)
		return;

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

	targetVelocity = sqrt(
		target->s.pos.trDelta[0]*target->s.pos.trDelta[0] +
		target->s.pos.trDelta[1]*target->s.pos.trDelta[1] +
		target->s.pos.trDelta[2]*target->s.pos.trDelta[2]
	);

	flightTime = (level.time - inflictor->spawnTime) / 1000.0f;

	/*G_Printf("%s hit %s with a %s, flight time: %0.1f, target velocity: %.1f\n",
		attacker->client->pers.netname,
		target->client->pers.netname,
		inflictor->classname,
		flightTime,
		targetVelocity
	);*/

	if (target->rocketHits == 0) //first hit
		target->rocketHitter = attacker->s.clientNum;

	if (target->rocketHitter == attacker->s.clientNum) //subsequent hits, must be from the same attacker
		target->rocketHits++;
}

void ra_hit_ground(gentity_t *ent)
{
	if (ent->rocketHits) {
		//just kill the player, player_die() should take care of setting the fragger, MOD, etc.
		if (ent->health > 0)
			G_Damage(ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_AIRROCKET);
	}
}

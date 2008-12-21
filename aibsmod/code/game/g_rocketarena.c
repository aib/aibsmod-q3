#include "g_local.h"

void ra_register_hit(gentity_t *attacker, gentity_t *inflictor, gentity_t *target)
{
	gentity_t *ent;
	float targetFlightTime;
	float targetVelocity;
	float rocketFlightTime;
	float ownerFlightTime;
	float ownerVelocity;

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

	targetFlightTime = (level.time - target->jumpTime) / 1000.0f;
	targetVelocity = VectorMagnitude(target->s.pos.trDelta);
	rocketFlightTime = (level.time - inflictor->spawnTime) / 1000.0f;
	ownerFlightTime = (level.time - inflictor->jumpTime) / 1000.0f;
	ownerVelocity = inflictor->ownerVelocity;

	ent = G_TempEntity(attacker->r.currentOrigin, EV_ROCKETARENA_HIT);
	ent->s.otherEntityNum = target->s.number;
	ent->s.otherEntityNum2 = attacker->s.number;
	ent->s.angles2[0] = targetFlightTime;
	ent->s.angles2[1] = targetVelocity;
	ent->s.angles2[2] = rocketFlightTime;
	ent->s.origin2[0] = ownerFlightTime;
	ent->s.origin2[1] = ownerVelocity;

//	ent->r.svFlags = SVF_BROADCAST;	//send to everyone

/*	G_Printf("%s hit %s with a %s, rocket flight time: %0.1f, target flight time: %0.1f, target velocity: %.1f\n",
		attacker->client->pers.netname,
		target->client->pers.netname,
		inflictor->classname,
		rocketFlightTime,
		targetFlightTime,
		targetVelocity
	);*/

	if (target->rocketHits == 0) //first hit
		target->rocketHitter = attacker->s.clientNum;

	if (target->rocketHitter == attacker->s.clientNum) //subsequent hits, must be from the same attacker
		target->rocketHits++;
}

void ra_hit_ground(gentity_t *ent)
{
	ent->jumpTime = level.time;

	if (ent->rocketHits) {
		//just kill the player, player_die() should take care of setting the fragger, MOD, etc.
		if (ent->health > 0)
			G_Damage(ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_AIRROCKET);
	}
}

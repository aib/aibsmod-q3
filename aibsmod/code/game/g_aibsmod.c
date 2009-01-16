#include "g_local.h"

//vector maginute
float VectorMagnitude(const vec3_t v)
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

//set angles
void G_SetAngles(gentity_t *ent, vec3_t angles)
{
	VectorCopy(angles, ent->s.apos.trBase);
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorClear(ent->s.apos.trDelta);

	VectorCopy(angles, ent->r.currentAngles);

	VectorCopy(angles, ent->s.angles);
}

//rotate entity's bbox as best as you can
void G_SetRotatedBoundingBox(gentity_t *ent, const vec3_t orgmins, const vec3_t orgmaxs)
{
	vec3_t	tmins, tmaxs;
	vec3_t	tmins2, tmaxs2;

	RotatePointAroundVector(tmins, axisDefault[1], orgmins, ent->r.currentAngles[0]);
	RotatePointAroundVector(tmaxs, axisDefault[1], orgmaxs, ent->r.currentAngles[0]);

	RotatePointAroundVector(tmins2, axisDefault[2], tmins, ent->r.currentAngles[1]);
	RotatePointAroundVector(tmaxs2, axisDefault[2], tmaxs, ent->r.currentAngles[1]);

	ent->r.mins[0] = MIN(tmins2[0], tmaxs2[0]);
	ent->r.maxs[0] = MAX(tmins2[0], tmaxs2[0]);
	ent->r.mins[1] = MIN(tmins2[1], tmaxs2[1]);
	ent->r.maxs[1] = MAX(tmins2[1], tmaxs2[1]);
	ent->r.mins[2] = MIN(tmins2[2], tmaxs2[2]);
	ent->r.maxs[2] = MAX(tmins2[2], tmaxs2[2]);
}

//Gives all weapons + infinite ammo
void give_all_weapons(gclient_t *player)
{
	int i;

	//copied from "give weapons"
	player->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_GRAPPLING_HOOK ) - ( 1 << WP_NONE );

	//copied from "give ammo"
	for (i=0; i<MAX_WEAPONS; i++)
		player->ps.ammo[i] = -1;
}

//Give rocket+grenade + infinite ammo
void give_rocketarena_weapons(gclient_t *player)
{
	player->ps.stats[STAT_WEAPONS] = (1 << WP_GRENADE_LAUNCHER) | (1 << WP_ROCKET_LAUNCHER);

	player->ps.ammo[WP_GAUNTLET] = -1;
	player->ps.ammo[WP_GRENADE_LAUNCHER] = -1;
	player->ps.ammo[WP_ROCKET_LAUNCHER] = -1;
}

void teleport_player_straight(gentity_t *player)
{
	vec3_t	forward, right, up, muzzle;
	vec3_t	endpoint, normal, endpoint_backup;
	trace_t	tr;
	int		i;
	int		backtrace_done;

	//calculate forward, right, up and muzzle
	AngleVectors(player->client->ps.viewangles, forward, right, up);
	CalcMuzzlePoint(player, forward, right, up, muzzle);

	//trace the shot forward
	VectorMA(muzzle, MAX_TELEPORT_DISTANCE, forward, endpoint);
	trap_Trace(&tr, muzzle, NULL, NULL, endpoint, player->s.number, MASK_TELEPORT);
	VectorCopy(tr.plane.normal, normal);

	//save the result in endpoint
	VectorMA(muzzle, tr.fraction * MAX_TELEPORT_DISTANCE, forward, endpoint);
	VectorCopy(endpoint, endpoint_backup);

	backtrace_done = 0;
	//first backtrace using the normal
	for (i=0; i<TELEPORT_ANTINORMAL_COUNT; ++i) {
		trap_Trace(&tr, endpoint, player->r.mins, player->r.maxs, endpoint, player->s.number, MASK_TELEPORT);

		if (!tr.startsolid) { //we're free!
			backtrace_done = 1;
			break;
		}

		VectorMA(endpoint, TELEPORT_BACKTRACE_FRACTION, normal, endpoint);
	}

	if (backtrace_done) { //move us another unit backwards for good measure
		VectorAdd(endpoint, normal, endpoint);
	} else {
		//restore endpoint
		VectorCopy(endpoint_backup, endpoint);

		//backtrace using the forward vector
		for (i=0; i<TELEPORT_BACKTRACE_COUNT; ++i) {
			trap_Trace(&tr, endpoint, player->r.mins, player->r.maxs, endpoint, player->s.number, MASK_TELEPORT);

			if (!tr.startsolid) {//we're free!
				backtrace_done = 1;
				break;
			}

			//backtrace in the direction of the colliding surface's normal for the first few tries
			VectorMA(endpoint, -TELEPORT_BACKTRACE_FRACTION, forward, endpoint);
		}

		if (backtrace_done) { //move us another unit backwards for good measure
			VectorSubtract(endpoint, forward, endpoint);
		}
	}

	if (backtrace_done)
		TeleportPlayerWithoutShooting(player, endpoint, NULL);
}

//Called when rambo dies, used to switch rambo
void switch_rambo(gentity_t *oldrambo, gentity_t *newrambo)
{
	gentity_t *tmpent;

	if (!((g_gametype.integer == GT_RAMBO) || (g_gametype.integer == GT_RAMBO_TEAM)))
		return;

	if (oldrambo && oldrambo->client)
		oldrambo->client->ps.powerups[PW_CARRIER] = 0;

	if (newrambo && newrambo->client && (newrambo->health > 0)) { //player stole rambo
		newrambo->client->ps.powerups[PW_CARRIER] = INT_MAX;

		tmpent = G_TempEntity(newrambo->r.currentOrigin, EV_RAMBO_STEAL);
		if (g_gametype.integer == GT_RAMBO_TEAM)
			if (newrambo->client->sess.sessionTeam == TEAM_RED) //red
				tmpent->s.eventParm = 2;
			else
				tmpent->s.eventParm = 3;
		else
			tmpent->s.eventParm = 1;

		tmpent->s.otherEntityNum2 = newrambo->s.number;
		tmpent->r.svFlags = SVF_BROADCAST;	//send to everyone

		if (newrambo->health < newrambo->client->ps.stats[STAT_MAX_HEALTH]) {
			newrambo->health = newrambo->client->ps.stats[STAT_MAX_HEALTH];
			newrambo->client->ps.stats[STAT_HEALTH] = newrambo->client->ps.stats[STAT_MAX_HEALTH];
		}

		level.rambo = newrambo;
	} else if (oldrambo && oldrambo->client) { //player lost rambo
		tmpent = G_TempEntity(newrambo->r.currentOrigin, EV_RAMBO_STEAL);
		tmpent->s.eventParm = 4;
		tmpent->s.otherEntityNum = oldrambo->s.number;
		tmpent->s.otherEntityNum2 = -1;
		tmpent->r.svFlags = SVF_BROADCAST;	//send to everyone

		level.rambo = NULL;
	}

	if ((g_gametype.integer == GT_RAMBO_TEAM) && //team rambo switch
		(oldrambo && oldrambo->client) &&
		(newrambo && newrambo->client) &&
		(oldrambo->client->sess.sessionTeam != newrambo->client->sess.sessionTeam))
	{
		AddScore(newrambo, oldrambo->r.currentOrigin, 10);
	}
}

//based on TeleportPlayer from g_misc.c
void TeleportPlayerWithoutShooting(gentity_t *player, vec3_t dest, vec3_t angles)
{
	gentity_t	*tent;

	// use temp events at source and destination to prevent the effect
	// from getting dropped by a second player event
	if ( player->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		tent = G_TempEntity( player->client->ps.origin, EV_PLAYER_TELEPORT_OUT );
		tent->s.clientNum = player->s.clientNum;

		tent = G_TempEntity( dest, EV_PLAYER_TELEPORT_IN );
		tent->s.clientNum = player->s.clientNum;
	}

	// unlink to make sure it can't possibly interfere with G_KillBox
	trap_UnlinkEntity (player);

	VectorCopy ( dest, player->client->ps.origin );
	player->client->ps.origin[2] += 1;

	// spit the player out
//	AngleVectors( angles, player->client->ps.velocity, NULL, NULL );
//	VectorScale( player->client->ps.velocity, 400, player->client->ps.velocity );
//	player->client->ps.pm_time = 160;		// hold time
//	player->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

	// toggle the teleport bit so the client knows to not lerp
	player->client->ps.eFlags ^= EF_TELEPORT_BIT;

	//set angles
	if (angles != NULL)
		SetClientViewAngle(player, angles);

	// kill anything at the destination
	if ( player->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		G_KillBox (player);
	}

	// save results of pmove
	BG_PlayerStateToEntityState( &player->client->ps, &player->s, qtrue );

	// use the precise origin for linking
	VectorCopy( player->client->ps.origin, player->r.currentOrigin );

	if ( player->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		trap_LinkEntity (player);
	}
}

gentity_t *ClonePlayer(gentity_t *ent)
{
	gentity_t *clone;

	if (!ent->client)
		return NULL;

	clone = G_Spawn();
	clone->client = ent->client;
	clone->classname = "clone";
	clone->s.eType = ET_CLONE;
	clone->s.clientNum = ent->s.clientNum;

	//Set killability and health
	clone->takedamage = qtrue;
	clone->health = ent->health;
	clone->die = player_die;

	//Set contents and bbox
	clone->r.contents = ent->r.contents;
	clone->clipmask = ent->clipmask;
	VectorCopy(ent->r.mins, clone->r.mins);
	VectorCopy(ent->r.maxs, clone->r.maxs);

	G_SetOrigin(clone, ent->r.currentOrigin);

	return clone;
}

//We want clones to behave like a synchonized player, so we'll run ClientThink_real here once every server frame, just like RunClient
//Some stuff needs to be backed up and restored (such as player command arriving times)
//Yet others need to be copied to actual player state
//god help me --aib
void G_RunClone(gentity_t *clone)
{
	//b_* are backups variables
	gentity_t	*original;
	int			b_commandTime;
	int			b_pm_type;
	vec3_t		b_origin;
	vec3_t		b_velocity;
	int			b_weaponTime;
	int			b_serverTime;

	//Kill clones of disconnected clients
	if (clone->client->pers.connected != CON_CONNECTED) {
		trap_UnlinkEntity(clone);
		clone->inuse = qfalse;
	}

	original = &g_entities[clone->s.clientNum];

	//backup
	b_commandTime = clone->client->ps.commandTime;
	b_pm_type = clone->client->ps.pm_type;
	VectorCopy(clone->client->ps.origin, b_origin);
	VectorCopy(clone->client->ps.velocity, b_velocity);
	b_weaponTime = clone->client->ps.weaponTime;
	b_serverTime = clone->client->pers.cmd.serverTime;

	//now put clone data into the playerState
	clone->client->ps.commandTime = level.previousTime;
	clone->client->ps.pm_type = PM_NORMAL;
	VectorCopy(clone->s.pos.trBase, clone->client->ps.origin);
	VectorCopy(clone->s.pos.trDelta, clone->client->ps.velocity);
	clone->client->pers.cmd.serverTime = level.time;

	//cross your fingers and call ClientThink_real
	ClientThink_real(clone);

	//get modified playerState data back into the clone
	VectorCopy(clone->client->ps.origin, clone->s.pos.trBase);
	VectorCopy(clone->client->ps.velocity, clone->s.pos.trDelta);

	//restore
	clone->client->ps.commandTime = b_commandTime;
	clone->client->ps.pm_type = b_pm_type;
	VectorCopy(b_origin, clone->client->ps.origin);
	VectorCopy(b_velocity, clone->client->ps.velocity);
	clone->client->ps.weaponTime = b_weaponTime;
	clone->client->pers.cmd.serverTime = b_serverTime;
}

/*
//Kills a clone and returns the original entity
gentity_t *KillClone(gentity_t *clone)
{
	gentity_t	*original;

	original = &g_entities[clone->s.clientNum];

	VectorCopy(clone->s.pos.trBase, clone->client->ps.origin);
	VectorCopy(clone->s.pos.trDelta, clone->client->ps.velocity);

	trap_UnlinkEntity(clone);
	clone->inuse = qfalse;

	return original;
}
*/
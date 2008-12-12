#include "g_local.h"

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
	player->ps.stats[STAT_WEAPONS] = (1 << WP_GAUNTLET) | (1 << WP_GRENADE_LAUNCHER) | (1 << WP_ROCKET_LAUNCHER);

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

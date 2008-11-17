#include "g_local.h"

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
	player->client->ps.pm_time = 160;		// hold time
	player->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

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

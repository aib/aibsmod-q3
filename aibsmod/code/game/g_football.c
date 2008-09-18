#include "g_local.h"

#define BALL_RADIUS			8.0f
#define BALL_SHOOT_SPEED	1000
#define BALL_BOUNCE_FACTOR	0.90
#define BALL_AIR_FRICTION	0.995
#define BALL_RESET_TIME		30000
#define BALL_RECATCH_TIME	1000

gentity_t *football_create(vec3_t origin)
{
	gentity_t *ball;

	ball = G_Spawn();
	ball->classname = "football";

	ball->s.eType = ET_FOOTBALL;
	ball->r.svFlags = SVF_BROADCAST;//SVF_USE_CURRENT_ORIGIN;
	ball->clipmask = MASK_SHOT;
//	ball->r.contents = CONTENTS_BODY;

	VectorSet(ball->r.mins, -BALL_RADIUS, -BALL_RADIUS, -BALL_RADIUS);
	VectorSet(ball->r.maxs, +BALL_RADIUS, +BALL_RADIUS, +BALL_RADIUS);

	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;// - 50;		//move a bit on the very first frame
	VectorCopy(origin, ball->s.pos.trBase);
	VectorSet(ball->s.pos.trDelta, 0, 0, 0);

	VectorCopy(origin, ball->r.currentOrigin);

	return ball;
}

void football_reset(gentity_t *ball)
{
	level.ballCarrier = NULL;
	level.ballLastTouchTime = -1;

	ball->r.ownerNum = ENTITYNUM_NONE;
	ball->count = 0;

	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;
	VectorCopy(level.footballSpawnPoint, ball->s.pos.trBase);
	VectorSet(ball->s.pos.trDelta, 0, 0, 0);

	VectorCopy(level.footballSpawnPoint, ball->r.currentOrigin);

	G_Printf("The ball has reset!\n");
}

void football_catch(gentity_t *player)
{
	if (player == level.ballCarrier) //player is already carrying the ball
		return;

	level.ballCarrier = player;
	level.football->r.ownerNum = player->s.number;
//	level.football->count = 0;
	level.football->s.pos.trType = TR_LINEAR;

	G_Printf("%s got the ball!\n", player->client->pers.netname);
}

void football_shoot(gentity_t *ball, gentity_t *player, vec3_t direction)
{
	level.ballCarrier = NULL;
	level.ballLastTouchTime = level.time;

	ball->count = 0;
	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	direction[2] += 0.2f; //extra vertical velocity
	VectorNormalize(direction);

	VectorScale(direction, BALL_SHOOT_SPEED, ball->s.pos.trDelta);

	G_Printf("%s shot the ball!\n", player->client->pers.netname);
}

void G_RunFootball(gentity_t *ball)
{
	vec3_t		origin;
	trace_t		tr;
	int			passent;
	gentity_t	*hitent;

	vec3_t		balloffset;

	//adjust to player location if someone is carrying the ball
	if (level.ballCarrier) {
		//move the ball to front of the player and to foot level
		balloffset[0] = 16 * cos(level.ballCarrier->client->ps.viewangles[YAW] * (M_PI*2 / 360));
		balloffset[1] = 16 * sin(level.ballCarrier->client->ps.viewangles[YAW] * (M_PI*2 / 360));
		balloffset[2] = -14;

		//calculate new position
		VectorAdd(level.ballCarrier->s.pos.trBase, balloffset, origin);

		//trace towards new position
		trap_Trace(&tr, level.ballCarrier->s.pos.trBase, ball->r.mins, ball->r.maxs, origin, ball->r.ownerNum, ball->clipmask);

		//Adjust ball offset so the ball doesn't hit anything
		VectorScale(balloffset, tr.fraction, balloffset);

		VectorAdd(level.ballCarrier->s.pos.trBase, balloffset, ball->s.pos.trBase);
		VectorCopy(level.ballCarrier->s.pos.trDelta, ball->s.pos.trDelta);
		ball->s.pos.trTime = level.ballCarrier->s.pos.trTime;//level.time;

		VectorCopy(ball->s.pos.trBase, ball->r.currentOrigin);
		return;
	}

	if ((level.ballLastTouchTime >= 0) && ((level.time - level.ballLastTouchTime) >= BALL_RESET_TIME))
		football_reset(ball);

//	G_Printf("noone is carrying the ball at %.0f, %.0f, %.0f (%i / %i)\n", ball->s.pos.trBase[0], ball->s.pos.trBase[1], ball->s.pos.trBase[2],ball->r.ownerNum, ball->count);

//	//apply friction
	VectorScale(ball->s.pos.trDelta, BALL_AIR_FRICTION, ball->s.pos.trDelta);
//	if (VectorLength(ball->s.pos.trDelta) < 60) {
//		ball->s.pos.trDelta[0] = 0;
//		ball->s.pos.trDelta[1] = 0;
//		ball->s.pos.trDelta[2] = 0;
//	}

	//get current position
	BG_EvaluateTrajectory(&ball->s.pos, level.time, origin);

	//if the ball has not left the owner's bbox, it shouldn't interact with them
	if (!ball->count) {
		passent = ball->r.ownerNum;
	} else {
		passent = ENTITYNUM_NONE;
	}

	//trace a line from the previous position to the current position
	trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, origin, passent, ball->clipmask);

	if (tr.startsolid || tr.allsolid) {
		//make sure the tr.entityNum is set to the entity we're stuck in
		trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, ball->r.currentOrigin, passent, ball->clipmask);
		tr.fraction = 0;
	} else {
		VectorCopy(tr.endpos, ball->r.currentOrigin);
	}

	trap_LinkEntity(ball);

	if (tr.fraction != 1) {

		//hit a noimpact surface (sky), reset position!
		if (tr.surfaceFlags & SURF_NOIMPACT) {
			football_reset(ball);
			return;
		}

		hitent = &g_entities[tr.entityNum];

		if (hitent->client) {
			if (!level.ballCarrier)
				football_catch(hitent);
		} else {
			G_BounceFootball(ball, &tr);
		}

//		G_FootballImpact(ball, &tr);
	}

	//check if we've left the owner's bbox
	if (!ball->count) {
		trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, ball->r.currentOrigin, ENTITYNUM_NONE, ball->clipmask);
		if (!tr.startsolid || tr.entityNum != ball->r.ownerNum) {

			//but ignore until recatch time passes
			if ((level.ballLastTouchTime < 0) || (level.time - level.ballLastTouchTime) >= BALL_RECATCH_TIME)
				ball->count = 1;
		}
	}

	// check think function after bouncing
//	G_RunThink(ball);
}

void G_BounceFootball(gentity_t *ball, trace_t *trace)
{
	vec3_t	velocity;
	float	dot;
	int		hitTime;

	//reflect the velocity on the trace plane
	hitTime = level.previousTime + (level.time - level.previousTime) * trace->fraction;
	BG_EvaluateTrajectoryDelta(&ball->s.pos, hitTime, velocity);
	dot = DotProduct(velocity, trace->plane.normal);
	VectorMA(velocity, -2*dot, trace->plane.normal, ball->s.pos.trDelta);

	//lose some velocity
	VectorScale(ball->s.pos.trDelta, BALL_BOUNCE_FACTOR, ball->s.pos.trDelta);

	//check for stop
	if ((trace->plane.normal[2] > 0.2) && (VectorLength(ball->s.pos.trDelta) < 80)) {
		G_SetOrigin(ball, trace->endpos);
		return;
	}

	VectorAdd(ball->r.currentOrigin, trace->plane.normal, ball->r.currentOrigin);
	VectorCopy(ball->r.currentOrigin, ball->s.pos.trBase);
	ball->s.pos.trTime = level.time;
}

#include "g_local.h"

#define BALL_RADIUS				8.0f
#define BALL_SHOOT_SPEED		1000

#define BALL_BOUNCE_FACTOR		0.90f
#define BALL_BOUNCE_GOAL		0.50f
#define BALL_AIR_FRICTION		0.995f

#define BALL_IDLE_RESET_TIME	30000
#define BALL_GOAL_RESET_TIME	10000
#define BALL_RECATCH_TIME		500

//how much the ball should turn per unit traveled
#define BALL_ANGLE_SPEED		0.1

#define CONTENTS_GOALPOST		-1/*(CONTENTS_SOLID | CONTENTS_PLAYERCLIP)*/
#define CONTENTS_GOALCLIP		CONTENTS_PLAYERCLIP

const vec3_t goalpost_top_mins = { -100.0f, -100.0f, -100.0f };
const vec3_t goalpost_top_maxs = { +100.0f, +100.0f, +100.0f };

const vec3_t goalpost_back_mins = { 0.0f, 0.0f, 0.0f };
const vec3_t goalpost_back_maxs = { 0.0f, 0.0f, 0.0f };

const vec3_t goalpost_left_mins = { 0.0f, 0.0f, 0.0f };
const vec3_t goalpost_left_maxs = { 0.0f, 0.0f, 0.0f };

const vec3_t goalpost_right_mins = { 0.0f, 0.0f, 0.0f };
const vec3_t goalpost_right_maxs = { 0.0f, 0.0f, 0.0f };

void football_create(vec3_t origin)
{
	gentity_t *ball;

	ball = G_Spawn();
	ball->classname = "football";

	ball->s.eType = ET_FOOTBALL;
	ball->r.svFlags = SVF_BROADCAST;
	ball->clipmask = CONTENTS_SOLID|CONTENTS_BODY;

	//hurtable ball
//	ball->r.contents = CONTENTS_BODY;
//	ball->takedamage = qtrue;

	VectorSet(ball->r.mins, -BALL_RADIUS, -BALL_RADIUS, -BALL_RADIUS);
	VectorSet(ball->r.maxs, +BALL_RADIUS, +BALL_RADIUS, +BALL_RADIUS);

	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;// - 50;		//move a bit on the very first frame
	VectorCopy(origin, ball->s.pos.trBase);
	VectorSet(ball->s.pos.trDelta, 0, 0, 0);

	VectorCopy(origin, ball->r.currentOrigin);

	level.football = ball;
	level.goalTime = 0;
	level.ballLastTouchTime = 0;
}

void goalpost_create(vec3_t origin, int color) //1=red 2=blue
{
	gentity_t *post_back, *post_top, *post_left, *post_right;

	//top
	post_top = G_Spawn();
	post_top->classname = "goalpost_top";

	post_top->s.eType = ET_FOOTBALL_SOLID;
	post_top->s.modelindex = 1;
	post_top->s.generic1 = color;
//	post_top->clipmask = MASK_SHOT;

	VectorCopy(goalpost_top_mins, post_top->r.mins);
	VectorCopy(goalpost_top_maxs, post_top->r.maxs);
	post_top->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_top, origin);
	trap_LinkEntity(post_top);

	//back
	post_back = G_Spawn();
	post_back->classname = "goalpost_back";
	post_back->s.eType = ET_FOOTBALL_SOLID;
	post_back->s.modelindex = 2;
	post_back->s.generic1 = color;
//	post_back->clipmask = MASK_SHOT;

	VectorCopy(goalpost_back_mins, post_back->r.mins);
	VectorCopy(goalpost_back_maxs, post_back->r.maxs);
	post_back->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_back, origin);
	trap_LinkEntity(post_back);

	//left
	post_left = G_Spawn();
	post_left->classname = "goalpost_left";
	post_left->s.eType = ET_FOOTBALL_SOLID;
	post_left->s.modelindex = 3;
	post_left->s.generic1 = color;
//	post_left->clipmask = MASK_SHOT;

	VectorCopy(goalpost_left_mins, post_left->r.mins);
	VectorCopy(goalpost_left_maxs, post_left->r.maxs);
	post_left->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_left, origin);
	trap_LinkEntity(post_left);

	//right
	post_right = G_Spawn();
	post_right->classname = "goalpost_right";
	post_right->s.eType = ET_FOOTBALL_SOLID;
	post_right->s.modelindex = 4;
	post_right->s.generic1 = color;
//	post_right->clipmask = MASK_SHOT;

	VectorCopy(goalpost_right_mins, post_right->r.mins);
	VectorCopy(goalpost_right_maxs, post_right->r.maxs);
	post_right->r.contents = CONTENTS_GOALPOST;

//	post_right->r.contents = CONTENTS_SOLID;
	G_SetOrigin(post_right, origin);
	trap_LinkEntity(post_right);
}

void football_reset(gentity_t *ball)
{
	gentity_t *tmpent;

	if (level.ballCarrier) {
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;
		level.ballCarrier->client->ps.pm_flags &= ~PMF_GOTFOOTBALL;
	}

	level.ballCarrier = NULL;
	level.ballLastTouchTime = 0;
	level.goalTime = 0;

	ball->r.ownerNum = ENTITYNUM_NONE;
	ball->count = 0;

	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	VectorCopy(level.footballSpawnPoint, ball->s.pos.trBase);
	VectorSet(ball->s.pos.trDelta, 0, 0, 0);
	VectorCopy(level.footballSpawnPoint, ball->r.currentOrigin);

	//send event
	tmpent = G_TempEntity(ball->r.currentOrigin, EV_FOOTBALL_PASS);
	tmpent->s.eventParm = 0;
	tmpent->r.svFlags = SVF_BROADCAST;
}

void football_catch(gentity_t *player)
{
	gentity_t *tmpent;

	if (player == level.ballCarrier) //player is already carrying the ball
		return;

	level.ballCarrier = player;
	player->client->ps.powerups[PW_CARRIER] = INT_MAX;
	if (player->client->ps.weapon != WP_GAUNTLET)
		player->client->ps.pm_flags |= PMF_GOTFOOTBALL;

	level.ballLastTouchTime = 0;

	level.football->r.ownerNum = player->s.number;
	level.football->s.pos.trType = TR_LINEAR;

	//send event
	tmpent = G_TempEntity(player->r.currentOrigin, EV_FOOTBALL_PASS);
	if (player->client->sess.sessionTeam == TEAM_RED)
		tmpent->s.eventParm = 1;
	else
		tmpent->s.eventParm = 2;
	tmpent->s.otherEntityNum2 = player->s.number;
	tmpent->r.svFlags = SVF_BROADCAST;

	G_RunFootball(level.football);
}

void football_steal(gentity_t *player)
{
	if (level.ballCarrier) {
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;
		level.ballCarrier->client->ps.pm_flags &= ~PMF_GOTFOOTBALL;
	}

	football_catch(player);
	player->client->ps.pm_flags |= PMF_GOTFOOTBALL; //don't let the stealer shoot it back to the old carrier
}

void football_drop(gentity_t *ball, gentity_t *player, vec3_t extraVelocity)
{
	gentity_t *tmpent;

	if (level.ballCarrier) {
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;
		level.ballCarrier->client->ps.pm_flags &= ~PMF_GOTFOOTBALL;
	}

	level.ballCarrier = NULL;
	level.ballLastTouchTime = level.time;

	ball->count = 0;
	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	if (extraVelocity != NULL)
		VectorAdd(ball->s.pos.trDelta, extraVelocity, ball->s.pos.trDelta);

	//send event
	tmpent = G_TempEntity(player->r.currentOrigin, EV_FOOTBALL_PASS);
	tmpent->s.eventParm = 5;
	tmpent->s.otherEntityNum2 = player->s.number;
	tmpent->r.svFlags = SVF_BROADCAST;
}

void football_shoot(gentity_t *ball, gentity_t *player, vec3_t direction)
{
	gentity_t *tmpent;

	if (level.ballCarrier) {
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;
		level.ballCarrier->client->ps.pm_flags &= ~PMF_GOTFOOTBALL;
	}

	level.ballCarrier = NULL;
	level.ballLastTouchTime = level.time;

	ball->count = 0;
	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	direction[2] += 0.2f; //extra vertical velocity
	VectorNormalize(direction);

	VectorScale(direction, BALL_SHOOT_SPEED, ball->s.pos.trDelta);

	//send event
	tmpent = G_TempEntity(player->r.currentOrigin, EV_FOOTBALL_PASS);
	tmpent->s.eventParm = 4;
	tmpent->s.otherEntityNum2 = player->s.number;
	tmpent->r.svFlags = SVF_BROADCAST;
}

void football_goal(gentity_t *ball, int color) //1=red 2=blue
{
	gentity_t *scorer;
	gentity_t *tmpent;

	level.goalTime = level.time;

	if (ball->r.ownerNum == ENTITYNUM_NONE) //noone scored the goal
		return;

	scorer = &g_entities[ball->r.ownerNum];

	//send event
	tmpent = G_TempEntity(scorer->r.currentOrigin, EV_FOOTBALL_GOAL);
	if (scorer->client->sess.sessionTeam == TEAM_RED) {
		if (color == 2) {
			tmpent->s.eventParm = 1;
			scorer->flags |= FL_FORCE_GESTURE;
		} else {
			tmpent->s.eventParm = 3;
		}
	} else {
		if (color == 1) {
			tmpent->s.eventParm = 2;
			scorer->flags |= FL_FORCE_GESTURE;
		} else {
			tmpent->s.eventParm = 4;
		}
	}
	tmpent->s.otherEntityNum2 = scorer->s.number;
	tmpent->r.svFlags = SVF_BROADCAST;
}

void G_RunFootball(gentity_t *ball)
{
	vec3_t		origin;
	trace_t		tr;
	vec3_t		forward;	//used to calculate ball offset for the carrier
	int			passent;
	gentity_t	*hitent;
	float		xyspeed;

	if (level.goalTime && ((level.time - level.goalTime) >= BALL_GOAL_RESET_TIME))
		football_reset(ball);

	if (level.ballLastTouchTime && ((level.time - level.ballLastTouchTime) >= BALL_IDLE_RESET_TIME))
		football_reset(ball);

	//adjust to player location if someone is carrying the ball
	if (level.ballCarrier) {
		//move the ball to front of the player and to foot level
		forward[0] = 16 * cos(level.ballCarrier->client->ps.viewangles[YAW] * (M_PI*2 / 360));
		forward[1] = 16 * sin(level.ballCarrier->client->ps.viewangles[YAW] * (M_PI*2 / 360));
		forward[2] = -14;

		//calculate new position
		VectorAdd(level.ballCarrier->s.pos.trBase, forward, origin);

		//check for a touch goal
		if (!level.goalTime) {
			trap_Trace(&tr, level.ballCarrier->s.pos.trBase, ball->r.mins, ball->r.maxs, origin, ball->r.ownerNum, ball->clipmask|CONTENTS_GOALCLIP);
			if (tr.fraction < 1.0f) {
				hitent = &g_entities[tr.entityNum];

				if (hitent->s.eType == ET_FOOTBALL_GOAL) {
					football_goal(ball, hitent->s.generic1);
					football_drop(ball, level.ballCarrier, NULL);
				}
			}
		}

		if (level.ballCarrier) { //a touch goal might reset ballCarrier
			//trace towards new position
			trap_Trace(&tr, level.ballCarrier->s.pos.trBase, ball->r.mins, ball->r.maxs, origin, ball->r.ownerNum, ball->clipmask);

			//adjust ball offset so the ball doesn't hit anything
			VectorScale(forward, tr.fraction, forward);

			VectorAdd(level.ballCarrier->s.pos.trBase, forward, ball->s.pos.trBase);
			VectorCopy(level.ballCarrier->s.pos.trDelta, ball->s.pos.trDelta);
			ball->s.pos.trTime = level.ballCarrier->s.pos.trTime;//level.time;
			VectorCopy(ball->s.pos.trBase, ball->r.currentOrigin);

			//adjust ball rotation
			xyspeed = sqrt(ball->s.pos.trDelta[0]*ball->s.pos.trDelta[0] + ball->s.pos.trDelta[1]*ball->s.pos.trDelta[1]);
			if (xyspeed > 0) {
				ball->s.apos.trBase[YAW] = vectoyaw(ball->s.pos.trDelta);
				ball->s.apos.trBase[PITCH] += (xyspeed * BALL_ANGLE_SPEED);

				if (ball->s.apos.trBase[PITCH] > 360.0f)
					ball->s.apos.trBase[PITCH] -= 360.0f;
			}

			return;
		}
	}

	//apply friction
	VectorScale(ball->s.pos.trDelta, BALL_AIR_FRICTION, ball->s.pos.trDelta);

	//get current position
	BG_EvaluateTrajectory(&ball->s.pos, level.time, origin);

	//if the ball has not left the owner's bbox, it shouldn't interact with them
	if (!ball->count) {
		passent = ball->r.ownerNum;
	} else {
		passent = ENTITYNUM_NONE;
	}

	//check for a goal
	if (!level.goalTime) {
		trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, origin, passent, ball->clipmask|CONTENTS_GOALCLIP);
		if (tr.fraction < 1.0f) {
			hitent = &g_entities[tr.entityNum];

			if (hitent->s.eType == ET_FOOTBALL_GOAL) {
				football_goal(ball, hitent->s.generic1);
			}
		}
	}

	//trace a line from the previous position to the current position
	trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, origin, passent, ball->clipmask);

	/*if (tr.startsolid || tr.allsolid) {
		//make sure the tr.entityNum is set to the entity we're stuck in
		trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, ball->r.currentOrigin, passent, ball->clipmask);
		tr.fraction = 0;
	} else*/ {
		VectorCopy(tr.endpos, ball->r.currentOrigin);
	}

	if (tr.fraction != 1) {
		hitent = &g_entities[tr.entityNum];

		if (tr.surfaceFlags & SURF_NOIMPACT) {
			football_reset(ball);
			return;
		}

		if (hitent->client) {
			if (!level.goalTime)
				football_catch(hitent);
		}

		else {
			G_BounceFootball(ball, &tr);
		}
	}

	//check if we've left the owner's bbox
	if (!ball->count) {
		trap_Trace(&tr, ball->r.currentOrigin, ball->r.mins, ball->r.maxs, ball->r.currentOrigin, ENTITYNUM_NONE, ball->clipmask);
		if (!tr.startsolid || tr.entityNum != ball->r.ownerNum) {

			//but ignore until recatch time passes
			if (!level.ballLastTouchTime || ((level.time - level.ballLastTouchTime) >= BALL_RECATCH_TIME))
				ball->count = 1;
		}
	}

	//adjust ball rotation
	xyspeed = sqrt(ball->s.pos.trDelta[0]*ball->s.pos.trDelta[0] + ball->s.pos.trDelta[1]*ball->s.pos.trDelta[1]);
	if (xyspeed > 0) {
		ball->s.apos.trBase[YAW] = vectoyaw(ball->s.pos.trDelta);
		ball->s.apos.trBase[PITCH] += (xyspeed * BALL_ANGLE_SPEED);

		if (ball->s.apos.trBase[PITCH] > 360.0f)
			ball->s.apos.trBase[PITCH] -= 360.0f;
	}
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
	if (level.goalTime) //bounce less after a goal
		VectorScale(ball->s.pos.trDelta, BALL_BOUNCE_GOAL, ball->s.pos.trDelta);
	else
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

void SP_football_redgoal(gentity_t *ent)
{
	ent->s.eType = ET_FOOTBALL_GOAL;
	ent->s.generic1 = 1;

	trap_SetBrushModel(ent, ent->model);
	ent->r.contents = CONTENTS_GOALCLIP;

	trap_LinkEntity(ent);
}

void SP_football_bluegoal(gentity_t *ent)
{
	ent->s.eType = ET_FOOTBALL_GOAL;
	ent->s.generic1 = 2;

	trap_SetBrushModel(ent, ent->model);
	ent->r.contents = CONTENTS_GOALCLIP;

	trap_LinkEntity(ent);
}

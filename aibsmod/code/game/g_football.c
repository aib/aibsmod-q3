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

#define CONTENTS_GOALPOST		(CONTENTS_SOLID | CONTENTS_PLAYERCLIP)
#define CONTENTS_GOALCLIP		CONTENTS_PLAYERCLIP

#define GOAL_SCORE				10
#define OWN_GOAL_SCORE			-10
#define GOAL_ASSIST_SCORE		5

#define MIN(x,y) ((x < y) ? (x) : (y))
#define MAX(x,y) ((x > y) ? (x) : (y))

const vec3_t goalpost_top_mins = { -52.5f, -32.5f, +44.0f };
const vec3_t goalpost_top_maxs = { +52.5f, +30.0f, +46.5f };

const vec3_t goalpost_back_mins = { -52.5f, -32.5f, -16.0f };
const vec3_t goalpost_back_maxs = { +52.5f, -30.0f, +44.0f };

const vec3_t goalpost_left_mins = { -52.5f, -30.0f, -16.0f };
const vec3_t goalpost_left_maxs = { -50.0f, +30.0f, +44.0f };

const vec3_t goalpost_right_mins = { +50.0f, -30.0f, -16.0f };
const vec3_t goalpost_right_maxs = { +52.5f, +30.0f, +44.0f };

//goal area is 1 BALL_RADIUS inside the outer regions
//use the formula below, or just find the smallest magnitude numbers and add/subtract BALL_RADIUS
//could also use anywhere from 0 to ~2 x BALL_RADIUS
const vec3_t goalpost_inside_mins = { -50.0f+BALL_RADIUS, -30.0f+BALL_RADIUS, -16.0f+BALL_RADIUS };
const vec3_t goalpost_inside_maxs = { +50.0f-BALL_RADIUS, +30.0f-BALL_RADIUS, +44.0f-BALL_RADIUS };

//const vec3_t goalpost_inside_mins = { goalpost_left_maxs[0] + BALL_RADIUS, goalpost_back_maxs[1] + BALL_RADIUS, goalpost_back_mins[2] + BALL_RADIUS};
//const vec3_t goalpost_inside_maxs = { goalpost_right_mins[0] - BALL_RADIUS, goalpost_top_maxs[1] - BALL_RADIUS, goalpost_back_maxs[2] - BALL_RADIUS };

void RotateBoundingBox(gentity_t *ent, float yaw, const vec3_t orgmins, const vec3_t orgmaxs);

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
	VectorClear(ball->s.pos.trDelta);

	VectorCopy(origin, ball->r.currentOrigin);

	level.football = ball;
	level.goalTime = 0;

	level.ballCarrier = NULL;
	level.ballShooter = NULL;
	level.ballPasser = NULL;
	level.ballLastTouchTime = 0;
}

void goalpost_create(vec3_t origin, int color)
{
	gentity_t	*post_back, *post_top, *post_left, *post_right, *post_goal;
	float		yaw;

	//top
	post_top = G_Spawn();
	post_top->classname = "goalpost_top";

	post_top->s.eType = ET_FOOTBALL_SOLID;
	post_top->s.modelindex = 1;
	post_top->s.generic1 = color;

	post_top->r.bmodel = qfalse;
	post_top->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_top, origin);
	trap_LinkEntity(post_top);

	//back
	post_back = G_Spawn();
	post_back->classname = "goalpost_back";

	post_back->s.eType = ET_FOOTBALL_SOLID;
	post_back->s.modelindex = 2;
	post_back->s.generic1 = color;

	post_back->r.bmodel = qfalse;
	post_back->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_back, origin);
	trap_LinkEntity(post_back);

	//left
	post_left = G_Spawn();
	post_left->classname = "goalpost_left";

	post_left->s.eType = ET_FOOTBALL_SOLID;
	post_left->s.modelindex = 3;
	post_left->s.generic1 = color;

	post_left->r.bmodel = qfalse;
	post_left->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_left, origin);
	trap_LinkEntity(post_left);

	//right
	post_right = G_Spawn();
	post_right->classname = "goalpost_right";

	post_right->s.eType = ET_FOOTBALL_SOLID;
	post_right->s.modelindex = 4;
	post_right->s.generic1 = color;

	post_right->r.bmodel = qfalse;
	post_right->r.contents = CONTENTS_GOALPOST;

	G_SetOrigin(post_right, origin);
	trap_LinkEntity(post_right);

	//goal area
	post_goal = G_Spawn();
	post_goal->classname = "goalpost_inside";

	post_goal->s.eType = ET_FOOTBALL_GOAL;
	post_goal->s.generic1 = color;

	post_goal->r.bmodel = qfalse;
	post_goal->r.contents = CONTENTS_GOALCLIP; //Note: different

	G_SetOrigin(post_goal, origin);
	trap_LinkEntity(post_goal);

	if (color == TEAM_RED) {
		level.redpost_top = post_top;
		level.redpost_back = post_back;
		level.redpost_left = post_left;
		level.redpost_right = post_right;
		level.redGoalTrigger = post_goal;
		yaw = level.redGoalYaw;
	} else if (color == TEAM_BLUE) {
		level.bluepost_top = post_top;
		level.bluepost_back = post_back;
		level.bluepost_left = post_left;
		level.bluepost_right = post_right;
		level.blueGoalTrigger = post_goal;
		yaw = level.blueGoalYaw;
	}

	goalpost_rotate(yaw, color);
}

void goalpost_rotate(float yaw, int color)
{
	gentity_t	*post_back, *post_top, *post_left, *post_right, *post_goal;
	vec3_t		tangles;

	if (color == TEAM_RED) {
		post_top = level.redpost_top;
		post_back = level.redpost_back;
		post_left = level.redpost_left;
		post_right = level.redpost_right;
		post_goal = level.redGoalTrigger;
	} else if (color == TEAM_BLUE) {
		post_top = level.bluepost_top;
		post_back = level.bluepost_back;
		post_left = level.bluepost_left;
		post_right = level.bluepost_right;
		post_goal = level.blueGoalTrigger;
	}

	//Rotate models
	VectorSet(tangles, 0, yaw, 0);
	G_SetAngles(post_top, tangles);
	G_SetAngles(post_back, tangles);
	G_SetAngles(post_left, tangles);
	G_SetAngles(post_right, tangles);
	G_SetAngles(post_goal, tangles);

	//Rotate bounding boxes
	RotateBoundingBox(post_top, yaw, goalpost_top_mins, goalpost_top_maxs);
	RotateBoundingBox(post_back, yaw, goalpost_back_mins, goalpost_back_maxs);
	RotateBoundingBox(post_left, yaw, goalpost_left_mins, goalpost_left_maxs);
	RotateBoundingBox(post_right, yaw, goalpost_right_mins, goalpost_right_maxs);
	RotateBoundingBox(post_goal, yaw, goalpost_inside_mins, goalpost_inside_maxs);

	trap_LinkEntity(post_top);
	trap_LinkEntity(post_back);
	trap_LinkEntity(post_left);
	trap_LinkEntity(post_right);
	trap_LinkEntity(post_goal);
}

void football_reset(gentity_t *ball)
{
	gentity_t *tmpent;

	if (level.ballCarrier)
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;

	level.ballCarrier = NULL;
	level.ballShooter = NULL;
	level.ballPasser = NULL;
	level.ballLastTouchTime = 0;
	level.goalTime = 0;

	ball->r.ownerNum = ENTITYNUM_NONE;
	ball->count = 0;

	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	VectorCopy(level.footballSpawnPoint, ball->s.pos.trBase);
	VectorClear(ball->s.pos.trDelta);
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

	level.ballPasser = level.ballShooter;
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
	if (level.ballCarrier)
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;

	level.ballShooter = NULL;
	football_catch(player);
	player->client->ps.pm_flags |= PMF_GOTFOOTBALL; //don't let the stealer shoot it back to the old carrier
}

void football_drop(gentity_t *ball, gentity_t *player, vec3_t extraVelocity, gentity_t *cause)
{
	gentity_t *tmpent;

	if (level.ballCarrier)
		level.ballCarrier->client->ps.powerups[PW_CARRIER] = 0;

	level.ballPasser = NULL;
	level.ballShooter = cause;
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
		level.ballCarrier->client->ps.pm_flags |= PMF_GOTFOOTBALL; //set PMF_GOTFOOTBALL so they can't attack immediately

		level.ballShooter = level.ballCarrier;
		level.ballCarrier = NULL;
	}

	level.ballLastTouchTime = level.time;

	ball->count = 0;
	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	direction[2] += 0.2f; //extra vertical velocity
	VectorNormalize(direction);

	if (player && player->client && player->client->ps.powerups[PW_QUAD]) {
		G_AddEvent(player, EV_POWERUP_QUAD, 0);
		VectorScale(direction, g_quadfactor.value * BALL_SHOOT_SPEED, ball->s.pos.trDelta);
	} else {
		VectorScale(direction, BALL_SHOOT_SPEED, ball->s.pos.trDelta);
	}

	//send event
	tmpent = G_TempEntity(player->r.currentOrigin, EV_FOOTBALL_PASS);
	tmpent->s.eventParm = 4;
	tmpent->s.otherEntityNum2 = player->s.number;
	tmpent->r.svFlags = SVF_BROADCAST;
}

void football_goal(gentity_t *ball, int color)
{
	gentity_t	*scorer;
	gentity_t	*tmpent;
	qboolean	ownGoal;

	level.goalTime = level.time;

//	if (ball->r.ownerNum == ENTITYNUM_NONE) //noone scored the goal
//		return;

	//scorer = &g_entities[ball->r.ownerNum];
	scorer = level.ballShooter;

	if (!scorer || !scorer->client) //noone scored the goal
		return;

	//send event
	tmpent = G_TempEntity(scorer->r.currentOrigin, EV_FOOTBALL_GOAL);
	if (scorer->client->sess.sessionTeam == TEAM_RED) {
		if (color == TEAM_RED) {
			tmpent->s.eventParm = 3;
			ownGoal = qtrue;
		} else if (color == TEAM_BLUE) {
			tmpent->s.eventParm = 1;
			ownGoal = qfalse;
		}
	} else {
		if (color == TEAM_BLUE) {
			tmpent->s.eventParm = 4;
			ownGoal = qtrue;
		} else if (color == TEAM_RED) {
			tmpent->s.eventParm = 2;
			ownGoal = qfalse;
		}
	}
	tmpent->s.otherEntityNum2 = scorer->s.number;
	tmpent->r.svFlags = SVF_BROADCAST;

	//award team score
	if (color == TEAM_RED) {
		AddTeamScore(ball->r.currentOrigin, TEAM_BLUE, 1);
		Team_ForceGesture(TEAM_BLUE);
	} else {
		AddTeamScore(ball->r.currentOrigin, TEAM_RED, 1);
		Team_ForceGesture(TEAM_RED);
	}

	//award personal score
	if (ownGoal) {
		AddScore(scorer, ball->r.currentOrigin, OWN_GOAL_SCORE);

		//play humiliation on player, increase gauntlet count
		scorer->client->ps.persistant[PERS_GAUNTLET_FRAG_COUNT]++;

		//add the sprite over the player's head
		scorer->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		scorer->client->ps.eFlags |= EF_AWARD_GAUNTLET;
		scorer->client->rewardTime = level.time + REWARD_SPRITE_TIME;

	} else {
		AddScore(scorer, ball->r.currentOrigin, GOAL_SCORE);
		scorer->client->pers.teamState.captures++;

		//add the sprite over the player's head
		scorer->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_GOAL);
		scorer->client->ps.eFlags |= EF_AWARD_GOAL;
		scorer->client->rewardTime = level.time + REWARD_SPRITE_TIME;
		scorer->client->ps.persistant[PERS_CAPTURES]++;
	}

	//award assist
	if (level.ballPasser && level.ballPasser->client) {
		if (!ownGoal && (scorer != level.ballPasser) && OnSameTeam(scorer, level.ballPasser)) {
			AddScore(level.ballPasser, ball->r.currentOrigin, GOAL_ASSIST_SCORE);

			scorer->client->pers.teamState.assists++;
			level.ballPasser->client->ps.persistant[PERS_ASSIST_COUNT]++;

			//add the sprite over the player's head
			level.ballPasser->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_GOAL);
			level.ballPasser->client->ps.eFlags |= EF_AWARD_ASSIST;
			level.ballPasser->client->rewardTime = level.time + REWARD_SPRITE_TIME;
		}
	}

	//update client team scores
	CalculateRanks();
}

void G_RunFootball(gentity_t *ball)
{
	vec3_t		origin;
	trace_t		tr;
	vec3_t		forward;	//used to calculate ball offset for the carrier
	int			passent;
	gentity_t	*hitent;
	float		xyspeed;

	if (am_piercingRail.integer == 1 && ((rand()&31) == 1))
		G_Printf("Carrier: %s, Shooter: %s, Passer: %s\n",
			(level.ballCarrier ? level.ballCarrier->client->pers.netname : "N/A"),
			(level.ballShooter ? level.ballShooter->client->pers.netname : "N/A"),
			(level.ballPasser ? level.ballPasser->client->pers.netname : "N/A")
		);

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
					level.ballShooter = level.ballCarrier;
					football_goal(ball, hitent->s.generic1);
					football_drop(ball, level.ballCarrier, NULL, NULL);
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
	ent->s.generic1 = TEAM_RED;

	trap_SetBrushModel(ent, ent->model);
	ent->r.contents = CONTENTS_GOALCLIP;

	trap_LinkEntity(ent);
}

void SP_football_bluegoal(gentity_t *ent)
{
	ent->s.eType = ET_FOOTBALL_GOAL;
	ent->s.generic1 = TEAM_BLUE;

	trap_SetBrushModel(ent, ent->model);
	ent->r.contents = CONTENTS_GOALCLIP;

	trap_LinkEntity(ent);
}

void RotateBoundingBox(gentity_t *ent, float yaw, const vec3_t orgmins, const vec3_t orgmaxs)
{
	vec3_t	tmins, tmaxs;

	RotatePointAroundVector(tmins, axisDefault[2], orgmins, yaw);
	RotatePointAroundVector(tmaxs, axisDefault[2], orgmaxs, yaw);

	ent->r.mins[0] = MIN(tmins[0], tmaxs[0]);
	ent->r.maxs[0] = MAX(tmins[0], tmaxs[0]);
	ent->r.mins[1] = MIN(tmins[1], tmaxs[1]);
	ent->r.maxs[1] = MAX(tmins[1], tmaxs[1]);
	ent->r.mins[2] = MIN(tmins[2], tmaxs[2]);
	ent->r.maxs[2] = MAX(tmins[2], tmaxs[2]);
}

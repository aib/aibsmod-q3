#include "g_local.h"

gentity_t *football_create(vec3_t origin)
{
/*	gentity_t *football;

vec3_t hmm;
hmm[0] = hmm[1] = 0;
hmm[2] = 1;

	G_Printf("Using origin %f %f %f\n", origin[0], origin[1], origin[2]);

	football = G_Spawn();
	football->classname = "football";

	football->s.eType = ET_FOOTBALL;
	football->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	football->s.eFlags = EF_BOUNCE_HALF;
//	football->r.contents = CONTENTS_BODY;

	football->clipmask = MASK_SHOT;

	football->s.pos.trType = TR_GRAVITY;
	football->s.pos.trTime = level.time - 50;// - MISSILE_PRESTEP_TIME; //move a bit on the very first frame a la grenade

	VectorCopy(origin, football->s.pos.trBase);

//	VectorScale( dir, 700, bolt->s.pos.trDelta );
	VectorNormalize(hmm);
	VectorScale(hmm, 100, football->s.pos.trDelta);
	SnapVector(football->s.pos.trDelta); //save bandwidth?

	VectorCopy(origin, football->r.currentOrigin);
	trap_LinkEntity(football);

	fire_grenade(NULL, origin, hmm);

	return football;*/
	vec3_t dir;
		gentity_t	*bolt;

	dir[0] = dir[1] = 0;
	dir[2] = 1;

		VectorNormalize (dir);

		bolt = G_Spawn();
		bolt->classname = "football";
//		bolt->nextthink = level.time + 2500;
//		bolt->think = G_ExplodeMissile;
		bolt->s.eType = ET_FOOTBALL;
		bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
		bolt->s.weapon = WP_GRENADE_LAUNCHER;
		bolt->s.eFlags = EF_BOUNCE;
//		bolt->r.ownerNum = self->s.number;
//		bolt->parent = self;
		bolt->damage = 100;
		bolt->splashDamage = 100;
		bolt->splashRadius = 150;
		bolt->methodOfDeath = MOD_GRENADE;
		bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
		bolt->clipmask = MASK_SHOT;
		bolt->target_ent = NULL;

		bolt->s.pos.trType = TR_GRAVITY;
		bolt->s.pos.trTime = level.time;// - 50;		//move a bit on the very first frame
		VectorCopy(origin, bolt->s.pos.trBase);
		VectorScale(dir, 700, bolt->s.pos.trDelta);
		SnapVector(bolt->s.pos.trDelta);			//save net bandwidth

		VectorCopy(origin, bolt->r.currentOrigin);

	return bolt;
}

void football_catch(gentity_t *player)
{
//	if (!player || !player->client) {
//		level.ballCarrier = NULL;
//		level.football->r.ownerNum = ENTITYNUM_NONE;
//		level.football->s.pos.trType = TR_GRAVITY;
//		return;
//	}

	if (player == level.ballCarrier) //player is already carrying the ball
		return;

//	G_Printf("%s (%i) got the ball! (%i / %i)\n", player->client->pers.netname, player->s.number, level.football->r.ownerNum, level.football->count);
	G_Printf("%s got the ball!\n", player->client->pers.netname);

	level.ballCarrier = player;
	level.football->r.ownerNum = player->s.number;
	level.football->count = 0;
	level.football->s.pos.trType = TR_LINEAR;
}

void football_shoot(gentity_t *player, vec3_t direction)
{
	gentity_t *ball;

	ball = level.football;

	level.ballCarrier = NULL;
	ball->count = 0;
	ball->s.pos.trType = TR_GRAVITY;
	ball->s.pos.trTime = level.time;

	direction[2] += 0.2f; //extra vertical velocity
	VectorNormalize(direction);

	VectorScale(direction, 700, ball->s.pos.trDelta);

	G_Printf("%s shot the ball!\n", player->client->pers.netname);
}

void football_reset(void)
{
}

void G_RunFootball(gentity_t *ball)
{
	vec3_t		origin;
	trace_t		tr;
	int			passent;
	gentity_t	*hitent;

vec3_t tmp;
tmp[0] = 1;
tmp[1] = 0;
tmp[2] = 0;
	//adjust to player location if someone is carrying the ball
	if (level.ballCarrier) {
		VectorCopy(level.ballCarrier->s.pos.trBase, ball->s.pos.trBase);
//		G_Printf("%s is carrying the ball at %.0f, %.0f, %.0f (%i / %i)\n", level.ballCarrier->client->pers.netname, ball->s.pos.trBase[0], ball->s.pos.trBase[1], ball->s.pos.trBase[2], ball->r.ownerNum, ball->count);
//		VectorMA(level.ballCarrier->s.pos.trBase, 1, tmp, ball->s.pos.trBase);

		VectorCopy(level.ballCarrier->s.pos.trDelta, ball->s.pos.trDelta);
		ball->s.pos.trTime = level.ballCarrier->s.pos.trTime;//level.time;

		VectorCopy(ball->s.pos.trBase, ball->r.currentOrigin);
		return;
	}

//	G_Printf("noone is carrying the ball at %.0f, %.0f, %.0f (%i / %i)\n", ball->s.pos.trBase[0], ball->s.pos.trBase[1], ball->s.pos.trBase[2],ball->r.ownerNum, ball->count);

	//get current position
	BG_EvaluateTrajectory(&ball->s.pos, level.time, origin);

//	//apply friction
//	VectorScale(ent->s.pos.trDelta, 0.99, ent->s.pos.trDelta);
//	if (VectorLength(ent->s.pos.trDelta) < 60) {
//		ent->s.pos.trDelta[0] = 0;
//		ent->s.pos.trDelta[1] = 0;
//		ent->s.pos.trDelta[2] = 0;
//	}

	//if the ball has not left the owner's bbox, it shouldn't interact with them
	if (!ball->count) {
		passent = ball->r.ownerNum;
	} else {
		passent = ENTITYNUM_NONE;
	}

//	// if this missile bounced off an invulnerability sphere
//	if (ent->target_ent) {
//		passent = ent->target_ent->s.number;
//	}

//	// prox mines that left the owner bbox will attach to anything, even the owner
//	else if (ent->s.weapon == WP_PROX_LAUNCHER && ent->count) {
//		passent = ENTITYNUM_NONE;
//	}

//	else {
//		// ignore interactions with the missile owner
//		passent = ent->r.ownerNum;
//	}

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
			G_FreeEntity(ball);
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
			ball->count = 1;
		}
	}

	// check think function after bouncing
//	G_RunThink(ball);
}

/*void G_FootballImpact(gentity_t *ball, trace_t *trace)
{
	gentity_t		*other;
	qboolean		hitClient = qfalse;
	other = &g_entities[trace->entityNum];

	// check for bounce
//	if ( !other->takedamage && ( ent->s.eFlags & ( EF_BOUNCE | EF_BOUNCE_HALF ) ) ) {
		G_BounceFootball(ball, trace);
//		G_AddEvent(ent, EV_GRENADE_BOUNCE, 0);
//		return;
//	}

/*
	// impact damage
	if (other->takedamage) {
		// FIXME: wrong damage direction?
		if ( ent->damage ) {
			vec3_t	velocity;

			if( LogAccuracyHit( other, &g_entities[ent->r.ownerNum] ) ) {
				g_entities[ent->r.ownerNum].client->accuracy_hits++;
				hitClient = qtrue;
			}
			BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
			if ( VectorLength( velocity ) == 0 ) {
				velocity[2] = 1;	// stepped on a grenade
			}
			G_Damage (other, ent, &g_entities[ent->r.ownerNum], velocity,
				ent->s.origin, ent->damage,
				0, ent->methodOfDeath);
		}
	}

	// is it cheaper in bandwidth to just remove this ent and create a new
	// one, rather than changing the missile into the explosion?

	if ( other->takedamage && other->client ) {
		G_AddEvent( ent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
		ent->s.otherEntityNum = other->s.number;
	} else if( trace->surfaceFlags & SURF_METALSTEPS ) {
		G_AddEvent( ent, EV_MISSILE_MISS_METAL, DirToByte( trace->plane.normal ) );
	} else {
		G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );
	}

	ent->freeAfterEvent = qtrue;

	// change over to a normal entity right at the point of impact
	ent->s.eType = ET_GENERAL;

	SnapVectorTowards( trace->endpos, ent->s.pos.trBase );	// save net bandwidth

	G_SetOrigin( ent, trace->endpos );

	// splash damage (doesn't apply to person directly hit)
	if ( ent->splashDamage ) {
		if( G_RadiusDamage( trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius,
			other, ent->splashMethodOfDeath ) ) {
			if( !hitClient ) {
				g_entities[ent->r.ownerNum].client->accuracy_hits++;
			}
		}
	}

	trap_LinkEntity( ent );
}*/

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
	VectorScale(ball->s.pos.trDelta, 0.65, ball->s.pos.trDelta);

	//check for stop
	if ((trace->plane.normal[2] > 0.2) && (VectorLength(ball->s.pos.trDelta) < 40)) {
		G_SetOrigin(ball, trace->endpos);
		return;
	}

	VectorAdd(ball->r.currentOrigin, trace->plane.normal, ball->r.currentOrigin);
	VectorCopy(ball->r.currentOrigin, ball->s.pos.trBase);
	ball->s.pos.trTime = level.time;
}

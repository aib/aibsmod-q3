#include "g_local.h"

//aibsmod
const vec3_t tripmine_mins = { +0.0f, -13.0f, -7.0f };
const vec3_t tripmine_maxs = { +2.0f, +13.0f, +7.0f };

void Tripmine_Arm(gentity_t *ent)
{
	trace_t tr;
	vec3_t	end;
	vec3_t	nonSolidStart;
	float	nonSolidFraction;

	VectorMA(ent->s.origin, TRIPMINE_RANGE, ent->s.origin2, end);

	VectorMA(ent->s.origin, -1, ent->s.origin2, nonSolidStart);
	trap_Trace(&tr, nonSolidStart, NULL, NULL, end, ent->s.number, TRIPMINE_MASK & ~CONTENTS_SOLID);
	nonSolidFraction = tr.fraction;

	trap_Trace(&tr, ent->s.origin, NULL, NULL, end, ent->s.number, TRIPMINE_MASK);
	ent->laserDistance = TRIPMINE_RANGE * MIN(tr.fraction,nonSolidFraction);

	ent->s.modelindex = 1; //armed

	//Set beam's end
	VectorMA(ent->s.origin, ent->laserDistance, ent->s.origin2, ent->s.angles2);

	ent->think = Tripmine_Think;
	ent->nextthink = level.time + TRIPMINE_THINK_DELAY;
}

void Tripmine_Think(gentity_t *ent)
{
	gentity_t *tent;
	trace_t	tr;
	vec3_t	end;
	float	currentDistance;
	vec3_t	nonSolidStart;
	float	nonSolidFraction;

	VectorMA(ent->s.origin, TRIPMINE_RANGE, ent->s.origin2, end);

	VectorMA(ent->s.origin, -1, ent->s.origin2, nonSolidStart);
	trap_Trace(&tr, nonSolidStart, NULL, NULL, end, ent->s.number, TRIPMINE_MASK & ~CONTENTS_SOLID);
	nonSolidFraction = tr.fraction;

	trap_Trace(&tr, ent->s.origin, NULL, NULL, end, ent->s.number, TRIPMINE_MASK);
	currentDistance = TRIPMINE_RANGE * MIN(tr.fraction,nonSolidFraction);

	//explode if life expires or laser distance changes too much
	if ((level.time > ent->dieTime) || (fabs(currentDistance - ent->laserDistance) >= TRIPMINE_TRIP_DELTA)) {
		G_AddEvent(ent, EV_TRIPMINE, 2); //add exploding event

		ent->takedamage = qfalse;
		ent->think = Tripmine_Explode;
		ent->nextthink = level.time + TRIPMINE_EXPLODE_DELAY;
		return;
	}

	ent->laserDistance = currentDistance;
	VectorMA(ent->s.origin, currentDistance, ent->s.origin2, ent->s.angles2);

	ent->nextthink = level.time + TRIPMINE_THINK_DELAY;
}

void Tripmine_Die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	self->takedamage = qfalse;
	self->think = Tripmine_Explode;
	self->nextthink = level.time + TRIPMINE_EXPLODE_DELAY;
}

void Tripmine_Explode(gentity_t *ent)
{
	G_AddEvent(ent, EV_TRIPMINE, 3);
	ent->s.modelindex = 0;
	ent->s.eType = ET_GENERAL;

	if (ent->splashDamage) {
		if (G_RadiusDamage(ent->s.origin, ent->parent, ent->splashDamage, ent->splashRadius, NULL, ent->splashMethodOfDeath))
			g_entities[ent->r.ownerNum].client->accuracy_hits++;
	}

	ent->freeAfterEvent = qtrue;
	return;
}

qboolean CheckTripmineAttack(gentity_t *ent)
{
	vec3_t		forward, right, up;
	vec3_t		muzzle;

	trace_t		tr;
	vec3_t		end;

	gentity_t	*mine;
	vec3_t		minePos;
	vec3_t		mineAngles;

	AngleVectors(ent->client->ps.viewangles, forward, right, up);

	CalcMuzzlePoint(ent, forward, right, up, muzzle);

	VectorMA(muzzle, TRIPMINE_PLACE_RANGE, forward, end);
	trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, CONTENTS_SOLID);

	if (tr.fraction == 1) //didn't hit a wall
		return qfalse;

	if (tr.surfaceFlags & SURF_NOIMPACT) //hit a noimpact wall
		return qfalse;

	VectorMA(muzzle, TRIPMINE_PLACE_RANGE*tr.fraction, forward, minePos);
	vectoangles(tr.plane.normal, mineAngles);

	if ((tr.plane.normal[0] == 0.0f) && (tr.plane.normal[1] == 0.0f)) //mine directly on floor or ceiling, adjust yaw
		mineAngles[YAW] = ent->client->ps.viewangles[YAW];

	mine = G_Spawn();
	mine->classname = "tripmine";
	mine->s.eType = ET_TRIPMINE;

	mine->s.otherEntityNum = ent->s.number; //owner
	mine->s.generic1 = level.time; //flicker offset

	//Adjust callbacks
	mine->think = Tripmine_Arm;
	mine->nextthink = level.time + TRIPMINE_ARM_TIME;
	mine->dieTime = level.time + TRIPMINE_LIFE;

	//Killable
	mine->health = 50;
	mine->takedamage = qtrue;
	mine->r.contents = CONTENTS_CORPSE;
	mine->die = Tripmine_Die;

	//Adjust damages
	mine->parent = ent;
	mine->splashDamage = 150;
	mine->splashRadius = 360;
	mine->splashMethodOfDeath = MOD_TRIPMINE_SPLASH;

	//Adjust graphics
	mine->s.modelindex = 0; //unarmed

	//Adjust vectors and angles
	VectorCopy(minePos, mine->s.origin);
	VectorCopy(tr.plane.normal, mine->s.origin2);

	G_SetOrigin(mine, minePos);
	G_SetAngles(mine, mineAngles);

	//Adjust bounding box
	G_SetRotatedBoundingBox(mine, tripmine_mins, tripmine_maxs);

	trap_LinkEntity(mine);

	G_AddEvent(mine, EV_TRIPMINE, 1); //add arming event
	return qtrue;
}

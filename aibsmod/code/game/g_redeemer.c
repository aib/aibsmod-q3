#include "g_local.h"

const static vec3_t redeemerMissile_mins = { -1.0f, -1.0f, -1.0f };
const static vec3_t redeemerMissile_maxs = { +1.0f, +1.0f, +1.0f };

void Redeemer_Fire(gentity_t *ent)
{
	vec3_t		forward, right, up;
	vec3_t		muzzle;
	gentity_t	*clone;

	AngleVectors(ent->client->ps.viewangles, forward, right, up);
	CalcMuzzlePoint(ent, forward, right, up, muzzle);

	clone = ClonePlayer(ent);
	clone->r.ownerNum = ent->s.number;

	ent->client->noclip = 1;
	ent->redeeming = 1;
	ent->redeemClone = clone;

	ent->client->ps.pm_flags |= PMF_FIREDREDEEMER;
	ent->client->ps.eFlags |= EF_DIFFERENT_MODEL;
	ent->client->ps.generic1 = 1;
}

//Called after client moves
void Redeemer_Check(gentity_t *ent)
{
	trace_t tr;

	trap_Trace(&tr, ent->client->oldOrigin, redeemerMissile_mins, redeemerMissile_maxs, ent->client->ps.origin, ent->s.number, REDEEMER_MASK);

	if (tr.fraction == 1)
		return;

	Redeemer_Explode(ent);
}

void Redeemer_Explode(gentity_t *ent)
{
	G_Printf("BOOM!\n");

	ReturnToClone(ent, ent->redeemClone);

	ent->client->noclip = 0;
	ent->redeeming = 0;
	ent->redeemClone = NULL;
	ent->client->ps.eFlags &= ~EF_DIFFERENT_MODEL;
}

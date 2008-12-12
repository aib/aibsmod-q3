#include "g_local.h"

void ra_register_hit(gentity_t *attacker, gentity_t *inflictor, gentity_t *target)
{
	if (!Q_stricmp(inflictor->classname, "rocket")) {
		G_Printf("%s hit %s with a rocket, flight time: %0.1f\n",
			attacker->client->pers.netname,
			target->client->pers.netname,
			((level.time - inflictor->spawnTime) / 1000.0f)
		);
		G_Printf("Target velocity at the time: %s %s\n", vtos(target->client->ps.origin), vtos(target->client->ps.velocity));
	} else {
		G_Printf("%s hit %s with %s.\n", attacker->client->pers.netname, target->client->pers.netname, inflictor->classname);
	}

}

#include "cg_local.h"

#define MAX_SPEEDCALC_FRAMES 256
float getPlayerSpeed(void)
{
	static float previousSpeeds[MAX_SPEEDCALC_FRAMES];
	static float previousHeights[MAX_SPEEDCALC_FRAMES];
	static int index;
	int totalFrames;
	float total;
	float maxval; //max. height or speed, depending on method
	int i;

	previousSpeeds[index % MAX_SPEEDCALC_FRAMES] = cg.xyspeed;
	previousHeights[index % MAX_SPEEDCALC_FRAMES] = cg.zpos;
	index = (index + 1) % MAX_SPEEDCALC_FRAMES;

	totalFrames = am_drawSpeedFrames.integer;

	if (totalFrames <= 0)
		totalFrames = 1;
	else if (totalFrames >= MAX_SPEEDCALC_FRAMES)
		totalFrames = MAX_SPEEDCALC_FRAMES-1;

	if (am_drawSpeedMethod.integer == 2) { //speed at max. height
		total = 0.0f;
		maxval = -1000000.0f;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			if (previousHeights[i] > maxval) {
				total = previousSpeeds[i];
				maxval = previousHeights[i];
			}
		}
	}

	else if (am_drawSpeedMethod.integer == 1) { //speed at max. speed
		total = 0.0f;
		maxval = -1.0f;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			if (previousSpeeds[i] > maxval) {
				total = previousSpeeds[i];
				maxval = previousSpeeds[i];
			}
		}
	}

	else { //average speed
		maxval = 0.0f;
		for (i=(index + MAX_SPEEDCALC_FRAMES - totalFrames)%MAX_SPEEDCALC_FRAMES; i!=index; i=(i+1)%MAX_SPEEDCALC_FRAMES) {
			total += previousSpeeds[i];
			maxval += 1.0f;
		}
		total /= maxval;
	}

	return total;
}

float CG_DrawBigSpeed(float y)
{
	char		*s;
	int			w;

	s = va("%.0fups", getPlayerSpeed());
	w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;

	CG_DrawBigString(635 - w, y + 2, s, 1.0f);

	return y + BIGCHAR_HEIGHT + 4;
}

void CG_DrawSmallSpeed(void)
{
	static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	char *s;
	int w;

	s = va("%.0f", getPlayerSpeed());

	w = CG_DrawStrlen(s) * TINYCHAR_WIDTH;

	CG_DrawStringExt(320 - (w/2), 304, s, color, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);
}

void CG_DrawButtons(void)
{
	if (cg.buttonState & BTNFLAG_BUTTON_UP)
		CG_DrawPic(304, 320+1, 32, 32, cgs.media.button_up_downShader);
	else
		CG_DrawPic(304, 320+1, 32, 32, cgs.media.button_up_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_LEFT)
		CG_DrawPic(272+1, 352, 32, 32, cgs.media.button_left_downShader);
	else
		CG_DrawPic(272+1, 352, 32, 32, cgs.media.button_left_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_RIGHT)
		CG_DrawPic(336-1, 352, 32, 32, cgs.media.button_right_downShader);
	else
		CG_DrawPic(336-1, 352, 32, 32, cgs.media.button_right_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_DOWN)
		CG_DrawPic(304, 352, 32, 32, cgs.media.button_down_downShader);
	else
		CG_DrawPic(304, 352, 32, 32, cgs.media.button_down_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_JUMP)
		CG_DrawPic(240, 400, 160, 32, cgs.media.button_jump_downShader);
	else
		CG_DrawPic(240, 400, 160, 32, cgs.media.button_jump_upShader);

	if (cg.buttonState & BTNFLAG_BUTTON_FIRE)
		CG_DrawPic(240, 320, 32, 32, cgs.media.button_fire_downShader);
	else
		CG_DrawPic(240, 320, 32, 32, cgs.media.button_fire_upShader);
}

void CG_AibsmodEntities()
{
//	if (cgs.gametype == GT_FOOTBALL && am_drawFootballTracer.integer)
//		CG_DrawFootballTracer();
}

//Draw a laser beam
void CG_Laser(const vec3_t start, const vec3_t end, const vec3_t color)
{
	refEntity_t ent;

	memset(&ent, 0, sizeof(ent));

	ent.reType = RT_RAIL_CORE;
	ent.customShader = cgs.media.railCoreShader;

	VectorCopy(start, ent.origin);
	VectorCopy(end, ent.oldorigin);

	ent.shaderRGBA[0] = color[0] * 255;
	ent.shaderRGBA[1] = color[1] * 255;
	ent.shaderRGBA[2] = color[2] * 255;
	ent.shaderRGBA[3] = 255;

	trap_R_AddRefEntityToScene(&ent);
}

//draw the football
void CG_Football(centity_t *cent)
{
	refEntity_t ent;

	memset(&ent, 0, sizeof(ent));

	//set frame
	ent.frame = cent->currentState.frame;
	ent.oldframe = ent.frame;
	ent.backlerp = 0;

	VectorCopy(cent->lerpOrigin, ent.origin);
	VectorCopy(cent->lerpOrigin, ent.oldorigin);

	//copy position to global variable
	VectorCopy(cent->lerpOrigin, cg.footballPos);

	ent.hModel = cgs.media.footballModel;

	//convert angles to axis
	AnglesToAxis(cent->lerpAngles, ent.axis);

	//add to refresh list
	trap_R_AddRefEntityToScene(&ent);
}

//draw goal posts
void CG_FootballGoalpost(centity_t *cent)
{
	refEntity_t ent;

	memset(&ent, 0, sizeof(ent));

	//set frame
	ent.frame = cent->currentState.frame;
	ent.oldframe = ent.frame;
	ent.backlerp = 0;

	VectorCopy(cent->lerpOrigin, ent.origin);
	VectorCopy(cent->lerpOrigin, ent.oldorigin);

	switch (cent->currentState.modelindex) {
		case 11: //red top
			ent.hModel = cgs.media.goalpostRedTopModel;
			break;

		case 12: //red back
			ent.hModel = cgs.media.goalpostRedBackModel;
			break;

		case 13: //red left
			ent.hModel = cgs.media.goalpostRedLeftModel;
			break;

		case 14: //red right
			ent.hModel = cgs.media.goalpostRedRightModel;
			break;

		case 21: //blue top
			ent.hModel = cgs.media.goalpostBlueTopModel;
			break;

		case 22: //blue back
			ent.hModel = cgs.media.goalpostBlueBackModel;
			break;

		case 23: //blue left
			ent.hModel = cgs.media.goalpostBlueLeftModel;
			break;

		case 24: //blue right
			ent.hModel = cgs.media.goalpostBlueRightModel;
			break;

		default:
			return;
	}

	//convert angles to axis
	AnglesToAxis(cent->lerpAngles, ent.axis);

	//add to refresh list
	trap_R_AddRefEntityToScene(&ent);
}

//draw a laser tripmine
void CG_Tripmine(centity_t *cent)
{
	refEntity_t		mineBody;
	const vec3_t	laserColor = { 1.0f, 0.0f, 0.0f };

	memset(&mineBody, 0, sizeof(mineBody));

	//set frame
	mineBody.frame = cent->currentState.frame;
	mineBody.oldframe = mineBody.frame;
	mineBody.backlerp = 0;

	VectorCopy(cent->lerpOrigin, mineBody.origin);
	VectorCopy(cent->lerpOrigin, mineBody.oldorigin);

	mineBody.hModel = cgs.media.tripmineModel;

	//convert angles to axis
	AnglesToAxis(cent->lerpAngles, mineBody.axis);

	//add to refresh list
	trap_R_AddRefEntityToScene(&mineBody);

	//Draw laser if armed
	if (cent->currentState.modelindex == 1) { //armed
		CG_Laser(cent->currentState.origin, cent->currentState.angles2, laserColor);
	}
}

//draw a bounding box (mins=origin2 and maxs=angles2)
void CG_BBox(centity_t *cent)
{
	clientInfo_t	ci;
	vec3_t			mins, maxs;
	vec3_t			p1, p2;
	const vec3_t	color = { 0.5f, 0.0f, 1.0f };

	VectorAdd(cent->currentState.origin, cent->currentState.origin2, mins);
	VectorAdd(cent->currentState.origin, cent->currentState.angles2, maxs);

	VectorSet(ci.color1, 0.5f, 0, 1.0f);
	VectorSet(ci.color2, 0.5f, 0, 1.0f);

	VectorSet(p1, mins[0], mins[1], mins[2]);
	VectorSet(p2, mins[0], maxs[1], mins[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], maxs[1], mins[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p2, maxs[0], mins[1], mins[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, mins[0], mins[1], mins[2]);
	CG_Laser(p1, p2, color);

	VectorSet(p1, mins[0], mins[1], maxs[2]);
	VectorSet(p2, mins[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p2, maxs[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, mins[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);

	VectorSet(p1, mins[0], mins[1], mins[2]);
	VectorSet(p2, mins[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, mins[0], maxs[1], mins[2]);
	VectorSet(p2, mins[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], mins[1], mins[2]);
	VectorSet(p2, maxs[0], mins[1], maxs[2]);
	CG_Laser(p1, p2, color);
	VectorSet(p1, maxs[0], maxs[1], mins[2]);
	VectorSet(p2, maxs[0], maxs[1], maxs[2]);
	CG_Laser(p1, p2, color);
}

//Draw a tracer line from player to football
void CG_DrawFootballTracer(void)
{
	vec3_t tracerColor;

	if (cg.carrierNum == -1) //noone is carrying the ball
		VectorSet(tracerColor, 8.0f, 8.0f, 8.0f);
	else if (cgs.clientinfo[cg.carrierNum].team == TEAM_RED)
		VectorSet(tracerColor, 1.0f, 0.15f, 0.15f);
	else if (cgs.clientinfo[cg.carrierNum].team == TEAM_BLUE)
		VectorSet(tracerColor, 0.15f, 0.15f, 1.0f);
	else
		VectorSet(tracerColor, 0.15f, 1.0f, 0.15f);

	CG_Laser(cg.predictedPlayerState.origin, cg.footballPos, tracerColor);
}

void CG_DropWeaponChange(void)
{
	int i;
	int original;

	original = cg.weaponSelect;
	cg.weaponSelectTime = cg.time;

	for (i=15; i>0; i--) {
		if (i == original)
			continue;

		if (CG_WeaponSelectable(i)) {
			cg.weaponSelect = i;
			break;
		}
	}
}

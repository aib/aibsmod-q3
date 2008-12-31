powerups/rambo
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/quadmap2_green.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

powerups/ramboWeapon
{
	deformVertexes wave 100 sin 0.5 0 0 0
	{
		map textures/effects/quadmap2_green.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

textures/weapons/tripmine
{
	{
		map $whiteimage
		rgbGen entity
		blendFunc GL_ONE GL_ZERO
	}
	{
		map textures/weapons/tripmine.tga
		rgbGen identity
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identityLighting
	}
}

tripmineExplosion
{
	cull disable
	{
		animmap 8 models/weaphits/rlboom/rlboom_1.tga  models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga
		rgbGen wave inversesawtooth 0 1 0 8
		blendfunc add
	}
	{
		animmap 8 models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga gfx/colors/black.tga
		rgbGen wave sawtooth 0 1 0 8
		blendfunc add
	}
}

medals/airrocket
{
	nopicmip
	{
		clampmap gfx/icons/medal_airrocket.tga
		blendFunc blend
	}
}

medals/airgrenade
{
	nopicmip
	{
		clampmap gfx/icons/medal_airgrenade.tga
		blendFunc blend
	}
}

medals/aircombo
{
	nopicmip
	{
		clampmap gfx/icons/medal_aircombo.tga
		blendFunc blend
	}
}

show_buttons/up_up
{
	nopicmip
	{
		clampmap gfx/buttons/btn_up_up.tga
		blendFunc blend
	}
}

show_buttons/up_down
{
	nopicmip
	{
		clampmap gfx/buttons/btn_up_down.tga
		blendFunc blend
	}
}

show_buttons/down_up
{
	nopicmip
	{
		clampmap gfx/buttons/btn_down_up.tga
		blendFunc blend
	}
}

show_buttons/down_down
{
	nopicmip
	{
		clampmap gfx/buttons/btn_down_down.tga
		blendFunc blend
	}
}

show_buttons/left_up
{
	nopicmip
	{
		clampmap gfx/buttons/btn_left_up.tga
		blendFunc blend
	}
}

show_buttons/left_down
{
	nopicmip
	{
		clampmap gfx/buttons/btn_left_down.tga
		blendFunc blend
	}
}

show_buttons/right_up
{
	nopicmip
	{
		clampmap gfx/buttons/btn_right_up.tga
		blendFunc blend
	}
}

show_buttons/right_down
{
	nopicmip
	{
		clampmap gfx/buttons/btn_right_down.tga
		blendFunc blend
	}
}

show_buttons/jump_up
{
	nopicmip
	{
		clampmap gfx/buttons/btn_jump_up.tga
		blendFunc blend
	}
}

show_buttons/jump_down
{
	nopicmip
	{
		clampmap gfx/buttons/btn_jump_down.tga
		blendFunc blend
	}
}

show_buttons/fire_up
{
	nopicmip
	{
		clampmap gfx/buttons/btn_fire_up.tga
		blendFunc blend
	}
}

show_buttons/fire_down
{
	nopicmip
	{
		clampmap gfx/buttons/btn_fire_down.tga
		blendFunc blend
	}
}

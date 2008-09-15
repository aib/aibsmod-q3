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

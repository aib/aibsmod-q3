*** aibsmod Documentation ***

v0.94 Notes:
	Bikac yeni bisey ekledim. Bir de asagidaki notlara bakin:

v0.92 Notes:
	Modelleri duzelttim. cg_forceModel, model, headmodel, team_model, team_headmodel, enemy_model ve enemy_headmodel'in hepsinin calismasi lazim. Tek sey, VQ3 modelleri kullaniyorsaniz, team game'lerde modellerin skinleri red/blue olarak replace ediliyor. (Mesela DM'de enemy_model'i bones/blue, headmodel'i bones/red yapip ucubik bir yaratikla karsilasmak mumkunken, TDM'de kafasi da vucudu da team rengini aliyor.)

	forceModel'i acip kapamaya gerek yok, yukaridaki variable'lardan herhangi biri degistigi anda modellerin otomatik reload edilmesi lazim. CPMA modellerini acip kapadiktan sonra vid_restart hala sart, malesef.

	Su yukarda bahsini ettigim durum disinda her seyin calismasi lazim, lutfen calismayan seyleri bana bildirin.

v0.91 Notes:
	Alt tab seyini, rocket arena'da havada vuranin gozukmesini duzelttim. 10 tane kadar yeni renk ekledim. aibsmod_test9 altina atilacak pak1.pk3 seklinde bir update bu. Yoksa serverdan otomatik olarak almasi lazim. Almazsa, modu tekrar indirebilirsiniz.

v0.90 Notes:
	#sourgaming:
	Moda baya yeni bisiy ekledim. Bi tane yeni gametype'imiz var, mevcut gametype'lari \callvote g_gametype yazarak gorebilirsiniz. CPMA skin ve renk sistemi, dusman ve takim modelleri ayarlama var. Bu dosyanin Credits kismini da degistirdim, sanirim formati bu sekilde olacak. Atladigim bir suru birsey var; eger moda katkida bulunduysaniz, isminizin gecmesini istediginiz seklini (Orhan "aib" Kavrakoglu gibi) varsa atladigim seylerle birlikte bana IRC'den yollayiniz. Modu test edenlerden atladigim biri varsa (1-2 kereden fazla vq3 oynayanlari ariyorum, su anda sadece emin olduklarimi listeledim) nicklerini verirseniz, veya daha onemlisi bana gorunmek istedikleri isim/nickleri mesaj atmalarini soylerseniz, super olur.

	Deli gibi Midair Arena oynamayi planliyorum.
	--aib

Installation:

	Extract the archive file (probably the one you found this file in) to your Quake 3 directory. The archive contains a directory itself, so you should have:

	* X:\Quake3\aibsmod_test9\readme.txt
	* X:\Quake3\aibsmod_test9\pak0.pk3

	Where X:\Quake3 is your Quake 3 directory.


Variables:
Server Variables/Settings:

	* am_fastWeaponSwitch <0/1>
		This setting controls whether weapon switching occurs instantly (1; CPMA/3wave style) or with a delay (0; VQ3). Players can "callvote fastswitch 0/1" to change this setting.

	* am_trainingMode <0/1/2>
		This setting determines whether "training mode" is active or not. If active (1 or 2), players will be invulnerable. They will be able to walk through eachother and will not telefrag one another. Weapons will cause knockback but no damage. In addition, if this setting is 1, players will be given all weapons and infinite ammo. Players can "callvote training 0/1/2" to change this setting.

	* am_airControl <n>
		This setting is the acceleration constant for air movement. The default is 1.0, same as the original Quake setting. Setting it higher gives people more air control a la CPMA, but with more sensible movements. (For example, if pressing "right strafe" while going left stops you, pressing "right strafe" again for the same amount of time makes you move in the opposite direction. CPMA allows you to instantly change your direction using the "forward" key only; VQ3 (and aibsmod) allows you to *accelerate* towards any direction, making direction change more gradual. Setting this value higher makes it less gradual.)

	* am_selfDamage <0/1>
		This setting controls whether players can damage themselves. If set to 0, players cannot damage themselves (neither health nor armorwise) but knockback still applies. Players can "callvote selfdamage 0/1" to change this setting.

	* am_fallDamage <0/1>
		This setting controls whether players take fall damage. Players can "callvote falldamage 0/1" to change this setting.

	* am_damageKick <0/1>
		This setting controls whether players' screens kick when they take damage. Disabling it also disables the blotch of blood that shows up on the screen.

	* am_disableWeapons <0/1>
		This setting controls whether players can use weapons other than the gauntlet. It is mainly useful in football and telefrag matches (see the "teleport" command below). Players can "callvote disableweapons 0/1" to change this setting.

	* am_piercingRail <0/1>
		This setting controls whether railgun shots go through walls. Players can "callvote piercing 0/1" to change this setting.

	* am_hyperGauntlet <0/1>
		This server setting controls whether the gauntlet does 50 (normal) or 1000 (instant-kill) damage. Players can "callvote hypergauntlet 0/1" to change this setting.

	* am_rocketBounce <n>
		This setting controls whether rockets can bounce off of walls. 0 is the default and doesn't allow rockets to bounce off walls. Changing this setting to any number will allow rockets to bounce that many times. For example, if am_rocketBounce is 2, rockets will bounce off of the first two walls they encounter, and then explode on the third. If set to a negative number, rockets that have bounced off at least once will be able to hit their owners. Players can "callvote rocketbounce n" to change this setting.

	* am_tripmineGrenades <0|1>
		This setting controls whether grenades are replaced by laser tripmines. See the section entitled "Laser Tripmines" below for details. Players call "callvote tripmines 0/1" to change this setting.

	* am_nonRamboKill <0/1/2>
		This setting controls whether non-rambo players can be damaged by other non-rambo players. If it is "0", non-rambo players cannot be damaged by other non-rambo players. A value of "1" will allow such damage, but will not reward (nor punish) such kills in [non-team] Rambomatch mode. A value of "2" will punish non-rambo killers by subtracting a point from their score in [non-team] Rambomatch mode. In Team Rambomatch mode, settings 1 and 2 do not differ and always reward opposing team non-rambo kills with +1 and punish team kills with -1. Players can "callvote nonrambokill 0/1/2" to change this setting.

	* am_redGoalRotation <0-360> and am_blueGoalRotation <0-360>
		These settings rotate the red and blue goals respectively, if the map isn't using custom ones. They are necessary on maps where the red/blue flag orientation isn't correct, i.e. the goals that have replaced the flags in Football mode are facing the wrong way. They can also be used to make scoring a goal more difficult. Their values are in degrees clockwise from the original orientation. (e.g. "am_redGoalRotation 90" means "turn the red goal 90 degrees clockwise")

	* am_midair_groundLaunch <0/n>
		This setting was introduced in order to discourage "ground camping" in the Midair Arena mode. Since ground hits do not count in this mode, it is an efficient (but an annoying and rather meaningless) strategy to stay on the ground and shoot players who dare take into the air. While the method employed by this setting isn't a very efficient deterrer, the idea came up many times (by myself and others alike) while I was developing the mod, and I thought to include it. If this setting is nonzero, any player taking rocket damage while on the ground will be shot into the air with vertical velocity 'n' as set. They will also be knocked sideways in a random direction with speed 0.25n ~ 0.50n. About 750 seems to be a good value, being the velocity of a good rocket jump, and will at least give "honorable" players a chance against ground campers. A small value (such as 1) effectively eliminates ground knockback. I highly suggest the default value of 0 (disabled, VQ3-identical physics), but if you need it, it's there. Players can "callvote midair_glaunch n" to change this setting.

	* am_dropTeamPowerups <0/1>
		This setting controls whether players drop their current powerups (e.g. quad) when they die in team games, as they do in FFA games.

	* am_droppableWeapons <0/1>
		This setting controls whether the "drop weapon" client command is enabled.

	* am_teleportDelay <n>
		This setting controls whether the "teleport" command (see the "Client Commands" section below) is enabled, and the delay between teleports. If it is a negative number, the command is disabled. Otherwise, it is the delay in milliseconds between successive uses of the "teleport" command. Players can "callvote teleportdelay n" to change this setting.

	* am_spawnHealth <n>
		If set (greater than 0), players will start with this amount of health. This variable can be adjusted by maps; see the Mapmaking section below for details.

	* am_spawnNoMG <0/1>
		If set, players don't start with a Machine Gun by default. This variable can be adjusted by maps; see the Mapmaking section below for details.


Client Variables/Settings:

	* am_drawKillNotice <0/1/2>
		This setting controls where the [occasionally annoying] "You fragged x/1st place with n kills" message appears. "1" is the default setting and doesn't change anything. Setting it to "0" will completely disable the message (you can still read your own kills from the "chat log" in the upper left corner) and setting it to "2" will make the message appear in the top center side of your screen, as opposed to the dead middle. Give it a try.

	* am_drawSpeed <0/1/2>
		This setting draws the infamous "player speed meter" (if not "0".) "1" draws it in the upper right corner, just below the FPS meter; "2" draws it in the middle of the screen, a bit below the crosshair.

	* am_drawSpeedMethod	<0/1/2>
	* am_drawSpeedFrames	<1-255>
	* am_drawSpeedFull	<0/1>
		See the section entitled "The Enhanced Speed Meter" below.

	* am_drawButtons <0/1>
		This setting controls whether an overlay indicating the buttons pressed by the current player (or spectatee) will be drawn.

	* am_drawFootballTracer <0/1>
		If set to 1, a line will be drawn from the current player's position towards the football. It will have the possessor's team color (or white.) This setting is currently disabled.

	* am_hitFeedback <0/1/2>
		Changes the feedback sound you hear when you damage a player. 0 is the default value that uses the original Quake 3 Arena behavior - all hits make the same sound. (Unfortunately, the Team Arena sound effects that notify you of the target's armor level, is incompatible with aibsmod.) 1 employs CPMA-style sound effects with different levels at 1-25, 26-50, 51-75 and 76+ damage. 2 behaves like Threewave with different sounds at at 1-19, 20-49, 50-99 and 100+ damage.

	* am_chatBeep <0/1/2>
		Enables or disables the "chat beep" you hear when somebody sends a message. 0 disables all beeps, 1 enables all, 2 enables beeps for team chat only.

	* am_weaponBob <0/1>
		Enables or disables the bobbing of your weapon as you move around.

	* am_CPMASkins      <0/1>
	* am_colors         "#####"
	* am_enemyColors    "#####"
	* am_friendlyColors "#####"
		See the section entitled "CPMA Skins and Color Management" below.

	* team_model		<model name>
	* team_headmodel	<model name>
	* enemy_model		<model name>
	* enemy_headmodel	<model name>
		While the former two of these variables existed in the original client, they did nothing. aibsmod added the latter two and gave a purpose to all four. These variables allow you to adjust the torso/legs model and the head model of your teammates and your enemies, if cg_forceModels is set to nonzero. Note that the 'teammate' "team_model" and "team_headmodel" settings do not affect the way you see yourself, your own model is always drawn using the "model" and "headmodel" settings. Because of the way the original code handles model settings, re-setting of the cg_forceModel variable and/or a vid_restart command may be in order after changing any of these.

	* am_demoFastForwardSpeed <n>
		Sets the fast-forward speed (timescale) for the demo fast-forward commands. Also see the "Demo Commands" section below.


Client Commands:

	* drop weapon
		This command drops your current weapon and all its ammo, presumably for a teammate to pick up. You cannot pick up a weapon you have dropped for 5 seconds. am_droppableWeapons needs to be 1 in order for this command to work.

	* teleport
		This command teleports you immediately to the point your crosshair is on, with a maximum distance of 8192 game units. (Same as the railgun's range.) It will try to avoid teleporting you into walls, but may place you slightly off-target in doing so. It will telefrag other players. am_teleportDelay needs to be positive in order for this command to work, and it will only work once every am_teleportDelay milliseconds.


Demo Commands:

	* demoff_time [mm:]ss
		This command fast-forwards your demo to the specified time, as reported by the client timer (cg_drawTimer 1). The minute part is optional; you can specify a time as either "70" (one minute and 10 seconds) or "1:10" (same.)

	* demoff_timefromnow [mm:]ss
		This command fast-forwards your demo 'ss' seconds (or 'mm' minutes and 'ss' seconds) into the future.


Laser Tripmines:

	aibsmod has laser tripmines that you may remember from a few other games. They are placed on solid surfaces and automatically activate after a few seconds. Once activated, they shoot a laser beam straight across and explode if anything crosses it.

	They are rather small and hard to hit, but have 50 health. They do 150 splash damage with a radius of 360 units. (That is thrice the rocket splash radius, but the damage scales down with the distance.) If two or more are placed close by, they will cause eachother to explode, creating chain reactions.

	They work by measuring the distance to the first solid (or laser length, if you will) once every server frame. If that distance changes across two frames, they explode with a slight delay. Due to this fact and the limitations of the Quake 3 engine, it is possible to cross the beam at high velocities without causing the mine to explode. Very slight changes in laser length do not disturb the mine, meaning, you can move along the path of the beam if you move slowly enough. This is by design.


The Enhanced Scoreboard:

	aibsmod has an enhanced scoreboard: To the left of players' names, their Excellent, Impressive and Gauntlet (Humiliation) counts are listed in their respective colors (Excellent=yellow, Impressive=cyan, Gauntlet=red). Furthermore, the Rambo in Rambomatch and Team Rambomatch games, and the ball carrier in Football games will be highlighted green.


The Enhanced Speed Meter:

	aibsmod has an "enhanced" speed meter built-in. It has different methods of calculating the player's horizontal (x/y) or full (x/y/z) speeds over varying amounts of time. am_drawSpeedFull controls whether the full speed (1) or just the horizontal speed (0) of the player is used. The mod records the client speed over am_drawSpeedFrames frames, adjustable from 1 to 255. What it does with this information is up to am_drawSpeedMethod:

	Method 0 is a simple average. Speeds recorded on the previous N frames are added up and then the result is divided by N, giving the average speed.

	Method 1 is the maximum speed attained over N frames. It simply gives the highest speed among the previous N frames.

	Method 2 is the speed attained at the highest client position among the previous N frames. In this method, the mod records the player's z-coordinate (up/down axis) value in addition to the speed over the last N frames, and gives the speed at the highest point among these N frames. Note that this value will be lower than the maximum speed attained as the player gains horizontal speed while falling down; this is precisely the information discarded by this method.

	Method 3 is the minimum speed attained over N frames. It's the counterpart to Method 1 above.

	"N" in the descriptions above stands for the current value of am_drawSpeedFrames, clipped to 1 or 255 if outside that range.

	Thus, am_drawSpeedMethod 0 and am_drawSpeedFrames 1 will give you your current speed; am_drawSpeedMethod 1 and am_drawSpeedFrames 10 will give you your maximum speed in the last 10 frames; am_drawSpeedMethod 2 and am_drawSpeedMethod 100 will give you your speed at the time you were at your highest vertical position in the last 100 frames.


CPMA Skins and Color Management:

	aibsmod makes use of CPMA skins, brightly-colored and customizable skins borrowed from CPMA and Threewave that reside in CPMAskins.pk3. They can have up to three different colors for their head, torso and leg parts. The am_CPMASkins variable controls whether the client uses original Q3 skins or these custom CPMA ones. The following apply only to clients with am_CPMASkins set to 1, as a value of 0 (default) will fall back to using the original skins.

	Color variables are made up of five digits that each correspond to a colorable part:
	- Head
	- Torso
	- Legs
	- Primary Weapon Effects (color1, e.g. rail core color)
	- Secondary Weapon Effects (color2, e.g. rail spiral color)

	* am_colors "abcde"
		This variable represents your own preference for your colors. The example above will set your head color to 'a', torso color to 'b', legs color to 'c', primary weapon effects color to 'd' and secondary weapon effects color to 'e'. A '-' or any other invalid value corresponds to "no choice" or "default value", head/torso/leg colors white, red or blue depending on your team, primary weapon effects the value of the "color1" variable, and secondary effects the value of "color2". See the color chart at the end of this section for possible values.

		For example, an am_colors value of "2-15-":
			Will set your head color to green (2).
			Will set your torso color to white in non-team and either red or blue in team games.
			Will set your pants' color to blue (1).
			Will set your primary effects/rail core color to magenta (5).
			Will set your secondary effects/rail spiral color to the value of your "color2" variable.

	* am_enemyColors "abcde"
		This variable will set your enemies' (opposing team members in team games and everyone else in non-team games) colors as you see them. A value of '-' in any digit here will cancel the override and set the corresponding color to the enemy's preference in non-team games or the team color (red or blue) in team games. Since the effect colors (last two digits) will not be set to red/blue in team games, the enemies will still be able to set them by either using the last two digits of their am_colors variables, or their color1 and color2 variables.

		For example, an am_enemyColors value of "2-22-":
			Will set enemies' head color to green (2).
			Will set enemies' torso color to their team color.
			Will set enemies' pants' color to green (2).
			Will set enemies' primary effects color to green (2).
			Will set enemies' secondary effects color to their own preference. (Last digit of their am_colors (or their "color2" if '-'.))

	* am_friendlyColors "abcde"
		This variable will set your teammates' colors in a fashion similar to the am_enemyColors variable above. It will also change your own look in the spirit of teamplay.

	To recap:
	- In team games,
		The first three digits:
			am_enemyColors/am_friendlyColors overrides team color.
		The last two digits:
			am_enemyColors/am_friendlyColors overrides am_colors overrides color1/color2.
	- In non-team games,
		The first three digits:
			am_enemyColors overrides am_colors overrides white.
		The last two digits:
			am_enemyColors overrides am_colors overrides color1/color2.

	The color chart:
		1	Blue
		2	Green
		3	Cyan
		4	Red
		5	Magenta
		6	Yellow
		7	White
		8	Gray
		a	Pastel blue
		b	Pastel green
		c	Pastel red
		d	Indigo
		e	Light blue
		f	Lime
		g	Aqua
		h	Orange
		i	Pink


Gametypes:

	* Midair Arena (g_gametype 3):
		The aim of this gametype is to shoot the enemies while they are in the air. Players are given infinite rockets and grenades, and are devoid of all other weapons, items and powerups. They are invulnerable except to rockets and grenades while in the air. If shot in such a fashion, they will die upon touching the ground (thus allowing for combos.) Suiciding is disabled, dying doesn't cost any points.

	* Rambomatch (g_gametype 4):
		One person is deemed the "rambo". Only killing the rambo and kills made by the rambo count as a point. If the rambo is killed or otherwise dies, the last person to attack them becomes rambo. If there is no such person, the next person to spawn becomes rambo. Suicides give you -1 point.

	* Team Rambomatch (g_gametype 6):
		Same as above, team scores are the sum of the indiviuals' scores. Attacking a teammate rambo doesn't count, so if the rambo dies, rambo status goes to the last enemy to attack the rambo, or to the next person to spawn. Normal kills count as 1 point, rambo kills as 2 points, and a team that steals rambo from the other team gets 10 points, awarded to the person to become the new rambo.

	* Football (g_gametype 11):
		The Quake version of football (or handball, or soccer, etc. --point being, there is a ball.) The players' aim is, of course, to shoot it through the opposing team's goal. If maps have custom goals (see the Mapmaking section below) and/or a custom football spawn point, they will be used. If not, the ball will spawn at the white flag's spawn point and red/blue flags will be replaced by premade goals.


Mapmaking:
	* Football maps:

		The Football gametype introduces a few custom brushes (regions or triggers) and entities (points.)

		The first is a point entity called "football_ball". This is where the ball spawns at the beginning of a football game, after a goal is scored or the ball goes out. If this entity doesn't exist on a map, the white/neutral flag spawn point ("team_CTF_neutralflag") will be used.

		The red/blue goals are very customizable as they use region brushes. When the ball goes inside a region called "football_redgoal", it scores a goal for the blue team, and vice versa for "football_bluegoal". These regions will be playerclip (the players won't be able to go through them except in training mode), but otherwise immaterial. They can be any shape and size, and you can have multiples of these regions, so it is possible to create a goal (goal post) having any shape you desire. Note that you will have to create their surroundings as well, e.g. actual posts for the goal or back/top/left/right-side covers so that it's only possible to shoot from the front.

		If these regions don't exist, then aibsmod will search for the CTF red/blue flag entities and replace them with simple cuboid goals. These goals will have top, back, left and right covers and will face the direction where the original flag would be facing. (Which is not always the proper orientation, but luckily it is possible to rotate them with the am_redGoalRotation/am_blueGoalRotation admin variables.

		If you're using GTKRadiant, you might want to add these lines to your entities.ent file, preferably just before the last "</classes>" line:

		<!-- aibsmod stuff -->
		<point name="football_ball" color=".8 .8 .8" box="-8 -8 -8 8 8 8" model="../aibsmod/models/football/ball.md3">
		This is the football (spawn point) for aibsmod football.

		If this entity doesn't exist on a map, the ball will spawn at the neutral flag post.
		</point>

		<group name="football_redgoal" color=".8 .1 .1">
		This is used for aibsmod custom football goal posts. When the ball goes inside this region, it will score a goal for the opposing (blue) team.

		If this region doesn't exist on a map, the CTF flag posts will be replaced by premade goal models, whose insides will consist of this region.
		</group>

		<group name="football_bluegoal" color=".1 .1 .8">
		This is used for aibsmod custom football goal posts. When the ball goes inside this region, it will score a goal for the opposing (red) team.

		If this region doesn't exist on a map, the CTF flag posts will be replaced by premade goal models, whose insides will consist of this region.
		</group>
		<!-- aibsmod stuff ends -->

	* General:

		There are two worldspawn keys your maps can use:

		"spawn_health" directly adjusts the value of the am_spawnHealth server setting and allows you to set the amount of health the players start with.

		"spawn_nomg" directly adjusts the value of the am_spawnNoMG server setting and allows you to control whether players start with a Machine Gun.


Notes:

	This mod is not compatible with vanilla Q3 clients. Although aibsmod contains many modifications, VQ3-like behavior was a priority in the design and where possible, the default values of settings were chosen such that unconfigured aibsmod servers and clients both would look and feel like their VQ3 counterparts.


Credits:

	Orhan "aib" Kavrakoglu - http://URL - aibok42 gmail.
		Code
		Concepts
		Design
		Models
		Sounds
		Bad-looking graphics

	Baska kimseyi yazmiyorum. Asagidakileri silicem. Mesaj atin.

/*
	gelaek/skos
		Concepts; Maps: dy3ram, football, football2; Testing; Sounds

	cruelstroke
		Graphics: tripmine; Maps: football

	fuayfsilfm/vesc
		Concepts; Early motivation

	disq
		Concepts; Testing; Reverse engineering

	Testing:
	The Sourgaming Crew - http://sourgaming.blogspot.com/
		skos
		vesc
		cruelstroke
		zfs
		Dreacos
		bosluk
		lepidodendron
		disq
		aib
*/
	Special thanks to zfs

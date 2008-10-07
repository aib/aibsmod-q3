*** aibsmod_test7 Documentation ***

Notes for #sourgaming:

	Cikan buglari ve onerilerinizi bana iletiyorsunuz, degil mi?
	
	Moda yeni bir oyun turu koymayi dusunuyorum. Ramboyu da cok kisiyle test etmedik daha, pek oturmadi.
	Bu release'i herkese dagitabilirsek bol bol oynama sansimiz olur.
	
	skos birkac yeni map cikardi, onlari da almayi unutmayin.
	
	Modun bi versiyonunda kurtuldugumdan emin olamadigim bir bug var gibi, harita degisiminde oyunu crash edebiliyo. Oyle bir sey oldugu takdirde pak1.pk3 seklinde bir update cikarabilirim, yeni ekleyecek bir seyim yoksa. Serverda test etmeden once 3-4 kisi soyle bikac oyun atmak istiyorum, sonucta server bu moda gectigi andan itibaren modsuz Q3'le girilemeyecek. (Gerci 26667'ye pek giren yok artik)
	
	
	test6 notlari:
	
	Su anda aibsmod'un test6 versiyonunu denemektesiniz. Simdiye kadar tum onerileriniz, geri donusleriniz ve desteginiz icin tesekkur ederim. Yakinda modun test olmayan, duz versiyonlari cikacaktir ve duzgun bir web sitesinden indirilebilir hale gelecektir. Mod, bu versiyondan itibaren VQ3 (modsuz, duz Quake 3 clientlari) ile uyumsuzdur ve oynamak icin hem serverin hem de butun oyuncularin moda sahip olmasini gerektirir. disq'in, VQ3 sunucusunu bu moda gecirip gecirmeyecegini bilmiyorum, daha konusmadik, fakat bunu yapmasi, ben bu modu release edip web siteme koyana kadar mantiksiz olacaktir; zira sunucumuza disardan gelen insanlar mevcut.
	
	Bana her zamanki gibi kanaldan ulasabilirsiniz, ve hatta ulasiniz. Moda ekletmek istediginiz fikirler varsa hemen iletiniz, cunku mod test surecinden cikmak uzeredir, bundan sonra buyuk degisiklikler yapmak cok daha uzun zaman alacaktir.

Installing:

	Extract the archive file (probably the one you found this file in) to your Quake 3 directory. The archive contains a directory itself, so you should have:

	* X:\Quake3\aibsmod\readme.txt
	* X:\Quake3\aibsmod\pak0.pk3

	Where X:\Quake3 is your Quake 3 directory.


Variables:
Server Variables/Settings:

	* am_fastWeaponSwitch <0/1>
		This server setting controls whether weapon switching occurs instantly (1; CPMA/3wave style) or with a delay (0; VQ3). Players can "callvote fastswitch 0/1" to change this setting.

	* am_trainingMode <0/1/2>
		This server setting determines whether "training mode" is active or not. If active (1 or 2), players can walk through eachother and will not telefrag one another. Weapons will cause knockback but no damage. In addition, if this setting is 1, players will be given all weapons and infinite ammo. Players can "callvote training 0/1/2" to change this setting.

	* am_selfDamage <0/1>
		This server setting controls whether players can damage themselves. If set to 0, players cannot damage themselves (neither health nor armorwise) but knockback still applies. Players can "callvote selfdamage 0/1" to change this setting.

	* am_piercingRail <0/1>
		This server setting controls whether railgun shots go through walls. Players can "callvote piercing 0/1" to change this setting.

	* am_hyperGauntlet <0/1>
		This server setting controls whether the gauntlet does 1000 (instant-kill) or 50 (normal) damage. Players can "callvote hypergauntlet 0/1" to change this setting.

	* am_nonRamboKill <0/1/2>
		This server setting controls whether non-rambo players can be damaged by other non-rambo players. If it is "0", non-rambo players cannot be damaged by other non-rambo players. A value of "1" will allow such damage, but will not reward (or punish) such kills in [non-team] Rambomatch mode. A value of "2" will punish non-rambo killers by subtracting a point from their score in [non-team] Rambomatch mode. In Team Rambomatch mode, settings 1 and 2 do not differ and always reward opposing team non-rambo kills with +1 and punish team kills with -1. Players can "callvote nonrambokill 0/1/2" to change this setting.
		
	* am_redGoalRotation and am_blueGoalRotation
		These server settings rotate the red and blue goalposts respectively, if the map isn't using custom ones. They are necessary on maps where the red/blue flag orientation isn't correct, i.e. the goalposts that have replaced the flags in Football mode are facing the wrong way. They can also be used to make scoring a goal more difficult. Their values are in degrees clockwise from the original orientation. (e.g. "am_redGoalRotation 90" means "turn the red goalpost 90 degrees clockwise")


Client Variables/Settings:

	* am_showKillNotice <0/1/2>
		This client setting controls where the [occasionally annoying] "You fragged x/1st place with n kills" message appears. "1" is the default setting and doesn't change anything. Setting it to "0" will completely disable the message (you can still read your own kills from the "chat log" in the upper left corner) and setting it to "2" will make the message appear in the top center side of your screen, as opposed to the dead middle. Give it a try.

	* am_drawSpeed <0/1/2>
		This client setting draws the infamous "player speed meter" (if not "0".) "1" draws it in the upper right corner, just below the FPS meter; "2" draws it in the middle of the screen, a bit below the crosshair.

	* am_drawSpeedMethod <0/1/2>
	* am_drawSpeedFrames <1-255>
		See the section entitled "The Enhanced Speed Meter" below.
		
	* am_drawButtons <0/1>
		This client setting controls whether an overlay indicating the buttons pressed by the current player (or spectator target) will be drawn.


The Enhanced Scoreboard:
	
	aibsmod has an enhanced scoreboard: To the left of players' names, their Excellent, Impressive and Gauntlet (Humiliation) counts are listed in their respective colors (Excellent=yellow, Impressive=cyan, Gauntlet=red). Furthermore, if a Rambomatch or Team Rambomatch type game is being played, the rambo has their background highlighted in green.


The Enhanced Speed Meter:
	
	aibsmod has an "enhanced" speed meter built-in. It has different methods of calculating the player's horizontal (x/y) speed, over varying amounts of time. The mod records the client speed over am_drawSpeedFrames frames, adjustable from 1 to 255. What it does with this information is up to am_drawSpeedMethod:
	
	Method 0 (or any number other than 1 and 2) is a simple average. Speeds recorded on the previous N frames are added up and then the result is divided by N, giving the average speed.
	
	Method 1 is the maximum speed attained over N frames. It simply gives the highest speed among the previous N frames.
	
	Method 2 is the speed attained at the highest client position among the previous N frames. In this method, the mod records the player's z-coordinate (up/down axis) value in addition to the speed over the last N frames, and gives the speed at the highest point among these N frames. Note that this value will be lower than the maximum speed attained as the player gains horizontal speed while falling down; this is precisely the information discarded by this method.

	"N" in the descriptions above stands for the current value of am_drawSpeedFrames, clipped to 1 or 255 if outside that range.
	
	Thus, am_drawSpeedMethod 0 and am_drawSpeedFrames 1 will give you your current speed; am_drawSpeedMethod 1 and am_drawSpeedFrames 10 will give you your maximum speed in the last 10 frames; am_drawSpeedMethod 2 and am_drawSpeedMethod 100 will give you your speed at the time you were at your highest vertical position in the last 100 frames.


Gametypes:

	* Rambomatch (g_gametype 3):
		One person is deemed the "rambo". Only killing the rambo and kills made by the rambo count as a point. If the rambo is killed or otherwise dies, the last person to attack them becomes rambo. If there is no such person, the next person to spawn becomes rambo. Suicides give you -1 point.

	* Team Rambomatch (g_gametype 5):
		Same as above, team scores are the sum of the indiviuals' scores. Attacking a teammate rambo doesn't count, so if the rambo dies, rambo status goes to the last enemy to attack the rambo, or to the next person to spawn. Normal kills count as 1 point, rambo kills as 2 points, and a team that steals rambo from the other team gets 10 points, awarded to the person to become the new rambo.
		
	* Football (g_gametype 10):
		The Quake version of football (or handball, or soccer, etc. --point being, there is a ball.) The players' aim is, of course, to shoot it through the opposing team's goalpost. If maps have custom goalposts (see the Mapmaking section below) and/or a custom football spawn point, they will be used. If not, the ball will spawn at the white flag's spawn point and red/blue flags will be replaced by premade goal posts.


Mapmaking:
	The Football gametype introduces a few custom brushes (regions or triggers) and entities (points.)
	TODO


Notes:

	This mod is no longer compatible with vanilla Q3 clients (meaning clients lacking this mod.) This was a design decision taken around test6, where fast weapon switching, training mode and the button display was implemented. Whereas VQ3 clients couldn't simply benefit from the former two, the method used for sending button presses to spectator aibsmod clients dropped their respective spectatees from the game due to the lack of a particular feature in the Q3A code.


Credits:

	Orhan "aib" Kavrakoglu - http://aib.ftuff.com/ and aibok42 gmail.
	Thanks to the guys at irc.freenode.net #sourgaming for helping me test the mod.
	Thanks to gelaek/skos (same place) for making dy3ram, the first map designed for Rambomatch play!
	Thanks to fuayfsilfm/vesc (same place) for keeping my TODO list :S and keeping me motivated.

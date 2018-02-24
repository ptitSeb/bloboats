# Bloboats

This version of Bloboats has been slightly modify for the OpenPandora. most `double` has been changed to `float` and there is an optionnal GLES1.1 renderer. Note that current OpenPandora verison now use gl4es and use original OpenGL renderer.

![sreenshot on Pandora](screenshot.png "screenshot on Pandora")

# Original Notice

Bloboats 1.0.2 by Markus "MakeGho" Kettunen <makegho@blobtrox.net>, http://bloboats.blobtrox.net/

1. Babble babble
2. Installation and configuring
  2.1. Unix-like systems
  2.2. Windows
  2.3. Configuration files
3. Credits
4. Controls & stuff
  4.1. Menu keys           (How do I delete that player or recording?!)
  4.2. In-game keys        (What are the default keys?!)
  4.3. Flags               (Is there a way to disable the crappy sounds completely?!)
  4.4. Recordings          (I can't find the recording I saved in the game!!)
  4.5. Structure of the recordings (That recording doesn't work!!)
  4.6. Public scoreboard   (I think I made a really good time! How fast should I be?)
5. Licence and copying

---

1. Babble babble

Bloboats is a boat racing game in which the objective is to reach the goal
as fast as possible, at least faster than your friend does. Perhaps the poor ship,
MS Enterprise, is always in distress, or something.

The development of Bloboats was started just for fun, but as Assembly '06 got closer,
the plan changed a bit. Now the plan was to not be the worst ranked in Assembly '06 game
development competition. The physics engine of Bloboats uses some heavy
spring forces related calculation with heavy damping, and was not originally developed
for Bloboats but just for my own testing purposes -- just for fun. As always.

Bloboats is licensed under GNU GPL 2 except for some data files. For details, see
"5. License and copying" and file "copying.txt".

The game was ranked 4th out of 14 competitors.

2. Installation and configuring

2.1. Unix-like systems

- Install SDL (http://www.libsdl.org, dev and runtime)
- Install SDL_image (http://www.libsdl.org/projects/SDL_image, dev and runtime)
- Install SDL_mixer (http://www.libsdl.org/projects/SDL_mixer, dev and runtime)
- Install libvorbis

It is possible to install without sounds (SDL_mixer) by setting SOUND = 0 in the Makefile
and deleting all -lSDL_mixer entries.

Installation steps:

1. Open Makefile in a text editor
2. Read the help lines and apply the needed changes
3. Compile by typing 'make', or if that doesn't work, try 'gmake'. Make sure you have
writing permissions to the installation directory (you may specify it in the Makefile).
Also make sure you have SDL-dev, SDL_image-dev, SDL_net-dev and SDL_mixer-dev.
4. Remember to do 'make install'. If you don't have root privileges, consult Makefile


2.2. Windows
- Run the installer


2.3. Configuration files

- Private directory (in Unix-like systems, specified in the Makefile, in Windows usually 'private')
contains directories 'ghost' and 'records'. Ghost directory contains the best run for each level
and 'records' directory contains all the recordings you've created with the game or copied there
for watching purposes.

- players.dat in the private directory shouldn't be touched. It contains the private highscores
for every player and the number of secrets found, etc. If this file is removed, it will be automatically
re-created, without information about the players.

- levels.dat contains the best times of all players listed in Select player menu. Deleting forces
highscore reset.

- config.dat has some information about the settings selected in options menu. Change the order of
these or remove a line and you may lose them all. This file is not meant to be edited, but will
be automatically recreated with default settings if deleted.


---

3. Credits

Bloboats was made by Markus "MakeGho" Kettunen, and released by Blobtrox.
Musics by Pauli "Gwaur" Marttinen

Thanks and greetings to:
  - Pauli "Gwaur" Marttinen for all the music in the game and data/images/ground2.png
  - Jouni "Cornix" Puotila for data/images/bloboats.png
  - Miiro "miiro" Lindfors for data/images/alus.png
  - Henri "str4nd" Strand for the homepage
  - Aspekt
  - Blobtrox
  - Fingersoft
  - Lonelycoders
  - Moonhazard
  - Abyss :)
  - Arcatan
  - Daed
  - kaviaari
  - Malkkis
  - Nexton
  - Sponji
  - sooda
  - tArzAn
  - You too, yes, you.

If you should be here, please tell me. (You are!)


---

4. Controls & stuff

4.1. Menu keys

Enter key selects, up and down change selection.
In the scoreboard view left and right change level, and in the volume view
left and right arrow keys change volume.

Delete or d may be used to delete player slots or recordings.


4.2. In-game keys

F1 toggles special information mode for hackers
F2 toggles FPS view

Default keys:

Left and right arrow keys rotate the ship. (Counter-clockwise, clockwise)
Up key thrusts, down key reverses
C jumps (the higher the bigger part of the ship is in the water)
R and F zoom

Note that these may be changed from the controls menu.


4.3. Flags

These are the flags for running bloboats from command line

--nosound : Sounds will be turned completely off until you start bloboats with --sound
--sound : Turns sound on if it has been turned off with --nosound
--windowed : Tells Bloboats to be run in windowed mode. Flag -w does the same.
--fullscreen : Tells Bloboats to be run in fullscreen mode. Flag -f does the same.
--resolution=NxM : Changes resolution to NxM.
--nolimit : Disables some limitations; used for checking validity of a record.
--help : displays help about the command line flags

Notice: in Windows the output goes to stdout.txt in the installation directory


4.4. Recordings

It is possible to open recordings straight with bloboats by selecting "open with"
from whatever you might be using. (bloboats <filename> in the correct directory in command line)

To distribute a recording, copy it from private/records/ and if you wish to play
a recording given to you, put it into private/records/. Replace private with whatever
privatedir is set to in file bloboats.dirs or in Unix-like systems, /etc/bloboats.dirs

Note that in order to be shown in the view records menu correctly, the name of the
recording must be written in capital letters (except for the .brc, (bloboats recording) extension)


4.5. Structure of the recordings

The recording files are just heavily compressed keyboard layout maps for every frame,
thus if you ever encounter a recording that "doesn't work" although it might even be in the
public scoreboard (http://bloboats.blobtrox.net/), it is most probably because your computer and
the player's computer calculate floating point calculations a bit differently. Although the 
error for one calculation is really small, it cumulates and may corrupt the recording.

This may even happen in your own recordings on the same computer, especially if your hardware
is overclocked, but in any case, this should be extremely rare.


4.6. Public scoreboard

You can submit your best times to the public scoreboard at http://bloboats.blobtrox.net/
by pressing the "Add a record!" link in the main page.

Tip: private/ghost directory has your best recordings for all the levels.


4.7. Secrets?

Yes, please. There are six special secrets hidden in the game. You'll find out.

---

5. License and copying

See "copying.txt"


Samples used from Freesound:

RHumphries ( http://freesound.iua.upf.edu/usersViewSingle.php?id=1112 )
	rbh Applause 01 big.WAV ( http://freesound.iua.upf.edu/samplesViewSingle.php?id=1921 )
meatball4u ( http://freesound.iua.upf.edu/usersViewSingle.php?id=59895 )
	explode4.wav ( http://freesound.iua.upf.edu/samplesViewSingle.php?id=17226 )
harri ( http://freesound.iua.upf.edu/usersViewSingle.php?id=18811 )
	1_fridge_open.mp3 ( http://freesound.iua.upf.edu/samplesViewSingle.php?id=8865 )
petenice ( http://freesound.iua.upf.edu/usersViewSingle.php?id=26137 )
	SPLASH.wav (http://freesound.iua.upf.edu/samplesViewSingle.php?id=9508 )
batchku ( http://freesound.iua.upf.edu/usersViewSingle.php?id=186 )
	colide-18_017.aif ( http://freesound.iua.upf.edu/samplesViewSingle.php?id=10479 )
NoiseCollector ( http://freesound.iua.upf.edu/usersViewSingle.php?id=4948 )
	granular ambience 2 boat.wav ( http://freesound.iua.upf.edu/samplesViewSingle.php?id=12510 )
By pushtobreak (http://freesound.iua.upf.edu/usersViewSingle.php?id=9056)
            Earth1.aif (http://freesound.iua.upf.edu/samplesViewSingle.php?id=16793)

Some of the sounds have been modified a bit or a lot for better suitability.


For details about the license information and GNU GPL version 2, see file "copying.txt".

Excession engine, 18 July 2001
==============================

Version 0.78 alpha

This is only supposed to be a downloadable demo of progress for our development partners.

Graphics & algorithms:  Rob Takken, the Netherlands
OpenGL Engine, C++ programming & algorithms:  Peter de Vocht, New Zealand

System requirements
===================
This program has been tested on the following systems:
OS: Win2K, Win98SE
Memory: min. 32MB
CPU: tested on an AMD 750MHz Duron, Intel 600 MHz PIII
Video cards tested: Riva TNT 2 (~16 fps), GeForce (~35 fps), GeForce II MX (~60 fps)
Wouldn't recommend anything less than a TNT 2, would recommend GeForce or better.

Controls
========
game:	x=explode tank (test feature)
	c=display vehicle (x,y,z)
	cursor keys for tank movement
	mouse move left/right & left mouse rotate left/right
	mouse move back/fwd & left mouse zoom in/out
	ctrl + mouse - targetting
	1,2,3,4 select cameras 1,2,3 and 4 (3 and 4 have currently no function)
	left mouse - fire
	ALT+Enter toggle full screen
	ALT+F4 quit
	press ESC to return to menu when in game

History
=======
updates: 18 July 2001
- added camera views & tiger tank, can be controled with second camera view
- fixed destruction bug (pfew!)

updates: 15 July 2001
- replaced all char* with TString objects
- wrote version 1.0 of the playback/action recording system

updates: 14 July 2001
- updated setup.exe to version 1.01
  replaced bitmap, improved interface a little, added various buttons
  added on/off and a few more functions for dealing with GDI controls
- replaced MP3 player (my mistake - should have read MSDN first) with
  PlayWave()

updates: 13 July 2001
- still crashes on certain object destruction (will be fixed soon)
  (throws exceptions - doesn't take down your machine anymore
- incorporated speed test
- wrote setup and now using registry settings for fog, resolution, bit depth,
  fullscreen and others
- speed test uses experimental mp3 player (frigging microsoft graph stuff)
- all log files are now written to whatever your temp or tmp environment var.
  is set to

updates: 11 July 2001
- added texture cache (invisible to high level objects)
- added object system
- updated converter to version 1.10 for objects & mesh merges
- updated converter to version 1.11 for speed tests etc.
- added temporary object destruction for demo purposes

updates: 26 June 2001
- added targeting system (still has hill adjustment bug)
- added explosion animation
- cleaned up logic & render code

updates: 22 June 2001
- implemented landscape object structure, creating real objects and obstacles
  on the landscape (including the trees)
- added right mouse button turret/barrel move in addition to CTRL key movements

updates: 16 June 2001
- implemented moving turret and barrel (use mouse + CTRL key down movements)
- fixed normals on tank relative to landscape - should draw correct now
- added primitive menu bar interface and changed mouse interface
- switched to using DirectX 7 for fullscreen mode, hopefully fixes crashes
  on certain machines.
- switched on auto logging to "enginelog.txt" and switched off console window
- cleaned up code
- fixed DX7 bugs
- fixed shutdown bug using close box
- added exhaust points to tanks
- fixed fullscreen switch bug (when in fullscreen mode -> switch to another app using
  TAB + ALT caused app to remain active)

updates: 29 May 2001
- cleaned up code (introduced copy constructors for all objects)
- removed low level pointers from code with actual objects & void constructors
- rob cleaned up water animation and memory usage
- bridges still need fixing

updates: 26 May 2001
- updated bridge algorithms to meshes (up version 7 of data for landscape)
- updated renderDepth to include size of landscape Mesh & max & min renderDepth accordingly
- moved vehicle logic from render to move routine

updates: 24 May 2001
- added animations to engine
- updated tools to generate animations & merged meshes
- added apdaptive frame rate engine for slow-ish machines and video cards
- added water height restrictions as no-go zones
- fixed animation sequencing bug
- lowered landscape memory usage by fixing inefficiencies
- added Manhattan heuristics to triangle find routines (should be faster)

updates: 18 May 2001
- preliminary landscape engine finished, we have a rough alpha
- added self animating textures to engine
- added pause animation capability
- added smoke to tank engine
- added animations to tanks
- added 3D sound back to engine (after it had been removed for a while)
- added auto camera follow to tank
- added command line fullscreen switch
- added versioning to dev tools & engine
- created new .lnd format for landscapes based on previous .bin format
- created landscape division software
- added multiple mesh divisions
- added recursive graphing & locality of reference triangle searches
- fixed graphing algorithm for the ump-teenth time (removed recursive loops)
- height mapping & graphs finished - triangle algorithm works
- line intersection algebra works
- added slow-down movement on hills
- moved keys to cursor keys & mouse movement
- finished fourth version of .lnd - includes bridges, no-go zones,
  triangle graphs, materials and meshes.
- fixed .bin format - cleaned up messy material references & replaced
  with indexes.
- introduced new classes: TLandscapeObject, TVehicle, based on TObject

updates: 27 March 2001
- seperated frame rate from engine logic, engine logic runs @ 20 fps
  screen logic runs at max possible speed
- added console for engine text feedback
- added enumeration for possible display modes (PIXELFORMATDESCRIPTORS)
- made sure accumulation buffer is clear & not used
- fixed CS_OWNDC bug
- releases CPU on minimise or deactivation (be nice)
- added res switches

updates: 22 March 2001
- finally updated Win32 engine to work properly with OpenGL
- full screen mode reverts to windowed mode on loss of focus
- tanks roughly follow landscape height (very rough)
- cleaned up library code & commented relevant sections

updates: 9 March 2001
- canibalised parts of previous engine
- switched off openGL blending, not yet needed
- added ALT+ENTER full screen mode using DirectX calls
- switched on mip-mapping reducing graphics flicker
- changed viewer size to 640x480 for now
- should run best in 16 bit colour depth

way back:
- this engine started life as "Peter's 3D living" back in 1999


Contact
=======
Peter de Vocht: email:peter@peter.co.nz
Rob Takken:     email:scarab@wanadoo.nl

alternatively - web sites:
Rob  : http://home.wanadoo.nl/scarab/tanks.htm
Peter: http://www.peter.co.nz/


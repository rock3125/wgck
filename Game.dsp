# Microsoft Developer Studio Project File - Name="Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Game - Win32 Scitech
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak" CFG="Game - Win32 Scitech"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Game - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Scitech" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Scitech release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release\game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Od /I "C:\Archive1\Games\wgi" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 mdllib.lib fmodvc.lib wsock32.lib zlib.lib pnglib.lib jpeglib.lib opengl32.lib glu32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcd.lib"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug\game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Archive1\Games\wgi" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fmodvc.lib wsock32.lib zlib.lib pnglib.lib jpeglib.lib mdllib.lib opengl32.lib glu32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib"
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "Game - Win32 Scitech"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Game___Win32_Scitech"
# PROP BASE Intermediate_Dir "Game___Win32_Scitech"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "debug\Game\Scitech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Archive1\WarGamesInc\game" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /D "_MESA" /YX /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Archive1\Games\wgi" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /D "_MESA" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gm.lib mglfx.lib fmodvc.lib wsock32.lib zlib.lib pnglib.lib jpeglib.lib mdllib.lib glu32.lib ddraw.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib"
# SUBTRACT BASE LINK32 /profile /map
# ADD LINK32 ztimer.lib mglfx.lib gm.lib fmodvc.lib wsock32.lib zlib.lib pnglib.lib jpeglib.lib mdllib.lib glu32.lib ddraw.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib"
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "Game - Win32 Scitech release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game___Win32_Scitech_release"
# PROP BASE Intermediate_Dir "Game___Win32_Scitech_release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "release\game\scitech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Od /I "C:\Archive1\WarGamesInc\game" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /GX /Od /I "C:\Archive1\Games\wgi" /D "_MESA" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mdllib.lib fmodvc.lib wsock32.lib zlib.lib pnglib.lib jpeglib.lib opengl32.lib glu32.lib ddraw.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcd.lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ztimer.lib mglfx.lib gm.lib mdllib.lib fmodvc.lib wsock32.lib zlib.lib pnglib.lib jpeglib.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmt.lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Game - Win32 Release"
# Name "Game - Win32 Debug"
# Name "Game - Win32 Scitech"
# Name "Game - Win32 Scitech release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\tank\main.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_logic.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_logicmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_logicserver.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_network.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_render.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_rendermenu.cpp
# End Source File
# Begin Source File

SOURCE=.\tank\tankapp_renderserver.cpp
# End Source File
# End Group
# Begin Group "ai"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ai\artilleryai.cpp
# End Source File
# Begin Source File

SOURCE=.\botCompiler\botgrammar.cpp
# End Source File
# Begin Source File

SOURCE=.\botCompiler\botparser.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\flakai.cpp
# End Source File
# Begin Source File

SOURCE=.\botCompiler\interpretor.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\landscapeai.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\mg42ai.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\planeai.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\tankai.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\v2ai.cpp
# End Source File
# End Group
# Begin Group "win32 resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32\win32resource.rc
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32\engineLogo.jpg
# End Source File
# Begin Source File

SOURCE=.\Win32\germanFlag.bmp
# End Source File
# Begin Source File

SOURCE=.\Win32\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Win32\myGL.ico
# End Source File
# Begin Source File

SOURCE=.\Win32\register.jpg
# End Source File
# Begin Source File

SOURCE=.\Win32\spaceStationCredits.jpg
# End Source File
# Begin Source File

SOURCE=.\Win32\unionJack.bmp
# End Source File
# End Group
# Begin Group "dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dialogs\audioSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\controlsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\downloadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\helpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\hostDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\joinDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\singlePlayerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs\videoSettings.cpp
# End Source File
# End Group
# Begin Group "ee system dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\systemDialogs\aboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\dialogAssist.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\enterCodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\enterRego.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\gui.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\languageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\registerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\timeLimit.cpp
# End Source File
# Begin Source File

SOURCE=.\systemDialogs\tipsDlg.cpp
# End Source File
# End Group
# Begin Group "ee sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3dSound\fmod_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sounds.cpp
# End Source File
# Begin Source File

SOURCE=.\3dSound\wavread.cpp
# End Source File
# End Group
# Begin Group "ee network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\networking.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\packet.cpp
# End Source File
# End Group
# Begin Group "ee object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\object\anim.cpp
# End Source File
# Begin Source File

SOURCE=.\object\artillery.cpp
# End Source File
# Begin Source File

SOURCE=.\object\binobject.cpp
# End Source File
# Begin Source File

SOURCE=.\object\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\object\character.cpp
# End Source File
# Begin Source File

SOURCE=.\object\compoundObject.cpp
# End Source File
# Begin Source File

SOURCE=.\object\createlandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\object\deffile.cpp
# End Source File
# Begin Source File

SOURCE=.\object\explosion.cpp
# End Source File
# Begin Source File

SOURCE=.\object\explosive.cpp
# End Source File
# Begin Source File

SOURCE=.\object\flak.cpp
# End Source File
# Begin Source File

SOURCE=.\object\geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\object\landscape.cpp
# End Source File
# Begin Source File

SOURCE=.\object\landscapeObject.cpp
# End Source File
# Begin Source File

SOURCE=.\object\material.cpp
# End Source File
# Begin Source File

SOURCE=.\object\mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\object\mg42.cpp
# End Source File
# Begin Source File

SOURCE=.\object\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\object\projectile.cpp
# End Source File
# Begin Source File

SOURCE=.\object\recordSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\object\smoke.cpp
# End Source File
# Begin Source File

SOURCE=.\object\soundTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\object\tank.cpp
# End Source File
# Begin Source File

SOURCE=.\object\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\object\tga.cpp
# End Source File
# Begin Source File

SOURCE=.\object\vector.cpp
# End Source File
# Begin Source File

SOURCE=.\object\vehicle.cpp
# End Source File
# End Group
# Begin Group "ee win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32\events.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32\mesaWin32.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32\params.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32\story.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32\tips.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32\vehicleSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32\win32.cpp
# End Source File
# End Group
# Begin Group "ee common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\compatability.cpp
# End Source File
# Begin Source File

SOURCE=.\common\font.cpp
# End Source File
# Begin Source File

SOURCE=.\common\languageStrings.cpp
# End Source File
# Begin Source File

SOURCE=.\common\string.cpp
# End Source File
# End Group
# Begin Group "mdl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mdllib\mathlib.c
# End Source File
# Begin Source File

SOURCE=.\mdllib\MDL.cpp
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="excession" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=excession - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "excession.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "excession.mak" CFG="excession - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "excession - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "excession - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "excession - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "excession___Win32_Release"
# PROP BASE Intermediate_Dir "excession___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release\excession"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\Archive1\Games\wgi" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "excession - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "excession___Win32_Debug"
# PROP BASE Intermediate_Dir "excession___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug\excession"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Archive1\Games\wgi" /D "_MSVC" /D "_USEDX7" /D "_DXSOUND_" /D "_USE3DSOUND_" /D "_USEFMOD" /D "_USEGUI" /D "_NETWORK" /D "_TANKGAME" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "excession - Win32 Release"
# Name "excession - Win32 Debug"
# Begin Group "ee common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\compatability.cpp
# End Source File
# Begin Source File

SOURCE=.\common\font.cpp
# End Source File
# Begin Source File

SOURCE=.\common\string.cpp
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

SOURCE=.\object\graph.cpp
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
# Begin Group "ee network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\networking.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\packet.cpp
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
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="Performance" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Performance - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Performance.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Performance.mak" CFG="Performance - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Performance - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Performance - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Performance - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Performance___Win32_Release"
# PROP BASE Intermediate_Dir "Performance___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "release\performance"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\ARCHIVE1\PROJECT TANK\source\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DXSOUND_" /D "_MSVC" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 AMSTRMID.LIB dxguid.lib dsound.lib jpeglib.lib opengl32.lib glu32.lib ddraw.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Performance - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Performance___Win32_Debug"
# PROP BASE Intermediate_Dir "Performance___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug\performance"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\ARCHIVE1\PROJECT TANK\source\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC" /D "_DXSOUND_" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AMSTRMID.LIB dxguid.lib dsound.lib jpeglib.lib opengl32.lib glu32.lib ddraw.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Performance - Win32 Release"
# Name "Performance - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=".\speed test\speedTest.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\PerformanceWin32\myGL.ico
# End Source File
# Begin Source File

SOURCE=.\PerformanceWin32\spaceStationCredits.jpg
# End Source File
# End Group
# Begin Group "object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\object\anim.cpp
# End Source File
# Begin Source File

SOURCE=.\object\binobject.cpp
# End Source File
# Begin Source File

SOURCE=.\object\compoundObject.cpp
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

SOURCE=.\object\object.cpp
# End Source File
# Begin Source File

SOURCE=.\object\smoke.cpp
# End Source File
# Begin Source File

SOURCE=.\object\tank.cpp
# End Source File
# Begin Source File

SOURCE=.\object\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\object\vector.cpp
# End Source File
# Begin Source File

SOURCE=.\object\vehicle.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\compatability.cpp
# End Source File
# Begin Source File

SOURCE=.\3dSound\play3dsound.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sounds.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tstring.cpp
# End Source File
# Begin Source File

SOURCE=.\3dSound\wavread.cpp
# End Source File
# End Group
# Begin Group "PerformanceWin32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PerformanceWin32\events.cpp
# End Source File
# Begin Source File

SOURCE=.\PerformanceWin32\win32.cpp
# End Source File
# Begin Source File

SOURCE=.\PerformanceWin32\win32resource.rc
# End Source File
# End Group
# End Target
# End Project

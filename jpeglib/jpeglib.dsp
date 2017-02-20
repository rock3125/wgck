# Microsoft Developer Studio Project File - Name="jpeglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jpeglib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak" CFG="jpeglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpeglib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release\jpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\Archive1\Games\wgi" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_MSVC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug\jpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Archive1\Games\wgi" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MSVC" /YX /FD /GZ /c
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

# Name "jpeglib - Win32 Release"
# Name "jpeglib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\JCAPIMIN.C
# End Source File
# Begin Source File

SOURCE=.\JCAPISTD.C
# End Source File
# Begin Source File

SOURCE=.\JCCOEFCT.C
# End Source File
# Begin Source File

SOURCE=.\JCCOLOR.C
# End Source File
# Begin Source File

SOURCE=.\JCDCTMGR.C
# End Source File
# Begin Source File

SOURCE=.\JCHUFF.C
# End Source File
# Begin Source File

SOURCE=.\JCINIT.C
# End Source File
# Begin Source File

SOURCE=.\JCMAINCT.C
# End Source File
# Begin Source File

SOURCE=.\JCMARKER.C
# End Source File
# Begin Source File

SOURCE=.\JCMASTER.C
# End Source File
# Begin Source File

SOURCE=.\JCOMAPI.C
# End Source File
# Begin Source File

SOURCE=.\JCPARAM.C
# End Source File
# Begin Source File

SOURCE=.\JCPHUFF.C
# End Source File
# Begin Source File

SOURCE=.\JCPREPCT.C
# End Source File
# Begin Source File

SOURCE=.\JCSAMPLE.C
# End Source File
# Begin Source File

SOURCE=.\JCTRANS.C
# End Source File
# Begin Source File

SOURCE=.\JDAPIMIN.C
# End Source File
# Begin Source File

SOURCE=.\JDAPISTD.C
# End Source File
# Begin Source File

SOURCE=.\JDATADST.C
# End Source File
# Begin Source File

SOURCE=.\JDATASRC.C
# End Source File
# Begin Source File

SOURCE=.\JDCOEFCT.C
# End Source File
# Begin Source File

SOURCE=.\JDCOLOR.C
# End Source File
# Begin Source File

SOURCE=.\JDDCTMGR.C
# End Source File
# Begin Source File

SOURCE=.\JDHUFF.C
# End Source File
# Begin Source File

SOURCE=.\JDINPUT.C
# End Source File
# Begin Source File

SOURCE=.\JDMAINCT.C
# End Source File
# Begin Source File

SOURCE=.\JDMARKER.C
# End Source File
# Begin Source File

SOURCE=.\JDMASTER.C
# End Source File
# Begin Source File

SOURCE=.\JDMERGE.C
# End Source File
# Begin Source File

SOURCE=.\JDPHUFF.C
# End Source File
# Begin Source File

SOURCE=.\JDPOSTCT.C
# End Source File
# Begin Source File

SOURCE=.\JDSAMPLE.C
# End Source File
# Begin Source File

SOURCE=.\JDTRANS.C
# End Source File
# Begin Source File

SOURCE=.\JERROR.C
# End Source File
# Begin Source File

SOURCE=.\JFDCTFLT.C
# End Source File
# Begin Source File

SOURCE=.\JFDCTFST.C
# End Source File
# Begin Source File

SOURCE=.\JFDCTINT.C
# End Source File
# Begin Source File

SOURCE=.\JIDCTFLT.C
# End Source File
# Begin Source File

SOURCE=.\JIDCTFST.C
# End Source File
# Begin Source File

SOURCE=.\JIDCTINT.C
# End Source File
# Begin Source File

SOURCE=.\JIDCTRED.C
# End Source File
# Begin Source File

SOURCE=.\JMEMMGR.C
# End Source File
# Begin Source File

SOURCE=.\JMEMNOBS.C
# End Source File
# Begin Source File

SOURCE=.\jmemsrc.C
# End Source File
# Begin Source File

SOURCE=.\jpeg_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\JQUANT1.C
# End Source File
# Begin Source File

SOURCE=.\JQUANT2.C
# End Source File
# Begin Source File

SOURCE=.\JUTILS.C
# End Source File
# Begin Source File

SOURCE=.\tjpeg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project

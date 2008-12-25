@REM 
@REM SoundTouch & SoundStretch Build script for Win32 platform
@REM 
@REM You'll need Visual C++ 6.0 installed to compile - also execute the 
@REM "vcvars32.bat" in VC install directotry before running this one.
@REM 
@REM Copyright (c) Olli Parviainen
@REM File Created: 09/Sep/2003
@REM
@REM $Id$ 

@rem ****************************
@rem try first for VS6.0 support

@if "%MsDevDir%"=="" goto nomsdevdir

md bin
md lib
msdev source\SoundTouch\SoundTouch.dsw /MAKE ALL
msdev source\example\SoundStretch\SoundStretch.dsw /MAKE "SoundStretch - Win32 Release"

goto end

:nomsdevdir

@rem **********************************
@rem try with devenv for VS2003 support

@if "%DevEnvDir%"=="" goto nodevdir

md bin
md lib
devenv source\SoundTouch\SoundTouch.vcproj /build debug
devenv source\SoundTouch\SoundTouch.vcproj /build release
devenv source\example\SoundStretch\SoundStretch.vcproj /build debug
devenv source\example\SoundStretch\SoundStretch.vcproj /build release

@goto end


:nodevdir

@echo off
echo *********************************************************************
echo **
echo ** ERROR: Visual Studio path not set.
echo **
echo ** Run "vcvars32.bat" from Visual Studio installation directory, e.g. 
echo ** "\Program Files\Microsoft Visual Studio\VC98\Bin", then try again.
echo **
echo *********************************************************************

:end
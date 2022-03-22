# DoomTools GUI Natives

Copyright (C) 2022 Matt Tropiano

## NOTICE

This will only compile for Windows. Yes, this is intended.


## Introduction

This repo contains C code for building a Java bootstrap for running the GUI Mains in DoomTools
as a child process so that it looks more like a regular WIN32 executable.

Still requires Java, no getting around that.


## Why?

Because running the DoomTools GUI via a CMD script will still produce an unnecessary console
window, and mars the user experience if Windows GUI applications. Just using `javaw` does not
prevent this.

So, this is a bootstrap that works exactly like the DoomTools command-line scripts for Java
detection and latest-JAR-procurement if the user updates their copy.


## Building this Stuff

A Makefile is provided for this project. This has been tested in the following environments
and toolchains:

* Windows 10 x64
* MinGW (GCC 4.8.1+)


## To Build

To build:

	make

To clean up the build directories:

	make clean

To run:

	run


## Other

This program and the accompanying materials are made available under the 
terms of the MIT License which accompanies this distribution.

A copy of the MIT License should have been included in this release (LICENSE.txt).
If it was not, please contact us for a copy, or to notify us of a distribution
that has not included it. 

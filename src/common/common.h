/*****************************************************************************
 * DoomTools GUI Bootstrap for Windows
 * Common Lib Header
 * Matt Tropiano (C) 2022
 * 
 * This program should not be used as a model for anything.
 * 
 * This is only for running the GUI under Microsoft Windows. No other
 * platforms have been tested.
 * 
 * This program and the accompanying materials are made available under 
 * the terms of the MIT License, which accompanies this distribution.
 *****************************************************************************/

#ifndef __GUI_BOOTSTRAP_COMMON__
#define __GUI_BOOTSTRAP_COMMON__

/*****************************************************************************/

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h> 

/****************************************************************************/

#define wfopen _wfopen
#define MAX_ENV 32767
#define MAX_CMDLINE 32767
#define MAX_OUTPATH 1024

/****************************************************************************/

#define ERROR_BADPATHSEARCH   L"INTERNAL ERROR: \"Found\" Java on PATH, but file doesn't EXIST!"

/****************************************************************************/

#define ENVVAR_DOOMTOOLS_PATH L"DOOMTOOLS_PATH"
#define ENVVAR_DOOMTOOLS_JAR  L"DOOMTOOLS_JAR"

/*****************************************************************************/

/**
 * @brief Show an info dialog.
 * @param message the dialog message.
 */
void ShowInfo(LPCWSTR message);

/**
 * @brief Show an error dialog.
 * @param message the dialog message.
 */
void ShowError(LPCWSTR message);

/**
 * @brief Get the parent path of this module, null terminated in the target char buffer (if length is less than maxLen).
 * @param target the target buffer.
 * @param maxLen the max amount of characters.
 * @return Amount of bytes returned.
 */
DWORD GetParentPath(LPWSTR target, DWORD maxLen);

/**
 * @brief Checks if a file exists by path.
 * @param wFilePath the input file path.
 * @return TRUE if so, FALSE if not.
 */
BOOL FileExists(LPCWSTR wFilePath);

/**
 * @brief Searches for Java and if found, returns TRUE and wFoundPath contains the path.
 * @param wExeParentPath the parent path of this executable.
 * @param wFoundPath the output buffer for the found Java EXE path.
 * @return TRUE if found, FALSE if not.
 */
BOOL SearchForJava(LPCWSTR wExeParentPath, LPWSTR wFoundPath);

/**
 * @brief Searches for the most recent JAR file to use.
 * @param wExeParentPath the parent path of this executable.
 * @param wFoundPath if found, the output path to the found file.
 * @return TRUE if found, FALSE if not.
 */
BOOL SearchForJar(LPCWSTR wExeParentPath, LPWSTR wFoundPath);

/*****************************************************************************/
#endif

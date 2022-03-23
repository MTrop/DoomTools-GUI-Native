/*****************************************************************************
 * DoomTools GUI Bootstrap for Windows
 * Common Lib Implementation
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

#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>
#include <windows.h> 

/****************************************************************************/

#include "common.h"

/****************************************************************************/

/**
 * @brief Displays a dialog box.
 * @param wTitle the box title.
 * @param wMessage the message.
 * @param dialogFlags dialog flags.
 */
static void Alert(LPCWSTR wTitle, LPCWSTR wMessage, UINT dialogFlags)
{
	MessageBox(NULL, wMessage, wTitle, dialogFlags);
}

/****************************************************************************/

// See common.h ShowInfo
void ShowInfo(LPCWSTR message)
{
	Alert(L"info", message, MB_OK | MB_ICONINFORMATION);
}

// See common.h ShowError
void ShowError(LPCWSTR message)
{
	Alert(L"Error", message, MB_OK | MB_ICONERROR);
}

// See common.h GetParentPath
DWORD GetParentPath(LPWSTR target, DWORD maxLen)
{
	DWORD bytes = GetModuleFileName(NULL, target, maxLen);
	if (bytes >= 0 && bytes < maxLen)
	{
		wchar_t *c = wcsrchr(target, L'\\');
		if (c)
			*(c) = L'\0';
	}
	return bytes;
}

// See common.h FileExists
BOOL FileExists(LPCWSTR wFilePath)
{
	FILE *wExeFile;
	if ((wExeFile = wfopen(wFilePath, L"r"))) // intentional
	{
		fclose(wExeFile);
		return TRUE;
	}
	return FALSE;
}

// See common.h SearchForJava
BOOL SearchForJava(LPCWSTR wExeParentPath, LPWSTR wFoundPath)
{
	wchar_t wEnvTemp[MAX_ENV];
	wchar_t wPathTemp[MAX_OUTPATH];
	DWORD envLen = 0;

	// Prioritize embedded JRE.
	wsprintf(wPathTemp, L"%s\\jre\\bin\\javaw.exe", wExeParentPath);
	if (FileExists(wPathTemp))
	{
		wsprintf(wFoundPath, L"%s", wPathTemp);
		return TRUE;
	}

	// Search PATH for JAVAW.
	if (SearchPath(NULL, L"javaw.exe", NULL, MAX_ENV, wEnvTemp, NULL))
	{
		if (FileExists(wEnvTemp))
		{
			wsprintf(wFoundPath, L"%s", wEnvTemp);
			return TRUE;
		}
		ShowError(ERROR_BADPATHSEARCH);
		return FALSE;
	}

	// Check JAVA_HOME
	if ((envLen = GetEnvironmentVariable(L"JAVA_HOME", wEnvTemp, MAX_ENV)))
	{
		wsprintf(wPathTemp, L"%s\\bin\\javaw.exe", wEnvTemp);
		if (FileExists(wPathTemp))
		{
			wsprintf(wFoundPath, L"%s", wPathTemp);
			return TRUE;
		}
	}

	// Check JDK_HOME
	if ((envLen = GetEnvironmentVariable(L"JDK_HOME", wEnvTemp, MAX_ENV)))
	{
		wsprintf(wPathTemp, L"%s\\bin\\javaw.exe", wEnvTemp);
		if (FileExists(wPathTemp))
		{
			wsprintf(wFoundPath, L"%s", wPathTemp);
			return TRUE;
		}
	}

	// Check JRE_HOME
	if ((envLen = GetEnvironmentVariable(L"JRE_HOME", wEnvTemp, MAX_ENV)))
	{
		wsprintf(wPathTemp, L"%s\\javaw.exe", wEnvTemp);
		if (FileExists(wPathTemp))
		{
			wsprintf(wFoundPath, L"%s", wPathTemp);
			return TRUE;
		}
	}

	return FALSE;
}

// See common.h SearchForJar
BOOL SearchForJar(LPCWSTR wExeParentPath, LPWSTR wFoundPath)
{
	wchar_t wJarSearchPath[MAX_OUTPATH];
	wsprintf(wJarSearchPath, L"%s\\jar\\*.jar", wExeParentPath);

	BOOL found = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fCurrentFile;

	if ((hFile = FindFirstFile(wJarSearchPath, &fCurrentFile)) == INVALID_HANDLE_VALUE)
		return FALSE;

	do {
		// Get the last lexicographically relevant file.
		if (!found || CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE | SORT_STRINGSORT, fCurrentFile.cFileName, wcslen(fCurrentFile.cFileName), wFoundPath, wcslen(wFoundPath)) > 0)
		{
			wsprintf(wJarSearchPath, L"%s", fCurrentFile.cFileName);
			found = TRUE;
		}
	} while (FindNextFile(hFile, &fCurrentFile));
	
	if (found)
		wsprintf(wFoundPath, L"%s\\jar\\%s", wExeParentPath, wJarSearchPath);

	FindClose(hFile);

	return found;
}

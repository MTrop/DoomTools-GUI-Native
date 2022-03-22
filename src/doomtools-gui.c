/*****************************************************************************
 * DoomTools GUI Bootstrap for Windows
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

#define wfopen _wfopen
#define MAX_ENV 32767
#define MAX_CMDLINE 32767
#define MAX_OUTPATH 1024

/****************************************************************************/

#define ERRORMSG_NOJAVA \
L"Java 8 or higher could not be detected. To use these tools, a JRE must be \
installed.\n\
\n\
The environment variables JAVA_HOME, JRE_HOME, or JDK_HOME are not set to \
your JRE or JDK directories, nor were Java binaries detected on your PATH.\n\
\n\
For help, visit https://www.java.com/.\n\
\n\
Java can be downloaded from the following places:\n\
\n\
https://www.azul.com/downloads/ (Azul)\n\
https://www.microsoft.com/openjdk (Microsoft)\n\
https://java.com/en/download/ (Oracle)"

#define ERROR_NOJAR           L"FATAL ERROR: Could not find JAR directory, or no suitable JAR files!"
#define ERROR_NOEXEPARENT     L"FATAL ERROR: Could not fetch parent directory of this executable."
#define ERROR_BADCMDPARSE     L"FATAL ERROR: Could not parse command line."
#define ERROR_BADPATHSEARCH   L"INTERNAL ERROR: \"Found\" Java on PATH, but file doesn't EXIST!"
#define ERROR_INVALIDFILE     L"INTERNAL ERROR: An Invalid File Handle was used to search for JARs!"

#ifndef ARG_JAVA_MAIN_CLASS
#define ARG_JAVA_MAIN_CLASS   L"net.mtrop.doom.tools.gui.DoomToolsGUIMain"
#endif

#define ARG_JAVA_CLASSPATH    L"-cp"
#define ARG_JAVA_OPTION0      L"-Xms64M"
#define ARG_JAVA_OPTION1      L"-Xmx768M"

/****************************************************************************/

/**
 * @brief Displays a dialog box.
 * @param wTitle the box title.
 * @param wMessage the message.
 * @param dialogFlags dialog flags.
 */
void Alert(LPCWSTR wTitle, LPCWSTR wMessage, UINT dialogFlags)
{
	MessageBox(NULL, wMessage, wTitle, dialogFlags);
}

/**
 * @brief Show an info dialog.
 * @param message the dialog message.
 */
void ShowInfo(LPCWSTR message)
{
	Alert(L"info", message, MB_OK | MB_ICONINFORMATION);
}

/**
 * @brief Show an error dialog.
 * @param message the dialog message.
 */
void ShowError(LPCWSTR message)
{
	Alert(L"Error", message, MB_OK | MB_ICONERROR);
}

/****************************************************************************/

#define CharsToWide(cp,ch,wch) MultiByteToWideChar((cp), 0, (ch), -1, (wch), (sizeof (wch)) >> 1)
#define ANSIToWide(ch,wch)     CharsToWide(CP_ACP, (ch), (wch))
#define UTF8ToWide(ch,wch)     CharsToWide(CP_UTF8, (ch), (wch))

/****************************************************************************/

/**
 * @brief Get the parent path of this module, null terminated in the target char buffer (if length is less than maxLen).
 * @param target the target buffer.
 * @param maxLen the max amount of characters.
 * @return Amount of bytes returned.
 */
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

/**
 * @brief Checks if a file exists by path.
 * @param wFilePath the input file path.
 * @return TRUE if so, FALSE if not.
 */
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

/**
 * @brief Searches for Java and if found, returns TRUE and wFoundPath contains the path.
 * @param wExeParentPath the parent path of this executable.
 * @param wFoundPath the output buffer for the found Java EXE path.
 * @return TRUE if found, FALSE if not.
 */
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

/**
 * @brief Searches for the most recent JAR file to use.
 * @param wExeParentPath the parent path of this executable.
 * @param wFoundPath if found, the output path to the found file.
 * @return TRUE if found, FALSE if not.
 */
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

/**
 * @brief Calls Java and waits for completion.
 * @param wExePath the executable path/command.
 * @param wToolsPath the DoomTools directory path.
 * @param wArgv argument vector from this EXE (0 is the EXE Path).
 * @param argc argument count from this EXE.
 * @return int the return code.
 */
int CallJava(LPCWSTR wExePath, LPCWSTR wJarPath, LPWSTR *wArgv, int argc)
{
	wchar_t wCmdLineOut[MAX_CMDLINE];

	// TODO: Finish this.

	/*
	BOOL CreateProcessW(
		[in, optional]      LPCWSTR               lpApplicationName,
		[in, out, optional] LPWSTR                lpCommandLine,
		[in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
		[in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,
		[in]                BOOL                  bInheritHandles,
		[in]                DWORD                 dwCreationFlags,
		[in, optional]      LPVOID                lpEnvironment,
		[in, optional]      LPCWSTR               lpCurrentDirectory,
		[in]                LPSTARTUPINFOW        lpStartupInfo,
		[out]               LPPROCESS_INFORMATION lpProcessInformation
	);
	*/

	// TODO: Return result from Java.
	return 0;
}

/****************************************************************************/

// Main Entry.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd) 
{
	wchar_t wExeParentPath[MAX_OUTPATH];
	wchar_t wJavaPath[MAX_OUTPATH];
	wchar_t wJarPath[MAX_OUTPATH];

	LPWSTR *szArgList;
	int argCount;

	if ((szArgList = CommandLineToArgvW(GetCommandLine(), &argCount)) == NULL)
	{
		ShowError(ERROR_BADCMDPARSE);
		return 4;
	}
	
	if (!GetParentPath(wExeParentPath, MAX_OUTPATH))
	{
		LocalFree(szArgList);
		ShowError(ERROR_NOEXEPARENT);
		return 1;
	}

	ShowInfo(wExeParentPath);

	if (!SearchForJava(wExeParentPath, wJavaPath))
	{
		LocalFree(szArgList);
		ShowError(ERRORMSG_NOJAVA);
		return 2;
	}

	ShowInfo(wJavaPath);

	if (!SearchForJar(wExeParentPath, wJarPath))
	{
		LocalFree(szArgList);
		ShowError(ERROR_NOJAR);
		return 3;
	}

	ShowInfo(wJarPath);

	int result = CallJava(wJavaPath, wJarPath, szArgList, argCount);
	LocalFree(szArgList);
	return result;
}

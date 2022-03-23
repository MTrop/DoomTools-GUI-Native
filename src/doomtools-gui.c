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

#include "common/common.h"

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
#define ERROR_CANTSTART       L"FATAL ERROR: Could not create Java process."
#define ERROR_NORESULT        L"FATAL ERROR: Could not get Java process result on exit!"
#define ERROR_INVALIDFILE     L"INTERNAL ERROR: An Invalid File Handle was used to search for JARs!"

#ifndef ARG_JAVA_MAIN_CLASS
#define ARG_JAVA_MAIN_CLASS   L"net.mtrop.doom.tools.gui.DoomToolsGUIMain"
#endif

#define ARG_JAVA_CLASSPATH    L"-cp"
#define ARG_JAVA_MEM_INIT     L"-Xms64M"
#define ARG_JAVA_MEM_MAX      L"-Xmx768M"

/****************************************************************************/

/**
 * @brief Calls Java and waits for completion.
 * @param wJavaExePath the Java executable path/command.
 * @param wWorkingDirPath the working directory path.
 * @param wToolsPath the DoomTools directory path.
 * @param wArgv argument vector from this EXE (0 is the EXE Path).
 * @param argc argument count from this EXE.
 * @return int the return code.
 */
static int CallJava(LPCWSTR wJavaExePath, LPCWSTR wWorkingDirPath, LPCWSTR wJarPath, LPWSTR *wArgv, int argc)
{
	wchar_t wCmdLineOut[MAX_CMDLINE];

	// This is not very safe, but I don't care.
	int cmdLen = 0;
	cmdLen += wsprintf(wCmdLineOut + cmdLen, L"\"%s\"", wJavaExePath);
	cmdLen += wsprintf(wCmdLineOut + cmdLen, L" %s", ARG_JAVA_CLASSPATH);
	cmdLen += wsprintf(wCmdLineOut + cmdLen, L" \"%s\"", wJarPath);
	cmdLen += wsprintf(wCmdLineOut + cmdLen, L" %s", ARG_JAVA_MEM_INIT);
	cmdLen += wsprintf(wCmdLineOut + cmdLen, L" %s", ARG_JAVA_MEM_MAX);
	cmdLen += wsprintf(wCmdLineOut + cmdLen, L" %s", ARG_JAVA_MAIN_CLASS);
	for (int i = 1; i < argc; i++)
		cmdLen += wsprintf(wCmdLineOut + cmdLen, L" \"%s\"", wArgv[i]);

	DWORD flags = CREATE_NO_WINDOW;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	ZeroMemory(&startupInfo, sizeof(startupInfo));

	if (!CreateProcess(wJavaExePath, wCmdLineOut, NULL, NULL, FALSE, flags, NULL, wWorkingDirPath, &startupInfo, &processInfo))
	{
		ShowError(ERROR_CANTSTART);
		return 100;
	}

	DWORD result;
	WaitForSingleObject(processInfo.hProcess, INFINITE);

	BOOL gotCode = GetExitCodeProcess(processInfo.hProcess, &result);
	CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

	if (!gotCode)
	{
		ShowError(ERROR_NORESULT);
		return 100;
	}

	return result;
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

	if (!SearchForJava(wExeParentPath, wJavaPath))
	{
		LocalFree(szArgList);
		ShowError(ERRORMSG_NOJAVA);
		return 2;
	}

	if (!SearchForJar(wExeParentPath, wJarPath))
	{
		LocalFree(szArgList);
		ShowError(ERROR_NOJAR);
		return 3;
	}

	// Set local environment to be carried over to the Java process.
	SetEnvironmentVariable(ENVVAR_DOOMTOOLS_PATH, wExeParentPath);
	SetEnvironmentVariable(ENVVAR_DOOMTOOLS_JAR, wJarPath);

	int result = CallJava(wJavaPath, wExeParentPath, wJarPath, szArgList, argCount);
	LocalFree(szArgList);
	return result;
}

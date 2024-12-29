#pragma once

#include "Base.h"
#include "Primitives.h"
#include "UIManager.h"

class CApplication {
	HINSTANCE Instance;
	HINSTANCE PreviousInstance;
	LPSTR CommandLine;
	Int32 ShowCommand;
	HANDLE ConsoleThread;
	HWND Window;
	RECT WindowRect;
	Bool IsRunning;

	CFileManager* FileManager;
	CUIManager* UIManager;
	CLocalization* Localization;
	CRenderer* Renderer;
	SocketRef LoginSocket;

public:
	String ApplicationDirectory;
	String ResourceDirectory;
	String LanguageDirectory;

	CApplication();

	~CApplication();

	CLocalization* GetLocalization();

	CUIRect GetViewport();

	Int32 Run(
		HINSTANCE Instance,
		HINSTANCE PreviousInstance,
		LPSTR CommandLine,
		Int32 ShowCommand
	);

	Void Exit();
};

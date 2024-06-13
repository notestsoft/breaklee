#include "Application.h"
#include "Archive.h"
#include "FileManager.h"
#include "Localization.h"
#include "Renderer.h"
#include "UIManager.h"

typedef DWORD WINAPI XInputGetStateProc(DWORD, XINPUT_STATE*);

DWORD WINAPI XInputGetStateStub(DWORD, XINPUT_STATE*) { return 0; }

static XInputGetStateProc* InputGetState = XInputGetStateStub;

Void LoadXInputLibrary() {
	HMODULE Library = LoadLibrary(XINPUT_DLL);
	if (!Library) {
		Fatal("Failed to load xinput library!");
	}

	InputGetState = (XInputGetStateProc*)GetProcAddress(Library, "XInputGetState");
}

LRESULT CALLBACK CApplicationWindowProc(
	HWND Window,
	UINT Message,
	WPARAM WParam,
	LPARAM LParam
) {
	if (Message == WM_SIZE) {
		RECT Frame;
		if (!GetClientRect(Window, &Frame)) return 0;

		Int32 Width = Frame.right - Frame.left;
		Int32 Height = Frame.bottom - Frame.top;
	}

	if (Message == WM_DESTROY) {
		PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}

	return DefWindowProc(Window, Message, WParam, LParam);
}

DWORD WINAPI CApplicationConsoleThread(
	LPVOID Param
) {
	AllocConsole();

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	while (true) {
		Sleep(100);
	}

	FreeConsole();
	return 0;
}

CApplication::CApplication() {
	EncryptionLoadLibrary();
	LoadXInputLibrary();

	Char Directory[MAX_PATH] = { 0 };
	PathGetCurrentDirectory(Directory, sizeof(Directory));
	this->ApplicationDirectory = std::string(Directory);
	this->ResourceDirectory = std::string(Directory).append("\\Data");
	this->LanguageDirectory = std::string(this->ResourceDirectory).append("\\Language\\Korean");
}

CApplication::~CApplication() {
	// Called for memory ? Now is your last chance to clear everything up, 
	// because the program is about to end.
	// Also, DirectX calls for various interfaces to be shut down or �released�.
	// These are all done here.

	delete this->Renderer;
	delete this->UIManager;
	delete this->FileManager;
	delete this->Localization;

	if (this->ConsoleThread) {
		CloseHandle(this->ConsoleThread);
	}

	if (this->Window) {
		CloseWindow(this->Window);
	}

	EncryptionUnloadLibrary();
}

CLocalization* CApplication::GetLocalization() {
	return this->Localization;
}

CUIRect CApplication::GetViewport() {
	return CUIRect(
		this->WindowRect.left,
		this->WindowRect.top,
		this->WindowRect.right - this->WindowRect.left,
		this->WindowRect.bottom - this->WindowRect.top
	);
}

Int32 CApplication::Run(
	HINSTANCE Instance,
	HINSTANCE PreviousInstance,
	LPSTR CommandLine,
	Int32 ShowCommand
) {
	this->Instance = Instance;
	this->PreviousInstance = PreviousInstance;
	this->CommandLine = CommandLine;
	this->ShowCommand = ShowCommand;

	// Create Window
	WNDCLASSEX WindowClass;
	ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = CApplicationWindowProc;
	WindowClass.hInstance = this->Instance;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.lpszClassName = "CApplicationWindowClass";

	if (!RegisterClassEx(&WindowClass)) {
		Fatal("Window creation failed!");
	}

	this->WindowRect = { 0, 0, 1024, 768 };
	this->Window = CreateWindowEx(
		NULL,
		WindowClass.lpszClassName,
		"Breaklee Client",
		WS_OVERLAPPEDWINDOW,
		this->WindowRect.left,
		this->WindowRect.top,
		this->WindowRect.right - this->WindowRect.left,
		this->WindowRect.bottom - this->WindowRect.top,
		NULL,
		NULL,
		this->Instance,
		NULL
	);

	if (!this->Window) {
		Fatal("Window creation failed!");
	}

	// Create Console
	this->ConsoleThread = CreateThread(nullptr, 0, CApplicationConsoleThread, nullptr, 0, nullptr);

	// Create Runtime
	this->FileManager = new CFileManager(this);
	this->UIManager = new CUIManager(this);
	this->Renderer = new CRenderer(this->Window);
	this->Localization = new CLocalization();

	// This part is basically setting up your game to be played.
	// This is where you might select a map, set the location 
	// of your player(and anyone else in your game world) or 
	// set random values, such as random terrain, that are 
	// decided at the beginning of the game only.After all 
	// of this, you enter the game loop itself for the kickoff.

	auto Archive = this->FileManager->LoadArchive(this->LanguageDirectory, "msg.enc");
	for (auto Iterator = Archive->QueryBegin("cabal_message.msg_pool.word"); Iterator != Archive->QueryEnd(); ++Iterator) {
		auto Key = Iterator.GetAttribute("id");
		auto Value = Iterator.GetAttribute("cont");
		this->Localization->InsertWord(Key, Value);
	}

	// TODO: Load game data here...
	if (!this->UIManager->LoadData(this->FileManager, this->Renderer)) Fatal("Loading ui data failed!");

	ShowWindow(this->Window, this->ShowCommand);
	ShowCursor(FALSE);
	memset(this->UIManager->InputState.KeyStates, 0, sizeof(this->UIManager->InputState.KeyStates));

	// Core Game Loop
	this->IsRunning = true;
	while (this->IsRunning) {
		POINT MousePosition;
		GetCursorPos(&MousePosition);
		ScreenToClient(this->Window, &MousePosition);

		CUIRect Viewport = this->GetViewport();
		CUIRect Bounds = this->Renderer->GetBounds();
		Size Resolution = { 1024, 768 };
		Float32 ScaleX = 1.0f * Resolution.Width / (Viewport.Right - Viewport.Left);
		Float32 ScaleY = 1.0f * Resolution.Height / (Viewport.Bottom - Viewport.Top);
		this->UIManager->InputState.MouseX = ScaleX * MousePosition.x;
		this->UIManager->InputState.MouseY = ScaleY * MousePosition.y;

		// Here, you get all the information from the keyboard, 
		// mouse, joystick, controller or whatever device your 
		// player is using.
		// This phase is mostly covered in the DirectInput tutorial.
		
		MSG Message;
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);

			if (Message.message == WM_QUIT) {
				this->IsRunning = false;
			}

			if (Message.message == WM_SIZE) {
				GetWindowRect(this->Window, &this->WindowRect);
			}

			if (Message.message == WM_LBUTTONDOWN) {
				this->UIManager->InputState.KeyStates[VK_LBUTTON] = true;
			}

			if (Message.message == WM_LBUTTONUP) {
				this->UIManager->InputState.KeyStates[VK_LBUTTON] = false;
			}

			if (Message.message == WM_RBUTTONDOWN) {
				this->UIManager->InputState.KeyStates[VK_RBUTTON] = true;
			}

			if (Message.message == WM_RBUTTONUP) {
				this->UIManager->InputState.KeyStates[VK_RBUTTON] = false;
			}

			if (Message.message == WM_SYSKEYDOWN ||
				Message.message == WM_SYSKEYUP ||
				Message.message == WM_KEYUP ||
				Message.message == WM_KEYDOWN) {
				UInt32 KeyCode = (UInt32)Message.wParam;
				Bool IsKeyDown = (((UInt32)Message.lParam) & (1 << 31)) == 0;
				Bool IsKeyHeld = IsKeyDown && (((UInt32)Message.lParam) & (1 << 30)) > 0;
				this->UIManager->InputState.KeyStates[KeyCode] = IsKeyDown;
			}
		}

		for (auto Index = 0; Index < XUSER_MAX_COUNT; Index++) {
			XINPUT_STATE State;
			ZeroMemory(&State, sizeof(XINPUT_STATE));

			if (InputGetState(Index, &State) == ERROR_SUCCESS) {
				// Controller is connected, read input events
			}
			else {
				// Controller is not connected
			}
		}

		if (!this->IsRunning) break;

		// In this part you process what is going on in your world.
		// Where exactly did your player move in the last sixtieth 
		// of a second ? How much ammo does he have left ? 
		// Are his enemies coming or going ? What about his allies ? 
		// Did his ship crash into a mountain ? 
		// Or was that just a cloud he just ran into ? 
		// All this and more is determined in this phase.
		// I don�t have any tutorials on these yet, 
		// but I hope to get them up well before the end of the year, so stay tuned.

		// TODO: UpdateLogic here...
		this->UIManager->Update();

		// This is where DirectX is mostly used. Here you would process all your 3D (and 2D) 
		// graphics and render them to the screen. This will be gone over in the Direct3D tutorials.
		// this->Renderer->Render();

		this->Renderer->BeginSceneUI();
		this->UIManager->Render(this->Renderer);
		this->Renderer->EndSceneUI();

		Sleep(20);
	}

	return EXIT_SUCCESS;
}

Void CApplication::Exit() {
	this->IsRunning = false;
}

Int32 WINAPI WinMain(
	_In_ HINSTANCE Instance,
	_In_opt_ HINSTANCE PreviousInstance,
	_In_ LPSTR CommandLine,
	_In_ Int32 ShowCommand
) {
	chdir("C:\\CABAL\\");
	// TODO: Create new diagnostic system
	DiagnosticSetup(stdout, LOG_LEVEL_TRACE, NULL, NULL);
	
	return CApplication().Run(
		Instance, 
		PreviousInstance, 
		CommandLine, 
		ShowCommand
	);
}
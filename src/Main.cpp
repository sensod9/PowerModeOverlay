#include <cstdio>
#include <windows.h>
#include <string>
#include "../include/ConfigParser.hpp"
#include "../include/StringToVKey.hpp"

int MAX_POWER = 5000;
int CONSUMPTION_INTERVAL = 15;

std::string enableKeybindStr = "F6";
std::string restartKeybindStr = "R";
std::string toggleKeybindStr = "Space";
std::string shutdownKeybindStr = "F8";

unsigned int ENABLE_KEYBIND = StringToVKey(enableKeybindStr);
unsigned int RESTART_KEYBIND = StringToVKey(restartKeybindStr);
unsigned int TOGGLE_KEYBIND = StringToVKey(toggleKeybindStr);
unsigned int SHUTDOWN_KEYBIND = StringToVKey(shutdownKeybindStr);

enum Phases {
	Disabled,
	Started,
	Active,
	Ended
};
enum Phases phase = Disabled;

HHOOK hKeyHook = nullptr;
HHOOK hMouseHook = nullptr;
HWND hOverlayWnd = nullptr;
HWND hPowerDisplayWnd = nullptr;

int power = MAX_POWER;
bool active = false; // is overlay enabled
bool covered = false; // is screen covered in black

LRESULT LLKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* pHookStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
			if (pHookStruct->vkCode == ENABLE_KEYBIND || pHookStruct->vkCode == RESTART_KEYBIND || pHookStruct->vkCode == TOGGLE_KEYBIND) {
				PostMessage(hOverlayWnd, WM_APP + 100, pHookStruct->vkCode, 0);			
			}
			else if (pHookStruct->vkCode == SHUTDOWN_KEYBIND) {
				PostMessage(hOverlayWnd, WM_DESTROY, 0, 0);			
			}
		}
	}
	return CallNextHookEx(hKeyHook, code, wParam, lParam);
}

LRESULT LLMouseProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION) {
		if (wParam == WM_LBUTTONDOWN && (ENABLE_KEYBIND == VK_LBUTTON || RESTART_KEYBIND == VK_LBUTTON || TOGGLE_KEYBIND == VK_LBUTTON)) {
			PostMessage(hOverlayWnd, WM_APP + 100, VK_LBUTTON, 0);			
		}
		else if (wParam == WM_LBUTTONDOWN && VK_LBUTTON == SHUTDOWN_KEYBIND) {
			PostMessage(hOverlayWnd, WM_DESTROY, 0, 0);			
		}
		else if (wParam == WM_RBUTTONDOWN && (ENABLE_KEYBIND == VK_RBUTTON || RESTART_KEYBIND == VK_RBUTTON || TOGGLE_KEYBIND == VK_RBUTTON)) {
			PostMessage(hOverlayWnd, WM_APP + 100, VK_RBUTTON, 0);			
		}
		else if (wParam == WM_RBUTTONDOWN && VK_RBUTTON == SHUTDOWN_KEYBIND) {
			PostMessage(hOverlayWnd, WM_DESTROY, 0, 0);			
		}
	}
	return CallNextHookEx(hMouseHook, code, wParam, lParam);
}

void RedrawPowerDisplay()
{
	InvalidateRect(hPowerDisplayWnd, nullptr, TRUE);
	UpdateWindow(hPowerDisplayWnd);
}

void CoverScreen()
{
	SetLayeredWindowAttributes(hOverlayWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
}

void UncoverScreen()
{
	SetLayeredWindowAttributes(hOverlayWnd, RGB(0, 0, 0), 0, LWA_ALPHA);
}

void WINAPI TimerProc(HWND hWnd, UINT uMSG, UINT_PTR idEvent, DWORD dwTime)
{
	if (active && !covered && phase == Active)
	{
		if (power > 1) {
			power--;
			RedrawPowerDisplay();
		}
		else if (power == 1) {
			power--;
			RedrawPowerDisplay();
			CoverScreen();
			covered = true;
		}
	}
}

LRESULT PowerDisplayProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect{0, 0, 385, 65};
			GetClientRect(hWnd, &rect);
			FillRect(hdc, &rect, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

			if (phase == Ended) {
				SetTextColor(hdc, RGB(0, 230, 0));
			}
			else {
				SetTextColor(hdc, RGB(255, 255, 255));
			}
			SetBkColor(hdc, RGB(0, 0, 0));

			HFONT hFont;
			char text[256];
			if (phase != Disabled) {
				hFont = CreateFont(100, 0, 0, 0, FW_BOLD, 
					FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
					OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
					VARIABLE_PITCH, "Consolas");
				sprintf_s(text, 256, "%.1f%%", static_cast<float>(power)/10);
				SelectObject(hdc, hFont);
				DrawTextEx(hdc, const_cast<char*>(text), -1, &rect, DT_SINGLELINE, nullptr);
			}
			else {
				hFont = CreateFont(20, 0, 0, 0, FW_BOLD, 
					FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
					OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
					VARIABLE_PITCH, "Consolas");
				sprintf_s(text, 256,
					"'%s': Enable overlay / Finish night\n"
					"'%s': (Re)start night when overlay is enabled\n"
					"'%s': Toggle visibility\n"
					"'%s': Shut down overlay\n"
					"how to use and customize keybinds:\n"
					"github.com/sensod9/PowerModeOverlay   v1.0.1", enableKeybindStr.c_str(), restartKeybindStr.c_str(), toggleKeybindStr.c_str(), shutdownKeybindStr.c_str());
				SelectObject(hdc, hFont);
				DrawTextEx(hdc, const_cast<char*>(text), -1, &rect, NULL, nullptr);
			}

			DeleteObject(hFont);
			EndPaint(hWnd, &ps);;
			return 0;
		}
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT OverlayProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_APP + 100:
		{
			DWORD vkCode = static_cast<DWORD>(wParam);
			if (vkCode == ENABLE_KEYBIND) // Toggle Overlay (`F6`)
			{
				active = !active;
				if (!active) {
					UncoverScreen();
					covered = false;
					if (phase == Active) {
						phase = Ended;
						RedrawPowerDisplay();
					}
					power = MAX_POWER;
					phase = Ended;
				}
				else {
					phase = Started;
					RedrawPowerDisplay();
				}
			}
			else if (vkCode == RESTART_KEYBIND) // Restart Night (`R`)
			{
				if (active || phase == Ended) {
					if (phase == Started || phase == Ended) {
						if (phase == Ended) {
							active = true;
						}
						phase = Active;
					}
					CoverScreen();
					covered = true;
					power = MAX_POWER;
					RedrawPowerDisplay();
				}
			}
			else if (vkCode == TOGGLE_KEYBIND)
			{
				if (active && power > 0 && phase == Active) {
					covered = !covered;
					if (covered) {
						CoverScreen();
					}
					else {
						UncoverScreen();
					}
				}
			}
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	ConfigParser configParser;
	if (configParser.load("pmode.cfg")) {
		MAX_POWER = configParser.getInt("amount", MAX_POWER/10)*10;
		CONSUMPTION_INTERVAL = configParser.getInt("consumption", CONSUMPTION_INTERVAL);

		std::string keybindStr;
		keybindStr = configParser.getString("enable");
		if (UINT keybind = StringToVKey(keybindStr); keybind) {
			ENABLE_KEYBIND = keybind;
			enableKeybindStr = keybindStr;
		}

		keybindStr = configParser.getString("restart");
		if (UINT keybind = StringToVKey(keybindStr); keybind) {
			RESTART_KEYBIND = keybind;
			restartKeybindStr = keybindStr;
		}

		keybindStr = configParser.getString("toggle");
		if (UINT keybind = StringToVKey(keybindStr); keybind) {
			TOGGLE_KEYBIND = keybind;
			toggleKeybindStr = keybindStr;
		}

		keybindStr = configParser.getString("shutdown");
		if (UINT keybind = StringToVKey(keybindStr); keybind) {
			SHUTDOWN_KEYBIND = keybind;
			shutdownKeybindStr = keybindStr;
		}
		
		power = MAX_POWER;
	}

	WNDCLASSEX wc = {sizeof(WNDCLASSEX)};
	wc.lpfnWndProc = OverlayProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "OverlayClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, "An error occured during class registration", "Error", MB_ICONERROR);
		return 1;
	}

	WNDCLASSEX wcPowerDisplay = {sizeof(WNDCLASSEX)};
	wcPowerDisplay.lpfnWndProc = PowerDisplayProc;
	wcPowerDisplay.hInstance = hInstance;
	wcPowerDisplay.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcPowerDisplay.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcPowerDisplay.lpszClassName = "PowerDisplayClass";
	if (!RegisterClassEx(&wcPowerDisplay)) {
		MessageBox(nullptr, "An error occured during class registration", "Error", MB_ICONERROR);
		return 1;
	}
	
	if (hOverlayWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED, "OverlayClass", "Power Mode Overlay", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInstance, nullptr); !hOverlayWnd) {
		MessageBox(nullptr, "An error occured while creating the window", "Error", MB_ICONERROR);
		return 1;
	}

	if (hPowerDisplayWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED, "PowerDisplayClass", "Power Display", WS_POPUP | WS_VISIBLE, 50, 20, 450, 120, nullptr, nullptr, hInstance, nullptr); !hPowerDisplayWnd) {
		MessageBox(nullptr, "An error occured while creating the window", "Error", MB_ICONERROR);
		return 1;
	}

	SetLayeredWindowAttributes(hOverlayWnd, RGB(0, 0, 0), 0, LWA_ALPHA);
	SetWindowPos(hOverlayWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	
	SetLayeredWindowAttributes(hPowerDisplayWnd, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
	SetWindowPos(hPowerDisplayWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyboardProc, hInstance, 0);
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LLMouseProc, hInstance, 0);
	
	UINT_PTR timerId = SetTimer(nullptr, 0, CONSUMPTION_INTERVAL, TimerProc);
	
	MSG msg{};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (hKeyHook) UnhookWindowsHookEx(hKeyHook);
	if (hMouseHook) UnhookWindowsHookEx(hMouseHook);
	KillTimer(nullptr, timerId);
	return static_cast<int>(msg.wParam);
}

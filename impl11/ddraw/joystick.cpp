// Copyright (c) 2016-2018 Reimar Döffinger
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"
#include "common.h"

#include <mmsystem.h>
#include <xinput.h>
#include <windows.h>

#include "joystick.h"

#pragma comment(lib, "winmm")
#pragma comment(lib, "XInput9_1_0")

#undef min
#undef max
#include <algorithm>

// --- MOUSE WHEEL HOOK LOGIC ---
static HHOOK hMouseHook = NULL;

void SendKey(WORD vKey) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vKey;
    SendInput(1, &input, sizeof(INPUT));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        short wheelDelta = HIWORD(pMouseStruct->mouseData);

        if (g_config.MouseScrollWheelBind > 0) {
            WORD keyUp = 0, keyDown = 0;
            switch (g_config.MouseScrollWheelBind) {
                case 1: keyUp = VK_BACK; keyDown = 0xDB; break; // Backspace / [
                case 2: keyUp = VK_BACK; keyDown = VK_RETURN; break; // Backspace / Enter
                case 3: keyUp = 0x30;    keyDown = 0x39; break; // 0 / 9
            }
            if (wheelDelta > 0 && keyUp != 0) SendKey(keyUp);
            else if (wheelDelta < 0 && keyDown != 0) SendKey(keyDown);
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

// --- CORE FUNCTIONS ---

DWORD emulGetTime() {
    static DWORD oldtime;
    static DWORD count;
    DWORD time = timeGetTime();
    if (time != oldtime) { oldtime = time; count = 0; }
    if (++count >= 20) { Sleep(2); time = timeGetTime(); count = 0; }
    return time;
}

static int needsJoyEmul() {
    JOYCAPS caps = {};
    if (joyGetDevCaps(0, &caps, sizeof(caps)) != JOYERR_NOERROR ||
        !(caps.wCaps & JOYCAPS_HASZ) || caps.wNumAxes <= 2 ||
        caps.wMid == 0x45e) {
        XINPUT_STATE state;
        if (XInputGetState(0, &state) == ERROR_SUCCESS) return 2;
    }
    return 1;
}

UINT WINAPI emulJoyGetNumDevs(void) {
    if (g_config.JoystickEmul < 0) g_config.JoystickEmul = needsJoyEmul();
    return 1;
}

static UINT joyYmax, joyZmax;

UINT WINAPI emulJoyGetDevCaps(UINT_PTR joy, struct tagJOYCAPSA *pjc, UINT size) {
    if (!g_config.JoystickEmul) return joyGetPosEx(joy, pjc, size);
    if (joy != 0) return MMSYSERR_NODRIVER;
    memset(pjc, 0, size);
    pjc->wXmax = 512; pjc->wYmax = 512;
    pjc->wNumButtons = 5; pjc->wNumAxes = 2;
    return JOYERR_NOERROR;
}

static DWORD lastGetPos;

UINT WINAPI emulJoyGetPosEx(UINT joy, struct joyinfoex_tag *pji) {
    if (!g_config.JoystickEmul) return joyGetPosEx(joy, pji);

    if (hMouseHook == NULL && g_config.MouseScrollWheelBind > 0) {
        hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
    }

    static bool relativeActive = true;
    static bool rCtrlWasDown = false;
    bool rCtrlDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    if (rCtrlDown && !rCtrlWasDown) relativeActive = !relativeActive;
    rCtrlWasDown = rCtrlDown;

    DWORD now = GetTickCount();
    static int centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
    static int centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

    // Safety snap ONLY if relative is active
    if (relativeActive && g_config.RelativeMouse && (now - lastGetPos) > 5000) {
        SetCursorPos(centerX, centerY);
    }
    lastGetPos = now;

    POINT pos;
    GetCursorPos(&pos);

    float mouseX = 256.0f;
    float mouseY = 256.0f;

    if (g_config.RelativeMouse && relativeActive) {
        mouseX = 256.0f + (pos.x - (float)centerX) * g_config.MouseSensitivity;
        mouseY = 256.0f + (pos.y - (float)centerY) * g_config.MouseSensitivity;
        SetCursorPos(centerX, centerY);
    } else {
        mouseX = 256.0f + (pos.x - (float)centerX) * g_config.MouseSensitivity;
        mouseY = 256.0f + (pos.y - (float)centerY) * g_config.MouseSensitivity;
    }

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)  mouseX = 256.0f - 256.0f * g_config.KbdSensitivity;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) mouseX = 256.0f + 256.0f * g_config.KbdSensitivity;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)  mouseY = 256.0f + 256.0f * g_config.KbdSensitivity; 
    if (GetAsyncKeyState(VK_UP) & 0x8000)    mouseY = 256.0f - 256.0f * g_config.KbdSensitivity; 

    pji->dwXpos = (DWORD)std::min(std::max(mouseX, 0.0f), 512.0f);
    pji->dwYpos = (DWORD)std::min(std::max(mouseY, 0.0f), 512.0f);

    pji->dwButtons = 0;
    pji->dwButtonNumber = 0;
    if (GetAsyncKeyState(VK_LBUTTON)) { pji->dwButtons |= 1; pji->dwButtonNumber++; }
    if (GetAsyncKeyState(VK_RBUTTON)) { pji->dwButtons |= 2; pji->dwButtonNumber++; }
    if (GetAsyncKeyState(VK_MBUTTON)) { pji->dwButtons |= 4; pji->dwButtonNumber++; }

    if (g_config.InvertYAxis) pji->dwYpos = 512 - pji->dwYpos;
    
    return JOYERR_NOERROR;
}

/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/
#include "precomp.h"

// This class is intended to test restrictions placed on APIs from within a UWP application context
class PolicyTests
{
    TEST_CLASS(PolicyTests);
        
    BEGIN_TEST_METHOD(WrongWayVerbsUAP)
        TEST_METHOD_PROPERTY(L"RunAs", L"UAP")
    END_TEST_METHOD();

    BEGIN_TEST_METHOD(WrongWayVerbsUser)
        TEST_METHOD_PROPERTY(L"RunAs", L"User")
    END_TEST_METHOD();
};

void DoWrongWayVerbTest(_In_ BOOL bResultExpected, _In_ DWORD dwStatusExpected)
{
    DWORD dwResult;
    BOOL bResultActual;

    // Try to read the output buffer
    {
        Log::Comment(L"Read the output buffer using string commands.");
        {
            wchar_t pwsz[50];
            char psz[50];
            COORD coord = { 0 };

            SetLastError(0);
            bResultActual = ReadConsoleOutputCharacterW(GetStdOutputHandle(), pwsz, ARRAYSIZE(pwsz), coord, &dwResult);
            VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
            VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());

            SetLastError(0);
            bResultActual = ReadConsoleOutputCharacterA(GetStdOutputHandle(), psz, ARRAYSIZE(psz), coord, &dwResult);
            VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
            VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());

            SetLastError(0);
            bResultActual = ReadConsoleOutputAttribute(GetStdOutputHandle(), pwsz, ARRAYSIZE(pwsz), coord, &dwResult);
            VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
            VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());
        }

        Log::Comment(L"Read the output buffer using CHAR_INFO commands.");
        {
            CHAR_INFO pci[50];
            COORD coordPos = { 0 };
            COORD coordPci;
            coordPci.X = 50;
            coordPci.Y = 1;
            SMALL_RECT srPci;
            srPci.Top = 1;
            srPci.Bottom = 1;
            srPci.Left = 1;
            srPci.Right = 50;

            SetLastError(0);
            bResultActual = ReadConsoleOutputW(GetStdOutputHandle(), pci, coordPci, coordPos, &srPci);
            VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
            VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());

            SetLastError(0);
            bResultActual = ReadConsoleOutputA(GetStdOutputHandle(), pci, coordPci, coordPos, &srPci);
            VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
            VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());
        }
    }

    // Try to write the input buffer
    Log::Comment(L"Write the input buffer using INPUT_RECORD commands.");
    {
        INPUT_RECORD ir[2];
        ir[0].EventType = KEY_EVENT;
        ir[0].Event.KeyEvent.bKeyDown = TRUE;
        ir[0].Event.KeyEvent.dwControlKeyState = 0;
        ir[0].Event.KeyEvent.uChar.UnicodeChar = L'@';
        ir[0].Event.KeyEvent.wRepeatCount = 1;
        ir[0].Event.KeyEvent.wVirtualKeyCode = ir[0].Event.KeyEvent.uChar.UnicodeChar;
        ir[0].Event.KeyEvent.wVirtualScanCode = static_cast<WORD>(MapVirtualKeyW(ir[0].Event.KeyEvent.wVirtualKeyCode, MAPVK_VK_TO_VSC));
        ir[1] = ir[0];
        ir[1].Event.KeyEvent.bKeyDown = FALSE;

        DWORD dwResult;

        SetLastError(0);
        bResultActual = WriteConsoleInputW(GetStdInputHandle(), ir, ARRAYSIZE(ir), &dwResult);
        VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
        VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());

        SetLastError(0);
        bResultActual = WriteConsoleInputA(GetStdInputHandle(), ir, ARRAYSIZE(ir), &dwResult);
        VERIFY_ARE_EQUAL(bResultExpected, bResultActual);
        VERIFY_ARE_EQUAL(dwStatusExpected, GetLastError());
    }
}

void PolicyTests::WrongWayVerbsUAP()
{
    Log::Comment(L"From the UAP environment, these functions should be access denied.");
    DoWrongWayVerbTest(FALSE, ERROR_ACCESS_DENIED);
}

void PolicyTests::WrongWayVerbsUser()
{
    Log::Comment(L"From the classic uer environment, these functions should return with a normal status code.");
    DoWrongWayVerbTest(TRUE, ERROR_SUCCESS);
}

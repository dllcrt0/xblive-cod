#include "stdafx.h"

void ConstantLoop() {
	xbLive.Initialize();

#ifdef DUMP_PATCH_DATA
	xbLive.pPatches->DumpPatches();
#endif

	if (Native::XamGetCurrentTitleId() != GAME_TITLE) {
		LOG_DEV("Unsupported title!");
		xbLive.bTerminateProcess = true;
		goto jUnload;
	}

	if (!pSystemHooks.Apply()) {
		LOG_DEV("Failed to apply system hooks!");
		xbLive.bTerminateProcess = true;
		goto jUnload;
	}

	if (pUtils.MountPath(MOUNT_PATH, pUtils.GetMountPath(), FALSE) == 0) {
		LOG_DEV("Mounted path successfully!");
	}

	LOG_DEV("Started!");

#ifdef RELEASE_VERSION
	if (XexExecutableModuleHandle) {
		auto title = *XexExecutableModuleHandle;
		if (title) {
			auto data = Native::GetCheatPatches(Native::XamGetCurrentTitleId(), title->TimeDateStamp);

			if (data) {
				memcpy(&xbLive.pPatches->BO3, data + 4, *(DWORD*)((DWORD)data));
				xbLive.pPatches->bHasPatches = true;
			}
		}
	}
#endif

	if (xbLive.pPatches->bHasPatches) {
		while (!xbLive.bTerminateProcess) {
			if (Native::XamGetCurrentTitleId() == GAME_TITLE && !xbLive.bShutdownCheats) {
				if (!pGame.bInitialized) {
					if (!pGame.Initialize()) {
						Sleep(1000);
					}
				} else {
					pGame.Run();
				}
			} else {
				LOG_DEV("Changing title! Shutdown initiated!");
				break;
			}
			Sleep(50);
		}

		if (pGame.bInitialized) {
			pGame.Shutdown();
		}
	}

jUnload:
	if (pSystemHooks.bInitialized)
		pSystemHooks.Remove();

	xbLive.Shutdown();

	LOG_DEV("Exiting!");

	xbLive.bConstantLoopRunning = false;

	if (!xbLive.bExternUnload) {
		*(WORD*)((DWORD)xbLive.hMyModule + 64) = 1;
		DWORD exitCode;
		GetExitCodeThread(GetCurrentThread(), &exitCode);
		XexUnloadImageAndExitThread(xbLive.hMyModule, (HANDLE)exitCode);
	}
}

HRESULT HiddenThreadStartup() {
	HANDLE hThread; DWORD hThreadID;
	
	if (ExCreateThread(&hThread, 0, &hThreadID, (PVOID)XapiThreadStartup, (LPTHREAD_START_ROUTINE)ConstantLoop, NULL, 0x1c000427) != S_OK) {
		return E_FAIL;
	}

	xbLive.bConstantLoopRunning = true;

	XSetThreadProcessor(hThread, 4);
	ResumeThread(hThread);
	CloseHandle(hThread);

	return S_OK;
}

HRESULT Initialize() {
	if (FAILED(Invoker::Initialize())) {
		LOG_DEV("Invoker failed!");
		return E_FAIL;
	}

#ifdef RELEASE_VERSION
	BYTE* bullshit = new BYTE[0x20];
	memset(bullshit, 0, 0x20);

	*(BYTE*)(bullshit) = 'x';
	*(BYTE*)(bullshit + 1) = 'b';
	*(BYTE*)(bullshit + 2) = 'd';
	*(BYTE*)(bullshit + 3) = 'm';
	*(DWORD*)(bullshit + 4) = (DWORD)HiddenThreadStartup;
	*(DWORD*)(bullshit + 8) = 0x90f00000;
	LDR_DATA_TABLE_ENTRY* entry = (LDR_DATA_TABLE_ENTRY*)&bullshit[12];

	// this call secretly initializes our thread
	XexPcToFileHeader(bullshit, &entry);

	// this never gets past, as we'll make entry a nullptr
	if (entry) {
		if (entry->ImageBase == (void*)0x90f00000) {
			if (entry->Flags & 1) {
				if (entry->LoadCount == 3) {
					DWORD joker = entry->CheckSum ^ 1337;
					if (joker < 0x1000) {
						DbgPrint("DBG: Secret AES key is bad!\n");
					} else {
						XECRYPT_AES_STATE state;
						XeCryptAesKey(&state, bullshit);
						XeCryptAesCbc(&state, (BYTE*)(0x90f00000 + 0x2000), 0x10, 0, 0, FALSE);
					}
				}
			}
		}
	}

	return S_OK;
#else
	return HiddenThreadStartup();
#endif
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH: {
			srand(time(0));
			xbLive.hMyModule = hModule;
			xbLive.bDevkit = *(int*)0x8E038610 & 0x8000 ? false : true;

			if (FAILED(Initialize())) {
				*(WORD*)((DWORD)hModule + 64) = 1;
				return FALSE;
			}

			return TRUE;
		}

		case DLL_PROCESS_DETACH: {
			xbLive.bExternUnload = true;
			xbLive.bTerminateProcess = true;

			while (xbLive.bConstantLoopRunning) {
				Sleep(50);
			}

			LOG_PRINT("%s engine unloaded!", GAME_NAME);
			return TRUE;
		}
	}

	return FALSE;
}
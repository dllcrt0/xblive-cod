#pragma once

void Terminate();

class Global {
public:
	bool bDevkit;
	bool bExternUnload;
	bool bTerminateProcess;
	bool bConstantLoopRunning;
	bool bGameInitialized;
	bool bShutdownCheats;

	HANDLE hMyModule;

	Cheats* pPatches;
	Patches* GetPatch() { return &pPatches->BO3; }

	HRESULT Initialize();
	void Shutdown();
};

extern Global xbLive;
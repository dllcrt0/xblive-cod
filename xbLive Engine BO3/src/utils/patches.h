#pragma once

struct Patches {
	DWORD dwVar;
};

class Cheats {
public:
	bool bHasPatches;
	Patches BO3;

	HRESULT Apply(PVOID data) {
#ifndef RELEASE_VERSION
		// set ur vars here

		bHasPatches = true;
#else
		if (data) {
			memcpy(&BO3, data, sizeof(Patches));
			bHasPatches = true;
			return S_OK;
		} else {
			bHasPatches = FALSE;
			return E_FAIL;
		}
#endif
		return S_OK;
	}

	void Initialize() {
		bHasPatches = false;

#ifndef RELEASE_VERSION
		ZeroMemory(&BO3, sizeof(Patches));
		Apply(NULL);
		bHasPatches = true;
#endif
	}

#ifdef DUMP_PATCH_DATA
	void DumpPatches();
#endif
};
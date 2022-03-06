#include "stdafx.h"

Invoker pInvoker;

DWORD* Invoker::Call(DWORD NativeAddress, NativeManager* Context) {
	DWORD* result = nullptr;
	NativeHandler handler = GetNativeHandler(NativeAddress);
	if (handler) {
		__try {
			handler(Context);
		} __except (true) {}
		result = (DWORD*)Context->GetResultPointer();
	}

	return result;
}

#pragma optimize( "", off )
NativeHandler Invoker::GetNativeHandler(DWORD NativeAddress) {
	NativeHandler Handler = nullptr;
	NativeRegistration* Registration = &NativeTable[NativeAddress & 0xFF];

	if (Registration != nullptr) {
		for (; Registration; Registration = Registration->pNext) {
			for (int i = 0; i < Registration->GetCount(); i++) {
				if (Registration->RandomStuff(1) != 0xFFFF) { // fattest meme
					if (NativeAddress == Registration->GetAddress(i)) {
						Handler = Registration->GetFunction(i);
						break;
					}
				}
			}
		}
	}

	return Handler;
}
#pragma optimize( "", on )

HRESULT Invoker::Initialize() {
	pCodCheatInterface = Interface::GetInterface<CodCheatInterface>(0x13371336); 
	
	if (pCodCheatInterface) {
		pInvoker.SetTable(pCodCheatInterface->GetRegistration());
	}

	LOG_DEV("Native table: %X", pInvoker.GetTable());

	return S_OK;
}
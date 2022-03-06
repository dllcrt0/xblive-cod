#include "stdafx.h"
#include "menu/submenus/visual/crosshairMenu.h"
#include "menu/submenus/visual/espMenu.h"
#include "menu/submenus/visual/indicatorMenu.h"

Title pTitle;

typedef void(*tMenu_PaintAll)(DWORD r3, UIContext* pContext);
tMenu_PaintAll OriginalMenu_PaintAll;
void Menu_PaintAllHook(DWORD r3, UIContext* r4) {
	VisualMenu::GetInstance()->FeatureUpdatePaintAll();
	CrosshairMenu::GetInstance()->FeatureUpdatePaintAll();
	ESPMenu::GetInstance()->FeatureUpdatePaintAll();
	IndicatorMenu::GetInstance()->FeatureUpdatePaintAll();

	OriginalMenu_PaintAll(r3, r4);
}

Detour<void> OriginalR_EndFrame;
void R_EndFrameHook() {
	pGame.pContext = (UIContext*)GetPatch(xbLive.pPatches->BO2.Menu_PaintAllPointer);

	static once_flag initFlag;
	call_once(&initFlag, [] {
		pRenderer.Initialize();
		pMenu.Initialize();
	});

	pController.Update();
	pSubmenuHandler.ASyncUpdate();
	pMenu.Render();
	pMenu.ASyncUpdate();

	OriginalR_EndFrame.CallOriginal();
}

typedef int(*tCL_SendCmd)(DWORD dwPacket);
tCL_SendCmd OriginalCL_SendCmd;
int CL_SendCmdHook(DWORD dwPacket) {
	static int fakeLagCount = 0;

	auto cg = Engine::GetCG();
	ClientActive* ca = Engine::GetClientActive();
	auto oldUserCommand = ca->GetCommand(ca->dwCommandNumber - 1);
	auto currentUserCommand = ca->GetCommand(ca->dwCommandNumber);
	auto nextUserCommand = ca->GetCommand(ca->dwCommandNumber + 1);

	if (!oldUserCommand || !currentUserCommand || !nextUserCommand) return OriginalCL_SendCmd(dwPacket);;

	*nextUserCommand = *currentUserCommand;
	currentUserCommand->iServerTime--;
	ca->dwCommandNumber++;

	// use command
	if (AimbotMenuVars::vars.bAimbot) {
		if (AimbotMenuVars::vars.bAimingRequired && cg->fZoomState < 0.5f) return OriginalCL_SendCmd(dwPacket);

		pGame.iAimbotTarget = AimbotMenuVars::CalculateBestClient();

		if (HvHMenuVars::vars.bXDirection) {
			if (HvHMenuVars::vars.iXDirection == 0) {
				// Shield
				nextUserCommand->vViewAngles.X = ANGLE2SHORT(-40.f - ca->vBaseAngles.X);
			} else if (HvHMenuVars::vars.iXDirection == 1) {
				// Verticle Jitter
				static float spin = 0.f; spin += 45.f;
				if (spin > 69.99f)
					spin = -69.99f;

				nextUserCommand->vViewAngles.X = ANGLE2SHORT(spin - ca->vBaseAngles.X);
			} else if (HvHMenuVars::vars.iXDirection == 2 || HvHMenuVars::vars.iXDirection == 3) {
				// Look Up/Look Down
				nextUserCommand->vViewAngles.X = ANGLE2SHORT((HvHMenuVars::vars.iXDirection == 2 ? -69.99f : 69.99f) - ca->vBaseAngles.X);
			}
		}

		if (HvHMenuVars::vars.bYDirection) {
			if (HvHMenuVars::vars.iYDirection == 0) {
				// Shield
				if (pGame.iAimbotTarget != -1 || pGame.iClosestAATarget != -1) {
					int target = pGame.iAimbotTarget == -1 ? pGame.iClosestAATarget : pGame.iAimbotTarget;
					Vector3<float> viewOrigin;
					Engine::CG_GetPlayerViewOrigin(0, Engine::GetCG()->GetPlayerState(), &viewOrigin);

					Vector3<float> pos = Engine::GetEntity(target).vOrigin;
					Vector3<float> subtracted = pos - viewOrigin;

					Vector3<float> angle;
					Engine::VectorToAngle(&subtracted, &angle);

					nextUserCommand->vViewAngles.Y = ANGLE2SHORT((angle.Y - 160.f) - ca->vBaseAngles.Y);
				}
			} else if (HvHMenuVars::vars.iYDirection == 1) {
				// Random Jitter
				nextUserCommand->vViewAngles.Y = ANGLE2SHORT(pUtils.FloatMinMax(0.f, 360.f) - ca->vBaseAngles.Y);
			} else if (HvHMenuVars::vars.iYDirection == 2) {
				// Spin
				static float angle = 0.f;

				if (angle > 360.f)
					angle -= 360.f;

				angle += HvHMenuVars::vars.iYSpeed;
				nextUserCommand->vViewAngles.Y = ANGLE2SHORT(angle);
			} else if (HvHMenuVars::vars.iYDirection == 3) {
				// Fake
				static bool flip = false; flip ^= true;
				nextUserCommand->vViewAngles.Y = ANGLE2SHORT((flip ? 69.99f : -69.99f) - ca->vBaseAngles.Y);
			}
		}
		
		if (pGame.iAimbotTarget != -1) {
			Vector3<float> viewOrigin;
			Engine::CG_GetPlayerViewOrigin(0, cg->GetPlayerState(), &viewOrigin);
			if (viewOrigin.IsNull()) return OriginalCL_SendCmd(dwPacket);;

			auto entity = Engine::GetEntity(pGame.iAimbotTarget);
			auto bonePosition = Engine::GetTagPosition(pGame.iAimbotTarget, pGame.Clients[pGame.iAimbotTarget].pBestTag, false);
			if (bonePosition.IsNull()) return OriginalCL_SendCmd(dwPacket);;

			if (AimbotMenuVars::vars.bPositionPrediction) {
				auto oldPosition = entity.GetPreviousPredictedPosition();
				auto newPosition = entity.GetNextPredictedPosition();

				auto velocity = (newPosition - oldPosition).NormalizeInline();
				velocity *= AimbotMenuVars::vars.fPositionPrediction;
				bonePosition += velocity;
			}

			auto subtracted = bonePosition - viewOrigin;

			Vector3<float> angle;
			Engine::VectorToAngle(&subtracted, &angle);

			switch (AimbotMenuVars::vars.iAimType) {
				case 0: // snap
					AimbotMenuVars::SnapAim(angle);
					break;
				case 1: // silent
					AimbotMenuVars::SilentAim(currentUserCommand, angle);
					break;
				case 2: // legit
					break;
			}

			if (MainMenuVars::vars.bNoSpread) {
				MainMenuVars::NoSpread(nextUserCommand, currentUserCommand);
			}

			if (AimbotMenuVars::vars.bAutoFire) {
				static int timer = 0;
				if (GetTickCount() - timer > AimbotMenuVars::vars.iAutoFireDelay) {
					timer = GetTickCount();
					nextUserCommand->iButtons |= 0x80000000;

					if (currentUserCommand->iButtons & 0x80000000) {
						currentUserCommand->iButtons &= 0x7FFFFFFFu;
					}
				}
			} else {
				XINPUT_STATE State;
				XInputGetState(0, &State);

				if (State.Gamepad.bRightTrigger > 35) {
					nextUserCommand->iButtons |= 0x80000000;

					if (currentUserCommand->iButtons & 0x80000000) {
						currentUserCommand->iButtons &= 0x7FFFFFFFu;
					}
				}
			}

			if (HvHMenuVars::vars.bCrouchEvent) {
				static int crouchCounter = 0;
				static bool crouching = false;

				switch (HvHMenuVars::vars.iCrouchEvent) {
				case 0: // On Damage
					if (Engine::GetCG()->iHealth < Engine::GetCG()->iMaxHealth) {
						crouchCounter++;

						if (crouchCounter >= 8 && !crouching) {
							Engine::CL_SetStance(0, 1);
							crouchCounter = 0;
							crouching = true;
						} else if (crouchCounter >= 8 && crouching) {
							Engine::CL_SetStance(0, 0);
							crouchCounter = 0;
							crouching = false;
						}
					}
					break;
				case 1: // On Target
					if (pGame.iAimbotTarget != -1) {
						crouchCounter++;

						if (crouchCounter >= 8 && !crouching) {
							Engine::CL_SetStance(0, 1);
							crouchCounter = 0;
							crouching = true;
						} else if (crouchCounter >= 8 && crouching) {
							Engine::CL_SetStance(0, 0);
							crouchCounter = 0;
							crouching = false;
						}
					}
					break;
				case 2: // Always
					crouchCounter++;

					if (crouchCounter >= 8 && !crouching) {
						Engine::CL_SetStance(0, 1);
						crouchCounter = 0;
						crouching = true;
					} else if (crouchCounter >= 8 && crouching) {
						Engine::CL_SetStance(0, 0);
						crouchCounter = 0;
						crouching = false;
					}
					break;
				}
			}

			if (HvHMenuVars::vars.bFakeLag) {
				fakeLagCount++;
				if (fakeLagCount >= HvHMenuVars::vars.iFakeLag || (oldUserCommand->iButtons != 0) || (currentUserCommand->iButtons != 0) || (nextUserCommand->iButtons != 0)) {
					fakeLagCount = 0;
				}
			} else {
				fakeLagCount = 0;
			}
		}
	} else {
		fakeLagCount = 0;
	}

	if (fakeLagCount == 0)
		return OriginalCL_SendCmd(dwPacket);
	else return 0;
}

typedef HRESULT(*tXamInputGetState)(DWORD, DWORD, XINPUT_STATE*);
tXamInputGetState OriginalXamInputGetState;
HRESULT XamInputGetStateHook(DWORD dwUserIndex, DWORD dwDeviceContext, XINPUT_STATE *pInputState) {
	auto result = OriginalXamInputGetState(dwUserIndex, dwDeviceContext, pInputState);

	if (pInputState) {
		if (Engine::InGame()) {
			if (MainMenuVars::vars.bFastReload) {
				if (MainMenuVars::vars.bWasReloading) {
					if (!MainMenuVars::vars.bReloadYPressed) {
						if (!((pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y)) {
							pInputState->Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
						}

						MainMenuVars::vars.bReloadYPressed = true;
						MainMenuVars::vars.iReloadCount++;
					}

					if (MainMenuVars::vars.iReloadCount > 1) {
						MainMenuVars::vars.bWasReloading = false;
						MainMenuVars::vars.bReloadYPressed = false;
						MainMenuVars::vars.iReloadCount = 0;
					}
				}
			}

			if (AimbotMenuVars::vars.bAutoADS) {
				if (pGame.iAimbotTarget != -1) {
					pInputState->Gamepad.bLeftTrigger = 100;
				}
			}
		}

		if (pMenu.IsOpen()) {
			pController.bABypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_A);
			pController.bBBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_B);
			pController.bLeftBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			pController.bRightBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
			pController.bLeftBumperBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			pController.bRightBumperBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

			pInputState->Gamepad.wButtons &= ~(XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B | XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER);
		}
	}

	return result;
}

typedef void(*tCG_Respawn)(int iLocalClient, bool bUnk);
tCG_Respawn OriginalCG_Respawn;
void CG_RespawnHook(int iLocalClient, bool bUnk) {
	OriginalCG_Respawn(iLocalClient, bUnk);

	Engine::CacheEquipmentIndexs();

	MainMenuVars::DvarSetHearEveryone();
	MainMenuVars::DvarThirdPerson();
	MainMenuVars::DvarFOV();

	MainMenuVars::vars.bWasReloading = false;
	MainMenuVars::vars.bReloadYPressed = false;
	MainMenuVars::vars.iReloadCount = 0;
}

typedef void(*tCG_EntityEvent)(int, int, int, int);
tCG_EntityEvent OriginalCG_EntityEvent;
void CG_EntityEventHook(int client, int entityState, int pevent, int eventParam) {
	if (MainMenuVars::vars.bFastReload) {
		if (pevent == 23) {
			auto reloadIndex = *(short*)(entityState + 0x2C2);
			if (reloadIndex == (short)Engine::GetCG()->iClientNumber) {
				MainMenuVars::vars.bWasReloading = true;
			}
		} else if (pevent == 29) {
			auto clientSwitchIndex = *(short*)(entityState + 0x2C2);
			if (MainMenuVars::vars.bWasReloading && MainMenuVars::vars.iReloadCount == 1) {
				MainMenuVars::vars.bReloadYPressed = false;
			}
		}
	}

	OriginalCG_EntityEvent(client, entityState, pevent, eventParam);
}

typedef DWORD(*tPreGameAntiFreeze)(const char** r3, DWORD* r4, DWORD r5, DWORD r6, DWORD r7, DWORD r8);
tPreGameAntiFreeze OriginalPreGameAntiFreeze;
DWORD PreGameAntiFreezeHook(const char** r3, DWORD* r4, DWORD r5, DWORD r6, DWORD r7, DWORD r8) {
	DWORD newR8;

	if (r8 == 0) {
		return OriginalPreGameAntiFreeze(r3, r4, r5, r6, r7, (DWORD)&newR8);
	}

	return OriginalPreGameAntiFreeze(r3, r4, r5, r6, r7, r8);
}

typedef DWORD(*tGameMessageOne)(DWORD localclient, DWORD Message, DWORD duration);
tGameMessageOne OriginalGameMessageOne;
DWORD GameMessageOneHook(DWORD localclient, DWORD Message, DWORD duration) {
	if (Message != 0) {
		const char* boldMessage = (const char *)Message;

		if (strstr(boldMessage, "^H")) {
			return 0;
		}
	}
	return OriginalGameMessageOne(localclient, Message, duration);
}

typedef DWORD(*tGameMessageTwo)(DWORD localclient, DWORD Message, DWORD duration);
tGameMessageTwo OriginalGameMessageTwo;
DWORD GameMessageTwoHook(DWORD localclient, DWORD Message, DWORD duration) {
	if (Message != 0) {
		const char* boldMessage = (const char *)Message;

		if (strstr(boldMessage, "^H")) {
			return 0;
		}
	}
	return OriginalGameMessageTwo(localclient, Message, duration);
}

typedef DWORD(*tShowNotify)(int localclientnum, char* msg, const char* idklol, int* idklol2);
tShowNotify OriginalShowNotify;
DWORD ShowNotifyHook(int localclientnum, char* msg, const char* idklol, int* idklol2) {
	if (Engine::InGame()) {
		if (strstr(msg, "^HO") || strstr(msg, "^I") || strstr(msg, "^i") || strstr(msg, "^H"))
			msg[0] = '\0';
	}

	return OriginalShowNotify(localclientnum, msg, idklol, idklol2);
}

typedef void(*tDynEntCl_DestroyEvent)(int localClientNum, unsigned __int16 dynEntId, int drawType, Vector3<float>& hitPos, Vector3<float>& hitDir);
tDynEntCl_DestroyEvent OriginalDynEntCl_DestroyEvent;
void DynEntCl_DestroyEvent_Hook(int localClientNum, unsigned __int16 dynEntId, int drawType, Vector3<float>& hitPos, Vector3<float>& hitDir) {
	if (Engine::InGame()) {
		int** someShitBoi = (int**)0x83798218;
		if (!someShitBoi[drawType] || !someShitBoi[drawType][dynEntId])
			return;
	}

	OriginalDynEntCl_DestroyEvent(localClientNum, dynEntId, drawType, hitPos, hitDir);
}

typedef void(*tXMemCpy)(void *Dst, const void *Source, int Size);
tXMemCpy OriginalXMemCpy;
void XMemCpyHook(void *Dst, const void *Source, int Size) {
	DWORD rAddr;
	__asm mflr rAddr

	if (rAddr == 0x82717C8C && !Engine::InGame())
		return;

	if (Size >= INT_MAX || Size <= 0)
		return;

	return OriginalXMemCpy(Dst, Source, Size);
}

bool Title::ApplyHooks() {
	if (!bInitialized) {
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.RedBoxPerk[0]) = 0x60000000;
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.RedBoxPerk[1]) = 0x60000000;
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.GetTagPosPatch[0]) = 0x60000000;
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.GetTagPosPatch[1]) = 0x60000000;
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.Probation[0]) = 0x60000000; // probation
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.Probation[1]) = 0x4800007C; // probation
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.CheatProtection) = 0x60000000; // cheat protection
		*(DWORD*)GetPatch(xbLive.pPatches->BO2.UnlimitedClassItems) = 0x3B40CBE7; // unlimited class items
		*(short*)GetPatch(xbLive.pPatches->BO2.AntiRapidFire) = 0x4800; // anti rapid fire

		if (xbLive.bDevkit) *(DWORD*)GetPatch(xbLive.pPatches->BO2.DevkitCrashPatch) = 0x48000018;

		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.CG_EntityEvent), &CG_EntityEventHook, (void**)&OriginalCG_EntityEvent);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.CG_Respawn), &CG_RespawnHook, (void**)&OriginalCG_Respawn);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.CL_SendCmd), &CL_SendCmdHook, (void**)&OriginalCL_SendCmd);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.Menu_PaintAll), &Menu_PaintAllHook, (void**)&OriginalMenu_PaintAll);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.XamInputGetState), &XamInputGetStateHook, (void**)&OriginalXamInputGetState);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.PreGameAntiFreeze), &PreGameAntiFreezeHook, (void**)&OriginalPreGameAntiFreeze);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.GameMessageOne), &GameMessageOneHook, (void**)&OriginalGameMessageOne);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.GameMessageTwo), &GameMessageTwoHook, (void**)&OriginalGameMessageTwo);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.ShowNotify), &ShowNotifyHook, (void**)&OriginalShowNotify);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.DynEntCl_DestroyEvent), &DynEntCl_DestroyEvent_Hook, (void**)&OriginalDynEntCl_DestroyEvent);
		pHooking.HookFunction(GetPatch(xbLive.pPatches->BO2.XMemCpy), &XMemCpyHook, (void**)&OriginalXMemCpy);
		OriginalR_EndFrame.SetupDetour(GetPatch(xbLive.pPatches->BO2.R_EndFrame), R_EndFrameHook);

		LOG_DEV("Applied detours!");
		bInitialized = true;
		return true;
	} else {
		LOG_DEV("TitleHooks already initialized!");
	}

	return false;
}

void Title::Remove(bool bOverride) {
	if (bInitialized || bOverride) {
		pHooking.UnhookAll();
		OriginalR_EndFrame.TakeDownDetour();

		Sleep(1000);

		LOG_DEV("Removed detours!");
		bInitialized = false;
	} else {
		LOG_DEV("TitleHooks have not been initialized!");
	}
}
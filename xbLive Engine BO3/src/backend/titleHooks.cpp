#include "stdafx.h"
// #include "menu/submenus/visual/compassMenu.h"

Title pTitle;

typedef void(*tMenu_PaintAll)(QWORD r3);
tMenu_PaintAll OriginalMenu_PaintAll;
void Menu_PaintAllHook(QWORD r3) {
	OriginalMenu_PaintAll(r3);

	static once_flag initFlag;
	call_once(&initFlag, [] {
		pRenderer.Initialize();
		pMenu.Initialize();
	});

	if (Engine::InGame()) pController.Update();
	pSubmenuHandler.ASyncUpdate();
	if (Engine::InGame()) pMenu.Render();
	pMenu.ASyncUpdate();
}

/*typedef int(*tCL_SendCmd)(DWORD dwPacket);
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
		if (AimbotMenuVars::vars.bAimingRequired && cg->GetZoomState() < 0.5f) return OriginalCL_SendCmd(dwPacket);

		pGame.iAimbotTarget = AimbotMenuVars::CalculateBestClient();
		if (pGame.iAimbotTarget != -1) {
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
					if (pGame.iAimbotTarget != -1) {
						Vector3<float> viewOrigin;
						Engine::CG_GetPlayerViewOrigin(0, Engine::GetCG()->GetPlayerState(), &viewOrigin);

						Vector3<float> pos = Engine::GetEntity(pGame.iAimbotTarget).vOrigin;
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
					if (Engine::GetCG()->GetHealth() < Engine::GetCG()->GetMaxHealth()) {
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

typedef void(*tCG_Respawn)(int iLocalClient, bool bUnk);
tCG_Respawn OriginalCG_Respawn;
void CG_RespawnHook(int iLocalClient, bool bUnk) {
	OriginalCG_Respawn(iLocalClient, bUnk);

	// Engine::CacheEquipmentIndexs();

	MainMenuVars::DvarThirdPerson();
	MainMenuVars::DvarFOV();
	MainMenuVars::DvarSetHearEveryone();

	MainMenuVars::vars.bWasReloading = false;
	MainMenuVars::vars.bReloadYPressed = false;
	MainMenuVars::vars.iReloadCount = 0;
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

			if (pMenu.IsOpen()) {
				pController.bABypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_A);
				pController.bBBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_B);
				pController.bLeftBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
				pController.bRightBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
				pController.bLeftBumperBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
				pController.bRightBumperBypass = (bool)(pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

				pInputState->Gamepad.wButtons &= ~(XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B | XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER);
			}

			if (AimbotMenuVars::vars.bAutoADS) {
				if (pGame.iAimbotTarget != -1) {
					pInputState->Gamepad.bLeftTrigger = 100;
				}
			}
		}
	}

	return result;
}

Detour<void> OriginalCG_EntityEvent;
void CG_EntityEventHook(int client, int entityState, int pevent, int eventParam) {
	if (MainMenuVars::vars.bFastReload) {
		if (pevent == 23) {
			auto reloadIndex = *(short*)(entityState + 0x1DC);
			if (reloadIndex == (short)Engine::GetCG()->iLocalClientNumber) {
				MainMenuVars::vars.bWasReloading = true;
			}
		} else if (pevent == 26) {
			auto clientSwitchIndex = *(short*)(entityState + 0x8);
			if (MainMenuVars::vars.bWasReloading && MainMenuVars::vars.iReloadCount == 1) {
				MainMenuVars::vars.bReloadYPressed = false;
			}
		}
	}

	OriginalCG_EntityEvent.CallOriginal(client, entityState, pevent, eventParam);
}
*/

bool Title::ApplyHooks() {
	if (!bInitialized) {
		DbgPrint("cg_hudGrenadePointerWidth: %X\n", Invoke::Call<DWORD>(0x826BA6C0, "cg_hudGrenadePointerWidth"));

		//OriginalCG_EntityEvent.SetupDetour(0x821C0818, &CG_EntityEventHook);

		//pHooking.HookFunction(0x821E0360, &CG_RespawnHook, &OriginalCG_Respawn); 
		//pHooking.HookFunction(0x8223B8C8, &CL_SendCmdHook, &OriginalCL_SendCmd); 
		//pHooking.HookFunction(0x826CF558, &XamInputGetStateHook, &OriginalXamInputGetState); 
		pHooking.HookFunction(0x822112F0, &Menu_PaintAllHook, &OriginalMenu_PaintAll); 

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
		//OriginalCG_EntityEvent.TakeDownDetour();
		pHooking.UnhookAll();

		Sleep(1000);

		LOG_DEV("Removed detours!");
		bInitialized = false;
	} else {
		LOG_DEV("TitleHooks have not been initialized!");
	}
}
#pragma once

class Engine {
public:
	// UI
	static Material* R_RegisterMaterial(const char* pFont, int iImageTrack);
	static Font* R_RegisterFont(const char* pFont, int iImageTrack);
	static void R_AddCmdDrawStretchPic(float x, float y, float width, float height, float xScale, float yScale, float xay, float yay, const float * color, Material* material);
	static void R_AddCmdDrawText(const char *text, int maxChars, Font *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style);
	static int R_TextWidth(const char* text, Font* font);
	static void* CG_DrawRotatedPicPhysical(int scrPlacement, float x, float y, float width, float height, float angle, const float* color, Material* material);
	static int R_TextHeight(Font* font);
	static int R_NormalizedTextScale(Font* font, float scale);

	// Dvars
	static bool Dvar_GetBool(const char* dvar);

	// Trace
	static void CG_Trace(Trace* trace, Vector3<float>* start, Vector3<float>* end, int ignore, int mask = 0x803003);
	static void FireBulletPenetrate(int client, BulletFireParams* bullet, int weapon, int unk1, int unk2, Entity* entity, int alwaysMinusOne, Vector3<float>* viewOrigin, bool unkBool);

	// Misc
	static void Cbuf_AddText(int localPlayer, const char* command);
	static bool CG_DObjGetWorldTagPos(Entity* entity, int dObj, DWORD tag, Vector3<float>* out);
	static Vector3<float> GetTagPosition(int client, const char* tag);
	static int Com_GetClientDObj(int handle, int something);
	static short SL_GetString(const char* str);
	static int AimAssist_GetTagPos(int mask, Entity* entity, short tag, Vector3<float>* out);
	static void CG_DrawGrenadePointer(int localClientNum, float centerX, float centerY, Vector3<float>* grenadeOffset, float* color);
	static void CG_DrawGrenadeIcon(int localClientNum, float centerX, float centerY, float unk, Vector3<float>* grenadeOffset, float unk2, float* color, Material* material);
	static void RandomBulletDir(int* randomSeed, float zero, float threesixty, float* x, float* y);
	static void BG_GetSpreadForWeapon(cg* ps, float* minSpread, float* maxSpread);
	static void CG_GetPlayerViewOrigin(int client, cg* ps, Vector3<float>* out);
	static void VectorToAngle(Vector3<float>* direction, Vector3<float>* out);
	static float BG_Random(int* pSeed);
	static void CL_SetStance(int client, int stance);
	static void CG_GetEntityBModelBounds(int local, int entityIndex, Vector3<float>* min, Vector3<float>* max);
	static int CG_GetWeaponIndexForName(const char* name);
	static bool CG_CalcNamePosition(int client, Vector3<float>* out, float* outx, float* outy);

	// Custom
	static Vector2<float> GetCenter();
	static Entity* GetEntityPtr();
	static Entity GetEntity(int index);
	static cg* GetCG();
	static ClientInfo GetClientInfo(int index);
	static ServerInfo* GetServerInfo();
	static ClientActive* GetClientActive();
	static bool InGame();
	static DWORD TimeGetTime();
	static bool IsFriend(int client);
	static bool IsAlive(int clientNum, bool notPlayer = false);
	static float GetBoneFraction(int client, const char* aimtag);
	static bool IsLookingAtMe(int client, float* pDegree);
	static float GetDistance(Vector3<float> from, Vector3<float> to);
	static bool WorldToScreen(Vector3<float> in, Vector3<float>* out);
	static void GetWeaponKillIconAspect(int type, Vector2<float>* aspect);
	static WeaponDef* GetWeaponDef(int weapon);
	static Material* GetWeaponMaterialWithAspect(int weapon, Vector2<float>* aspect);
	static bool IsWeaponEquipmentItem(int weapon);
	static bool IsWeaponThis(int weapon, eWeaponCheck check);
	static void CacheEquipmentIndexs();

	// Inline statics
	template<typename T>
	static inline void SetDvar(DWORD dwAddress, T value) {
		*(T*)(*(DWORD*)(dwAddress) + 0xC) = value;
	}

	template<typename T>
	static inline T GetDvar(DWORD dwAddress) {
		return *(T*)(*(DWORD*)(dwAddress) + 12);
	}

	static int EquipmentWeaponIndexs[14];
};
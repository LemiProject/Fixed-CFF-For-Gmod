#include "Esp1.h"
#include "../../engine/Engine.h"
//#include "../Util/EngineDraw.h"
#include "../Hooks/Hooks.h"
#include "../Functional/FriendList.h"
#include "../../engine/Render/Menu/Gui.h"
#include "../Functional/EffectHook.h"
#include "Teams.h"

std::vector<const char*> ttt_weapons =
{ "weapon_ttt_c4", "weapon_ttt_knife", "weapon_ttt_phammer", "weapon_ttt_sipistol", "weapon_ttt_flaregun", "weapon_ttt_push", "weapon_ttt_radio", "weapon_ttt_teleport", "(Disguise)" ,"spiderman's_swep", "weapon_ttt_trait_defilibrator", "weapon_ttt_xbow", "weapon_ttt_dhook", "weapon_awp", "weapon_jihadbomb", "weapon_ttt_knife", "weapon_ttt_c4", "weapon_ttt_decoy", "weapon_ttt_flaregun", "weapon_ttt_phammer", "weapon_ttt_push", "weapon_ttt_radio", "weapon_ttt_sipistol", "weapon_ttt_teleport", "weapon_ttt_awp", "weapon_ttt_silencedsniper", "weapon_ttt_turtlenade", "weapon_ttt_death_station", "weapon_ttt_sg552", "weapon_ttt_tripmine" };

void cEsp::Esp_Tick()
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || Render::Screen_Shot)
		return;
	this->pLocal = pLocal;
	/*ttt_players.clear();
	//C_BaseCombatWeapon::m_hOwnerEntity;
	for (auto i_Num = 0; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto pEntity = C_BaseEntity::GetBaseEntity(i_Num);

		if (!pEntity || !pEntity->IsValid() || pEntity == pLocal)
			continue;
		if (!pEntity->GetClientClass() || !pEntity->GetClientClass()->GetName() || !pEntity->GetScriptName())
			continue;

		if (strcmp("CWeaponSWEP", pEntity->GetClientClass()->GetName()) == 0) {
			bool find = false;
			for (auto it : ttt_weapons) {
				if (strcmp(pEntity->GetScriptName(), it) == 0)
					find = true;
			}
			if (find) {
				auto owner = g_pEntList->GetClientEntityFromHandle(pEntity->m_hOwnerEntity());
				if (owner && owner->IsValid()) {
					ttt_players.push_back(owner->entindex());
				}
			}
		}
	}*/

	if (Engine::Var::Instance().PropList_Toggle.Get()) {
		EntEsp::Instance().Update_Ent_vec();
		EntEsp::Instance().Update_Temp_Ent();
	}
	for (auto i_Num = 0; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto pEntity = C_BaseEntity::GetBaseEntity(i_Num);

		if (!pEntity || !pEntity->IsValid() || pEntity == pLocal || pEntity->IsDormant())
			continue;
		Vector temp = {};
		//auto org = pEntity->GetRenderOrigin();
		if (!SDK::W2S(pEntity->GetRenderOrigin(), temp)) {
			continue;
		}

		this->pEntity = pEntity;
		auto pGPlayer = ToGMOD_Player(ToBasePlayer(pEntity));
		pGPlayer
			? GPlayer_Tick(pGPlayer)
			: BaseEnt_Tick(pEntity);
	}

	//ttt_players.clear();
}

void cEsp::GPlayer_Tick(C_GMOD_Player * ply)
{
	if (!ply->IsGood())
		return;

	player_info_t info = {};
	if (!g_pEngine->GetPlayerInfo(ply->entindex(), &info))
		return;

	if (Engine::Var::Instance().Esp_Bot_Check.Get())
		if (info.fakeplayer)
			return;
	Vector Distance = pLocal->m_vecOrigin() - ply->m_vecOrigin();

	if (Distance.Length() >= Engine::Var::Instance().Esp_Max_Distance.Get())
		return;

	if (!ply->GetBoneTransform(pBoneToWorldOut))
		return;

	auto transform = ply->m_rgflCoordinateFrame();
	auto mins = ply->OBBMins();
	auto maxs = ply->OBBMaxs();

	Vector points[] = {
	{ mins[0], mins[1], mins[2] },
	{ mins[0], maxs[1], mins[2] },
	{ maxs[0], maxs[1], mins[2] },
	{ maxs[0], mins[1], mins[2] },
	{ maxs[0], maxs[1], maxs[2] },
	{ mins[0], maxs[1], maxs[2] },
	{ mins[0], mins[1], maxs[2] },
	{ maxs[0], mins[1], maxs[2] },
	};
	for (auto i = 0; i < 8; i++)
		pointsTransformed[i] = points[i].Transform(transform);

	if (!SDK::W2S(pointsTransformed[3], flb) || !SDK::W2S(pointsTransformed[5], brt)
		|| !SDK::W2S(pointsTransformed[0], blb) || !SDK::W2S(pointsTransformed[4], frt)
		|| !SDK::W2S(pointsTransformed[2], frb) || !SDK::W2S(pointsTransformed[1], brb)
		|| !SDK::W2S(pointsTransformed[6], blt) || !SDK::W2S(pointsTransformed[7], flt))
		return;



	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
	left = flb[0];
	top = flb[1];
	right = flb[0];
	bottom = flb[1];

	for (int i = 0; i < 8; i++)
	{
		if (left > arr[i][0])
			left = arr[i][0];
		if (top < arr[i][1])
			top = arr[i][1];
		if (right < arr[i][0])
			right = arr[i][0];
		if (bottom > arr[i][1])
			bottom = arr[i][1];
	}
	x = std::round(left);
	y = std::round(bottom);

	w = std::round(right - left);
	h = std::round(top - bottom);

	Teams::Team team_info = {};
	if (!Teams::GetTeamInfo(team_info, ply))
		return;
	this->Team = team_info.team_id;
	this->TeamName = team_info.Name;
	this->TeamColor = team_info.Color;
	if (Engine::Var::Instance().Esp_Team_Check.Get()) {
		if (Teams::PlayerWhitelisted(ply))
			return;
	}

	b_use_team_col = Engine::Var::Instance().Esp_Colors_By_Class.Get();
	b_is_friend = FriendList::Instance().IsFriend(ply);

	if (Engine::Var::Instance().Esp_Box2D.Get())
		DrawBox2D(ply);
	
	if (Engine::Var::Instance().Esp_Skeleton.Get())
		DrawSkeleton(ply);
	
	DrawInfo(ply, info);
}

void cEsp::BaseEnt_Tick(C_BaseEntity * ent)
{

	if (!ent->GetScriptName() || IsBadReadPtr(ent->GetScriptName(), 0x4))
		return;

	auto script_name = ent->GetScriptName();

	


key_end:;

	if (Engine::Var::Instance().PropList_Toggle.Get()) {
		//EntEsp::Instance().Update_Ent_vec();
		//EntEsp::Instance().Update_Temp_Ent();
		EntEsp::Instance().EntEsp_tick(ent);
	}


}

void cEsp::DrawBox2D(C_BaseEntity * ent)
{
	auto meme = cColor(this->TeamColor);
	cColor curr = {};
	curr.SetColor(meme.b(), meme.g(), meme.r(), 255);

	auto meme2 = cColor(Engine::Var::Instance().Friend_Color.Get());
	cColor curr2 = {};
	curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);

	auto meme3 = cColor(Engine::Var::Instance().Esp_Box2D_Color.Get());
	cColor curr3 = {};
	curr3.SetColor(meme3.b(), meme3.g(), meme3.r(), 255);
	auto th = Engine::Var::Instance().Esp_Thickness_Outline.Get();
	if (!b_is_friend) {
		//Draw::Box(x, y, w, h, D3DCOLOR_ARGB(255, 1, 1, 1), Engine::Var::Instance().Esp_Thickness.Get() + Engine::Var::Instance().Esp_Thickness_Outline.Get());

		//Draw::Box(x + (th*0.5f), y + (th*0.5f), w - th, h - th, D3DCOLOR_ARGB(255, 0, 0, 0), Engine::Var::Instance().Esp_Thickness.Get() + Engine::Var::Instance().Esp_Thickness_Outline.Get());
		Draw::Box_border(x, y, w, h, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), Engine::Var::Instance().Esp_Thickness.Get(), Engine::Var::Instance().Esp_Thickness_Outline.Get());
		//	draw.Outline(x, y, w, h, b_use_team_col ? this->TeamColor : Color(Engine::Var::Instance().Esp_Box2D_Color.Get()).to_cColor(), Engine::Var::Instance().Esp_Thickness.Get());
	}
	else {
		//Draw::Box(x, y, w, h, D3DCOLOR_ARGB(255, 1, 1, 1), Engine::Var::Instance().Esp_Thickness.Get() + Engine::Var::Instance().Esp_Thickness_Outline.Get());
	//	Draw::Box(x + (th*0.5f), y + (th*0.5f), w - th, h - th, D3DCOLOR_ARGB(255, 0, 0, 0), Engine::Var::Instance().Esp_Thickness.Get() + Engine::Var::Instance().Esp_Thickness_Outline.Get());
		Draw::Box_border(x, y, w, h, curr2.GetRawColor(), Engine::Var::Instance().Esp_Thickness.Get(), Engine::Var::Instance().Esp_Thickness_Outline.Get());
		//	draw.Outline(x, y, w, h, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), Engine::Var::Instance().Esp_Thickness.Get());

	}
}

void cEsp::DrawBox3D(C_BaseEntity * ent)
{

	auto col = Color(Engine::Var::Instance().Esp_Box3D_Color.Get()).to_cColor();
	auto l_thick = Engine::Var::Instance().Esp_Thickness.Get();
	auto o_thick = Engine::Var::Instance().Esp_Thickness_Outline.Get();

	Draw::Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));

	Draw::Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));

	Draw::Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
	Draw::Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));

	////////////////////

	if (!b_is_friend) {
		auto meme = cColor(this->TeamColor);
		cColor curr = {};
		curr.SetColor(meme.b(), meme.g(), meme.r(), 255);

		auto meme2 = cColor(col);
		cColor curr2 = {};
		curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);
		Draw::Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());

		Draw::Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());

		Draw::Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
	}
	else {
		col = Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor();
		auto meme = cColor(this->TeamColor);
		cColor curr = {};
		curr.SetColor(meme.b(), meme.g(), meme.r(), 255);

		auto meme2 = cColor(col);
		cColor curr2 = {};
		curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);
		Draw::Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());

		Draw::Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());

		Draw::Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
		Draw::Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick, b_use_team_col ? curr.GetRawColor() : curr2.GetRawColor());
	}
}
 
void cEsp::DrawSkeleton(C_GMOD_Player * ply)
{
	auto pStudioModel = g_pVModelInfo->GetStudiomodel(ply->GetModel());

	if (!pStudioModel)
		return;

	auto col = Color(Engine::Var::Instance().Esp_Skeleton_Color.Get()).to_cColor();
	wchar_t Temp[256];
	for (int i = 0; i < pStudioModel->numbones; i++) {

		mstudiobone_t* pBone = pStudioModel->pBone(i);


		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;



		Vector vBonePos1;
		if (!SDK::W2S(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;

		Vector vBonePos2;
		if (!SDK::W2S(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;
		Draw::Line(vBonePos1.m_x, vBonePos1.m_y, vBonePos2.m_x, vBonePos2.m_y, Engine::Var::Instance().Esp_Thickness.Get() + Engine::Var::Instance().Esp_Thickness_Outline.Get(), D3DCOLOR_ARGB(255, 1, 1, 1));
	}
	auto meme = cColor(this->TeamColor);
	cColor curr = {};
	curr.SetColor(meme.b(), meme.g(), meme.r(), 255);

	auto meme2 = cColor(Engine::Var::Instance().Friend_Color.Get());
	cColor curr2 = {};
	curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);

	auto meme3 = cColor(col);
	cColor curr3 = {};
	curr3.SetColor(meme3.b(), meme3.g(), meme3.r(), 255);

	for (int i = 0; i < pStudioModel->numbones; i++) {

		mstudiobone_t* pBone = pStudioModel->pBone(i);


		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;

		Vector vBonePos1;
		if (!SDK::W2S(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;

		Vector vBonePos2;
		if (!SDK::W2S(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;
		if (!b_is_friend) {
			Draw::Line(vBonePos1.m_x, vBonePos1.m_y, vBonePos2.m_x, vBonePos2.m_y, Engine::Var::Instance().Esp_Thickness.Get(), b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor());
			//draw.Line(int(vBonePos1.m_x), int(vBonePos1.m_y), int(vBonePos2.m_x), int(vBonePos2.m_y), b_use_team_col ? this->TeamColor : col, Engine::Var::Instance().Esp_Thickness.Get());
		}
		else {
			Draw::Line(vBonePos1.m_x, vBonePos1.m_y, vBonePos2.m_x, vBonePos2.m_y, Engine::Var::Instance().Esp_Thickness.Get(), curr2.GetRawColor());
			//draw.Line(int(vBonePos1.m_x), int(vBonePos1.m_y), int(vBonePos2.m_x), int(vBonePos2.m_y), Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), Engine::Var::Instance().Esp_Thickness.Get());

		}
	}
}

void cEsp::DrawHeadCircle(C_GMOD_Player * ply)
{
	Vector min = {};
	Vector max = {};
	matrix3x4_t bone_data[MAXSTUDIOBONE] = {};

	if (!ply->SetupBones(bone_data, MAXSTUDIOBONE, 0x0100, 0))
		return;

	if (!ply->GetBoxBoundWorld("ValveBiped.Bip01_Head1", bone_data, min, max))
		return;
	auto center_coord = (min + max)*0.5f;
	Vector screen_center;
	if (!SDK::W2S(center_coord, screen_center))
		return;
	Vector screen_max;
	if (!SDK::W2S(max, screen_max))
		return;
	Vector screen_min;
	if (!SDK::W2S(min, screen_min))
		return;

	float Radius = (screen_max - screen_min).Length() / 2;



	auto col = Color(Engine::Var::Instance().Esp_Head_Color.Get()).to_cColor();

	auto meme = cColor(this->TeamColor);
	cColor curr = {};
	curr.SetColor(meme.b(), meme.g(), meme.r(), 255);

	auto meme2 = cColor(Engine::Var::Instance().Friend_Color.Get());
	cColor curr2 = {};
	curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);

	auto meme3 = cColor(col);
	cColor curr3 = {};
	curr3.SetColor(meme3.b(), meme3.g(), meme3.r(), 255);

	if (!b_is_friend) {
		Draw::CircleEmpty(screen_center[0], screen_center[1], Radius, 0, 360.f, 64, D3DCOLOR_ARGB(255, 0, 0, 0), Engine::Var::Instance().Esp_Thickness.Get() + 2.f);
		Draw::CircleEmpty(screen_center[0], screen_center[1], Radius, 0, 360.f, 64, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), Engine::Var::Instance().Esp_Thickness.Get());

		//Draw::Circle(screen_center[0], screen_center[1], Radius, 0, 360.f, 64, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor());
		//draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius, b_use_team_col ? this->TeamColor : col);
	}
	else {
		Draw::CircleEmpty(screen_center[0], screen_center[1], Radius, 0, 360.f, 64, D3DCOLOR_ARGB(255, 0, 0, 0), Engine::Var::Instance().Esp_Thickness.Get() + 2.f);
		Draw::CircleEmpty(screen_center[0], screen_center[1], Radius, 0, 360.f, 64, curr2.GetRawColor(), Engine::Var::Instance().Esp_Thickness.Get());

		//Draw::Circle(screen_center[0], screen_center[1], Radius, 0, 360.f, 64, curr2.GetRawColor());
		//draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor());
	}
}

void cEsp::DrawInfo(C_GMOD_Player * ply, player_info_t& info)
{

	char Temp[256];

	auto Delta = 18.f;//g_pSurface->GetFontTall(draw.font_esp) / 0.7272727272727273f;
	auto text_col = Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor();

	auto meme = cColor(this->TeamColor);
	cColor curr = {};
	curr.SetColor(meme.b(), meme.g(), meme.r(), 255);

	auto meme2 = cColor(Engine::Var::Instance().Friend_Color.Get());
	cColor curr2 = {};
	curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);

	auto meme3 = cColor(text_col);
	cColor curr3 = {};
	curr3.SetColor(meme3.b(), meme3.g(), meme3.r(), 255);

	int idx = 1;
	if (Engine::Var::Instance().Esp_Active_Weapon.Get()) {
		char* weap_name = nullptr;
		if (ply->m_hActiveWeapon()) {
			auto Weapon = ply->GetActiveWeapon();
			if (Weapon && !IsBadReadPtr(Weapon, 0x4)) {
				if (Weapon->GetScriptName() && !IsBadReadPtr(Weapon->GetScriptName(), 0x4)) {
					weap_name = Weapon->GetScriptName();
				}
				else {
					weap_name = Weapon->GetClientClass()->GetName();
				}
			}
		}
		if (!weap_name) {
			weap_name = "None";
		}

		//	MultiByteToWideChar(CP_UTF8, NULL, &weap_name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			//Draw::Text_Outline(g_font, KeypadList[iter].TempCode.c_str(), (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, Color(Engine::Var::Instance().Esp_Text_Color.Get()).GetRawColor());
			Draw::Text_Outline(g_font, weap_name, (left + right)*0.5f, bottom - Delta, FontSize1, 2.f, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), true);
			//draw.text((left + right)*0.5f, bottom - Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			Draw::Text_Outline(g_font, weap_name, (left + right)*0.5f, bottom - Delta, FontSize1, 2.f, curr2.GetRawColor(), true);
			//	draw.text((left + right)*0.5f, bottom - Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);
		}
	}
	if (Engine::Var::Instance().Esp_HP.Get()) {
		//	auto diff = ((left + right)*0.5f) - left;
		auto x_pos = left - 8;
		float x_pos11;
		
		Draw::Line(x_pos, bottom - 1.f, x_pos, top + 1.f, 6.f, D3DCOLOR_ARGB(255, 1, 1, 1));
		//Draw::Line_Outline(left - 4, bottom, left - 4, top, 4, 2);
	//	draw.Line_outline(left - 4, bottom, left - 4, top, 4, 2);
		auto procent_ = float(ply->m_iHealth()) / float(ply->m_iMaxHealth());
		auto height_delta = bottom - top;
		int hp = ply->m_iHealth();
		if (hp >= 1000 && !(hp >= 10000))
			x_pos11 = left - 30;
		else if(hp >= 100 && !(hp >=1000))
			x_pos11 = left - 25;
		else if (hp >= 10000 && !(hp >= 100000))
			x_pos11 = left - 32;
		else if (hp >= 100000 && !(hp >= 1000000))
			x_pos11 = left - 38;
		else if (hp >= 1000000 && !(hp >= 10000000))
			x_pos11 = left - 40;
		else if (hp >= 10000000 && !(hp >= 100000000))
			x_pos11 = left - 44;
		else if (hp >= 100000000 && !(hp >= 1000000000))
			x_pos11 = left - 48;
		else if (hp >= 1000000000 && !(hp >= 10000000000))
			x_pos11 = left - 53;
		else if (hp >= 10000000000 && !(hp >= 100000000000))
			x_pos11 = left - 58;
		else if (hp >= 100000000000 && !(hp >= 1000000000000))
			x_pos11 = left - 58;
		else if (hp >= 9 && !(hp >= 10))
			x_pos11 = left - 20;
		else if (hp < 100)
			x_pos11 = left - 20;
		char hpa[256];
		sprintf(hpa, "%d", hp);
		procent_ <= 1.f
			? Draw::Line(x_pos, top, x_pos, top + (height_delta*procent_), 4, cColor(0, 255, 0, 255).GetRawColor())
			: Draw::Line(x_pos, bottom, x_pos, top, 4, cColor(0, 255, 0, 255).GetRawColor());
		Draw::Text_Outline(g_font, hpa, x_pos11, (top + height_delta)+10, FontSize1, 2, cColor(0, 255, 0, 255).GetRawColor(), true);
	}

	if (Engine::Var::Instance().Esp_Distance.Get()) {
		//	swprintf_s(Temp, L"%f", ply->m_flSimulationTime());
		sprintf_s(Temp, "%.1fm", (pLocal->m_vecOrigin() - ply->m_vecOrigin()).Length()* 0.0254f);


		if (!b_is_friend) {

			Draw::Text_Outline(g_font, Temp, (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), true);

			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			Draw::Text_Outline(g_font, Temp, (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, curr2.GetRawColor(), true);
			//	draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}

	if (Engine::Var::Instance().Esp_Steam_Name.Get()) {

		//MultiByteToWideChar(CP_UTF8, NULL, &info.name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			Draw::Text_Outline(g_font, info.name, (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), true);

			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			Draw::Text_Outline(g_font, info.name, (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, curr2.GetRawColor(), true);

			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}
	if (Engine::Var::Instance().Esp_RP_Name.Get()) {
		auto rp_name = Lua::GetName(ply->entindex());
		auto aye = Lua::GetUserGroup(ply->entindex());
		if (!rp_name)
			rp_name = "None";
		//MultiByteToWideChar(CP_UTF8, NULL, &rp_name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			Draw::Text_Outline(g_font, rp_name, (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), true);
			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			Draw::Text_Outline(g_font, rp_name, (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, curr2.GetRawColor(), true);
			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}
	if (Engine::Var::Instance().Esp_Rank.Get()) {
		auto aye = Lua::GetUserGroup(ply->entindex());
		if (!aye)
			aye = "None";
		//MultiByteToWideChar(CP_UTF8, NULL, &rp_name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			Draw::Text_Outline(g_font, aye, (left + right) * 0.5f, top + idx * Delta, FontSize1, 2.f, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), true);
			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			Draw::Text_Outline(g_font, aye, (left + right) * 0.5f, top + idx * Delta, FontSize1, 2.f, curr2.GetRawColor(), true);
			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}
	if (Engine::Var::Instance().Esp_Class.Get()) {
		//MultiByteToWideChar(CP_UTF8, NULL, &TeamName[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			Draw::Text_Outline(g_font, TeamName.c_str(), (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, b_use_team_col ? curr.GetRawColor() : curr3.GetRawColor(), true);

			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			/*const char * team_name = TeamName.c_str();
			for (auto it : ttt_players) {
				if (ply->entindex() == it)
					team_name = "Traitor";
			}*/
			Draw::Text_Outline(g_font, TeamName.c_str(), (left + right)*0.5f, top + idx * Delta, FontSize1, 2.f, curr2.GetRawColor(), true);

			//draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);
		}
		idx++;
	}

}

void EntEsp::EntEsp_tick(C_BaseEntity * ent)
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal)
		return;
	auto script_name = ent->GetScriptName();

	std::string ent_name = script_name;
	if (!ent_name.size())
		return;

	if (ent_name.find("gmodz") != -1) {
		ent_name = get_model_name(ent);
	}

	if (!ent_name.size())return;


	if (Ent_vec.size()) {
		auto iter = 0u;
		for (auto it : Ent_vec) {
			iter++;
			auto var_bool = it->ExtractAs<bool>();
			if (!var_bool)
				continue;

			if (var_bool->s_Name == ent_name) {
				if (var_bool->Get()) {
					if (Ent_vec.size() < iter + 1)
						break;
					auto var_col = Ent_vec[iter]->ExtractAs<D3DCOLOR>();
					if (!var_col)
						return;

					Vector Dist = pLocal->m_vecOrigin() - ent->m_vecOrigin();
					if (Dist.Length() > Engine::Var::Instance().PropList_Distance.Get())
						return;
					auto transform = ent->m_rgflCoordinateFrame();
					auto mins = ent->OBBMins();
					auto maxs = ent->OBBMaxs();

					Vector points[] = {
						{ mins[0], mins[1], mins[2] },
					{ mins[0], maxs[1], mins[2] },
					{ maxs[0], maxs[1], mins[2] },
					{ maxs[0], mins[1], mins[2] },
					{ maxs[0], maxs[1], maxs[2] },
					{ mins[0], maxs[1], maxs[2] },
					{ mins[0], mins[1], maxs[2] },
					{ maxs[0], mins[1], maxs[2] },
					};
					for (auto i = 0; i < 8; i++)
						pointsTransformed[i] = points[i].Transform(transform);

					if (!SDK::W2S(pointsTransformed[3], flb) || !SDK::W2S(pointsTransformed[5], brt)
						|| !SDK::W2S(pointsTransformed[0], blb) || !SDK::W2S(pointsTransformed[4], frt)
						|| !SDK::W2S(pointsTransformed[2], frb) || !SDK::W2S(pointsTransformed[1], brb)
						|| !SDK::W2S(pointsTransformed[6], blt) || !SDK::W2S(pointsTransformed[7], flt))
						return;


					auto col = Color(var_col->Get()).to_cColor();
					cColor curr = {};
					curr.SetColor(col.b(), col.g(), col.r(), 255);
					auto l_thick = Engine::Var::Instance().Esp_Thickness.Get();
					auto o_thick = Engine::Var::Instance().Esp_Thickness_Outline.Get();

					Draw::Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));

					Draw::Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));

					Draw::Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));
					Draw::Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick + o_thick, D3DCOLOR_ARGB(255, 1, 1, 1));

					/*	Draw::Line_Outline(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick, o_thick);
						Draw::Line_Outline(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick, o_thick);
						Draw::Line_Outline(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick, o_thick);
						Draw::Line_Outline(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick, o_thick);

						Draw::Line_Outline(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick, o_thick);
						Draw::Line_Outline(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick, o_thick);
						Draw::Line_Outline(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick, o_thick);
						Draw::Line_Outline(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick, o_thick);

						Draw::Line_Outline(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick, o_thick);
						Draw::Line_Outline(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick, o_thick);
						Draw::Line_Outline(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick, o_thick);
						Draw::Line_Outline(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick, o_thick);*/

						////////////////////

					Draw::Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick, curr.GetRawColor());
					Draw::Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick, curr.GetRawColor());
					Draw::Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick, curr.GetRawColor());
					Draw::Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick, curr.GetRawColor());

					Draw::Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick, curr.GetRawColor());
					Draw::Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick, curr.GetRawColor());
					Draw::Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick, curr.GetRawColor());
					Draw::Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick, curr.GetRawColor());

					Draw::Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick, curr.GetRawColor());
					Draw::Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick, curr.GetRawColor());
					Draw::Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick, curr.GetRawColor());
					Draw::Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick, curr.GetRawColor());

					if (Engine::Var::Instance().PropList_Render_Name.Get()) {
						Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
						float left = flb.m_x;
						float top = flb.m_y;
						float right = flb.m_x;
						float bottom = flb.m_y;

						for (int i = 1; i < 8; i++) {
							if (left > arr[i].m_x)
								left = arr[i].m_x;
							if (top < arr[i].m_y)
								top = arr[i].m_y;
							if (right < arr[i].m_x)
								right = arr[i].m_x;
							if (bottom > arr[i].m_y)
								bottom = arr[i].m_y;
						}

						auto meme2 = cColor(Engine::Var::Instance().Esp_Text_Color.Get());
						cColor curr2 = {};
						curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);
						//wchar_t Temp[256];
						//	auto text_col = Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor();
					//	MultiByteToWideChar(CP_UTF8, NULL, &ent_name[0], -1, &Temp[0], 255);
						Draw::Text_Outline(g_font, ent_name.c_str(), (left + right) / 2, top + 0.5f, FontSize1, 2.f, Color(Engine::Var::Instance().Esp_Text_Color.Get()).GetRawColor(), true);
						//draw.text((left + right) / 2, top + 0.5f, Temp, draw.font_esp, col, true);
					}

				}

				else {
					break;
				}
			}
		}
	}


}

void EntEsp::Update_Temp_Ent()
{
	if (!g_pEngine->IsInGame() || g_pEngine->IsDrawingLoadingImage())
		return;
	static float Show;
	float upd_Time = 0.1f*g_pGlobalVars->absoluteframetime;
	Show += upd_Time;
	if (Show <= 1.9f) {
		return;
	}
	Show = 0.f;

	if (Temp_Ent.size()) {
		Temp_Ent.clear();
	}
	for (auto i_Num = 0u; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto ent = C_BaseEntity::GetBaseEntity1(i_Num);
		if (!ent || !ent->GetScriptName() || IsBadReadPtr(ent->GetScriptName(), 0x4))
			continue;
		std::string ent_name = ent->GetScriptName();
		if (!ent_name.size())
			continue;
		if (ent_name.find("gmodz") != -1) {
			ent_name = get_model_name(ent);
		}
		if (exist_in_temp(ent_name) || exist_in_list(ent_name))
			continue;

		Temp_Ent.push_back(ent_name);
	}


}

void EntEsp::Force_Update_Temp_Ent()
{
	if (Temp_Ent.size()) {
		Temp_Ent.clear();
	}
	for (auto i_Num = 0u; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto ent = C_BaseEntity::GetBaseEntity1(i_Num);
		if (!ent || !ent->GetScriptName() || IsBadReadPtr(ent->GetScriptName(), 0x4))
			continue;
		std::string ent_name = ent->GetScriptName();
		if (!ent_name.size())
			continue;
		if (ent_name.find("gmodz") != -1) {
			ent_name = get_model_name(ent);
		}
		if (exist_in_temp(ent_name) || exist_in_list(ent_name))
			continue;

		Temp_Ent.push_back(ent_name);
	}
}

void EntEsp::Update_Ent_vec()
{
	if (!is_vec_change&&Engine::Var::Instance().EngineVars.ParseVarGroup("Ent_vec").size() == Ent_vec.size())
		return;
	Ent_vec = Engine::Var::Instance().EngineVars.ParseVarGroup("Ent_vec");
	is_vec_change = false;
}

void EntEsp::Vis_PropEsp_tick()
{
	static C_BaseEntity* LastSelected;
	static float Show = 0.f;


	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsValid()) {

		return;
	}
		
	Vector EyePos = pLocal->EyePosition();
	Vector Forward;
	Hooks::Variables::PureAngle.ToVector(&Forward);
	Vector start = EyePos;
	Vector end = EyePos + Forward * Engine::Var::Instance().PropList_Distance.Get();
	trace_t Trace;

	static float Step;
	Step = (1.f / Engine::Var::Instance().PropList_HoldTime.Get())*g_pGlobalVars->absoluteframetime;
	SDK::Function::UTIL_TraceLine(start, end, MASK_SOLID, pLocal, 0, &Trace);

	if (Trace.m_pEnt&&Trace.m_pEnt->GetScriptName() && !IsBadReadPtr(Trace.m_pEnt->GetScriptName(), 0x4)) {
		if (!Trace.m_pEnt || Trace.m_pEnt->entindex() == -1)
			return;
		auto ent = C_BaseEntity::GetBaseEntity(Trace.m_pEnt->entindex());
		if (!ent || !ent->IsValid())
			return;
		if (LastSelected == ent) {
			Show += Step;
			Show = clamp(Show, 0.f, 1.f);
			std::string ent_name = ent->GetScriptName();
			if (!ent_name.size())
				return;
			if (ent_name.find("gmodz") != -1) {
				ent_name = get_model_name(ent);
			}
			if (Show >= 0.9) {
				//auto text_col = Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor();

				//wchar_t Temp[256];
				int w, h;
				g_pEngine->GetScreenSize(w, h);

				//MultiByteToWideChar(CP_UTF8, NULL, &ent_name[0], -1, &Temp[0], 255);
				//draw.text(w / 2, h / 2, Temp, draw.font_esp, text_col, true);
				auto meme2 = cColor(Engine::Var::Instance().Esp_Text_Color.Get());
				cColor curr2 = {};
				curr2.SetColor(meme2.b(), meme2.g(), meme2.r(), 255);
				Draw::Text_Outline(g_font, ent_name.c_str(), w / 2, h / 2, FontSize1, 2.f, curr2.GetRawColor(), true);
				if (GetAsyncKeyState(Engine::Var::Instance().PropList_Fast_Add_Key.Get()) && !exist_in_list(ent_name)) {

					Engine::Var::Instance().EngineVars.BeginGroup("Ent_vec");
					Engine::Var::Instance().EngineVars.RegisterVar(*new Core::VarUtil::Var<bool>, true, ent_name.c_str(), Core::VarUtil::v_dynamic);
					Engine::Var::Instance().EngineVars.RegisterVar(*new Core::VarUtil::Var<D3DCOLOR>, D3DCOLOR_ARGB(250, 250, 250, 255), std::string(ent_name + "_color"), Core::VarUtil::v_dynamic);
					Engine::Var::Instance().EngineVars.EndGroup();
					is_vec_change = true;
					EntEsp::Instance().Update_Ent_vec();
					EntEsp::Instance().Force_Update_Temp_Ent();
				}
				if (GetAsyncKeyState(Engine::Var::Instance().PropList_Fast_Del_Key.Get())) {


					std::string name = ent_name;
					std::string name_col = std::string(std::string(ent_name) + "_color");
					auto var = Engine::Var::Instance().EngineVars.FindVar(name, "Ent_vec");
					while (var) {
						Engine::Var::Instance().EngineVars.DeleteVar(*var);
						var = Engine::Var::Instance().EngineVars.FindVar(name.c_str(), "Ent_vec");
					}
					auto iter = 0u;
					for (auto it : EntEsp::Instance().Ent_vec) {
						if (!it || !it->s_Name) {
							iter++;
							continue;
						}
						if (strcmp(it->s_Name, name.c_str()) == 0) {
							EntEsp::Instance().Ent_vec.erase(EntEsp::Instance().Ent_vec.begin() + iter);
							iter--;
						}
						iter++;
					}

					auto var_col = Engine::Var::Instance().EngineVars.FindVar(name_col, "Ent_vec");
					while (var_col) {
						Engine::Var::Instance().EngineVars.DeleteVar(*var_col);
						var_col = Engine::Var::Instance().EngineVars.FindVar(name_col, "Ent_vec");
					}
					iter = 0u;
					for (auto it : EntEsp::Instance().Ent_vec) {
						if (!it || !it->s_Name) {
							iter++;
							continue;
						}
						if (strcmp(it->s_Name, name_col.c_str()) == 0) {
							EntEsp::Instance().Ent_vec.erase(EntEsp::Instance().Ent_vec.begin() + iter);
							iter--;
						}
						iter++;
					}
					EntEsp::Instance().is_vec_change = true;
					EntEsp::Instance().Update_Ent_vec();
					EntEsp::Instance().Force_Update_Temp_Ent();
				}
			}
		}
		else {
			Show = 0.f;
			LastSelected = ent;
		}
	}
	else {
		LastSelected = nullptr;
	}

}

bool EntEsp::exist_in_temp(std::string & name)
{
	for (auto a_name : Temp_Ent) {
		if (a_name == name) {
			return true;
		}
	}
	return false;
}

bool EntEsp::exist_in_list(std::string & name)
{
	for (auto it : Ent_vec) {
		auto b_var = it->ExtractAs<bool>();
		if (!b_var)
			continue;
		if (b_var->s_Name == name) {
			return true;
		}
	}
	return false;
}

std::string EntEsp::get_model_name(C_BaseEntity * ent)
{

	std::string ret = std::string();
	auto Model = ent->GetModel();
	if (!Model) {
		return ret;
	}
	auto pStudioModel = g_pVModelInfo->GetStudiomodel(Model);
	if (!pStudioModel) {
		return ret;
	}

	ret = pStudioModel->name;
	if (!ret.size()) {
		return ret;
	}
	if (ret.find_last_of('/') != -1) {
		ret = (ret.substr(ret.find_last_of('/') + 1));
	}
	if (ret.find_last_of('\\') != -1) {
		ret = (ret.substr(ret.find_last_of('\\') + 1));
	}
	if (ret.find('.') != -1) {
		ret = (ret.substr(0, ret.find('.')));
	}

	return ret;
}

#include "stdafx.h"
#include "Loading.h"
#include "Management.h"

#include "Sky.h"
#include "TexEffect.h"
#include "MeshEffect.h"
#include "Player.h"

#include "Weapon.h"
#include "Drain_Weapon.h"

#include "Dummy_Target.h"
#include "Trail_VFX.h"

#include "UI_Manager.h"
#include "Item_Manager.h"

#include "MonsterHeaders.h"
#include "YachaMan.h"

#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"

#include "MonsterUI.h"
#include "MassageUI.h"
#include "DamegeNumUI.h"
#include "Get_ItemUI.h"
#include "PickUp_ItemUI.h"

#include "Player_Colleague.h"

//#include "Item.h"


USING(Client)

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	ZeroMemory(m_szString, sizeof(_tchar) * 128);
	Safe_AddRef(m_pGraphicDev);
}

HRESULT CLoading::Ready_Loading(SCENEID eLoadingID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoadingID = eLoadingID;

	return S_OK;
}

unsigned int CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*	pLoading = (CLoading*)pArg;

	_uint		iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case SCENE_TITLE:
	{
		iFlag = pLoading->Loading_Title();
		break;
	}

	case SCENE_STAGE:
	{
		iFlag = pLoading->Loading_Stage();
		break;
	}
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	return iFlag;
}

HRESULT CLoading::Ready_Effect(void)
{
#pragma region ButterFly
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_PointParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_RingLine", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_RingLine_Distortion", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion_Circle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Bottom")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Smoke_Red_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Smoke_Red_Once")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Floor")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Mist")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_PointParticle_Plum")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Ready_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Ready_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Chunk")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"ButterFly_WaterSplash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_GlitterSand")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_PopSand")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_SubSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_PointParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_Chunk")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_Distortion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_Tail")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_DeadMist")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_DeadSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_DeadSplash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_BackStepSand01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_BackStepSand02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_BackStepSand_Floor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion_SmokeGravity")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_Body")))
		return E_FAIL;
#pragma endregion

#pragma region Queens Knight
	if (FAILED(Add_EffectPrototype(L"QueensKnight_2Phase_SwordCrash_Chunk")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_2Phase_SwordCrash_Chunk_Dark")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_2Phase_SwordCrash_ShockWave")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DistortionCircle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Intro_Smoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Intro_Smoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_JumpDown_Particle_Red")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_JumpDown_ShockWave", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_JumpDown_Smoke_Black")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_JumpDown_Smoke_Red")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Floor_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Floor_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_Hand")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lightning_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lightning_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lightning_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lightning_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lightning_4")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lightning_5")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LightningBody_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_ShieldAttack_Distortion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_ShieldAttack_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_ShieldAttack_RedLight")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_SwordCrash_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_SwordCrash_Particle_Orange")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Particle_Black")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Smoke_Red")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_DistortionSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Trail_Lightning_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Trail_Lightning_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Trail_Lightning_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Trail_Lightning_2_Dark")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_WhirlWind_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Sting_Tornade", true)))
		return E_FAIL;
#pragma endregion
	
#pragma region IceGirl
	if (FAILED(Add_EffectPrototype(L"IceFloorAura_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceFloorAura_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceFloorAura_03")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceSmoke_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceSmoke_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_FlashParticle_Blue")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_FlashParticle_Green")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_PointParticle_Blue")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_PointParticle_Green")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Main", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_01", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_02", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Break", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_SwordBullet_Mesh", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_SwordBullet_Mesh_2", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceCrystal_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceCrystal_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceCrystal_03")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Charge_Hand_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Charge_Hand_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Charge_Hand_Smoke_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Aura")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Aura_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Aura_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Break_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Break_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_FloorAura_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_FloorAura_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_FloorAura_03")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Smoke_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Smoke_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Break_Lightning")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Charge_Smoke_01")))
		return E_FAIL;
#pragma endregion

	if (FAILED(Add_EffectPrototype(L"Boss_KnockDown_Dust")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Boss_Dead_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"SpawnParticle_ForBoss")))
		return E_FAIL;

#pragma region Blood
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodMist_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodMist_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_4")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_5")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_4")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_5")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_Direction_6")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodDecal_0")))
		return E_FAIL;
#pragma endregion

#pragma region Slash
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_SlashLine_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Particle_Red")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Particle_Yellow")))
		return E_FAIL;
#pragma endregion

#pragma region Bullet
	if (FAILED(Add_EffectPrototype(L"Bullet_Body")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_Body_Aura")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_DeadFlash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_DeadSmoke_Base")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_DeadSmoke_Black")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_Fire_Flash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_Ready_Aura")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_Ready_Flash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Bullet_Tail_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"RockBullet_Body", true)))
		return E_FAIL;
#pragma endregion

	if (FAILED(Add_EffectPrototype(L"SpawnParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"SpawnParticle_Sub")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"MistletoeParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MistletoeParticle_Sub")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"ItemGet_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ItemObject")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ItemObject_Red")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ItemObject_Green")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ItemObject_Blue")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ItemObject_Purple")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ItemObject_Yellow")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"MapDust")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MapDust_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MapMist")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke_Jump")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke_DodgeBack")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Player_SpaceBar_StepParticle")))
		return E_FAIL;

#pragma region Skills
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Scratch_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Scratch_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_ScratchBlur_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_ScratchBlur_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_ScratchBlur_Sub_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_ScratchBlur_Sub_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_ShadowAssault_Scratch")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_ShadowAssault_ScratchBlur")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_DarkSmokeAura")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_Small")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_Big")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_Circle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_Flash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_ShockWave")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Buff_HandLight")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Buff_HandSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Buff_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Buff_Flash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Heal_RedLight")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Heal_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_BlastMesh", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Ring_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Ring_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Particle_Explosion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RedParticle_Explosion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RedParticle_Upper")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RedCircle_Flash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Distortion_Circle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Distortion_Water")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Distortion_Blaster")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RedOnion", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_SplitAssert_LaserBefore")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_SplitAssert_LaserAfter")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_SplitAssert_LaserBody")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_DarkRedOnion_Explosion", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Blood_Explosion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_SplitAssert_LaserAfter_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_SplitAssert_LaserAfter_RingLine", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Floor_BlackRing", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Floor_RedRing", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RotYRing_Black", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RotYRing_Red", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_WindMesh", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_WindTornadeMesh", true)))
		return E_FAIL;
#pragma endregion



	return S_OK;
}

HRESULT CLoading::Add_EffectPrototype(const _tchar* szName, _bool bIsMesh)
{
	_tchar szBuff[256] = L"../../Data/EffectData/";
	lstrcat(szBuff, szName);
	lstrcat(szBuff, L".dat");

	if (!bIsMesh)
	{
		if (FAILED(g_pManagement->Add_Prototype(szName, CTexEffect::Create(m_pGraphicDev, Read_EffectData(szBuff)))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(g_pManagement->Add_Prototype(szName, CMeshEffect::Create(m_pGraphicDev, Read_EffectData(szBuff)))))
			return E_FAIL;
	}

	return S_OK;
}


Engine::EFFECT_INFO* CLoading::Read_EffectData(const _tchar* szPath)
{
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		::MSG_BOX("Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;
	Engine::EFFECT_INFO* pInfo = new Engine::EFFECT_INFO;

	_bool bIsTex = false;
	::ReadFile(hFile, &bIsTex, sizeof(_bool), &dwByte, nullptr);

	while (true)
	{
		::ReadFile(hFile, &pInfo->bBillBoard, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bOnlyYRot, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bDistortion, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bStaticFrame, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bUseColorTex, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bUseRGBA, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->bColorMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bDirMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bFadeIn, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bFadeOut, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bLinearMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandomMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandomRot, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandStartPos, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRevColor, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRotMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bScaleMove, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->fAlphaSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fAlphaSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fAlphaSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fColorSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fCreateDelay, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fCreateDelay_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fCreateDelay_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fLifeTime, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMaxAlpha, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fAnimSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMaxFrame, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveScaleSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRandStartPosRange_Max, sizeof(_float) * 3, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRandStartPosRange_Min, sizeof(_float) * 3, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRotSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRotSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRotSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->iMaxCount, sizeof(_int), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szColorName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szGradientName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vEndColor, sizeof(_v4), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vMoveDirection, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vMoveScale, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vRandDirectionRange, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vRotDirection, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartColor, sizeof(_v4), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartPos, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartScale, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fColorIndex, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMaskIndex, sizeof(_float), &dwByte, nullptr);

		_bool bTemp;
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->bGravity, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandScale, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bMoveWithRot, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bSlowly, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fDistortionPower, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bDissolve, sizeof(_bool), &dwByte, nullptr);

		break;
	}

	CloseHandle(hFile);

	return pInfo;
}

_uint CLoading::Loading_Title()
{
	cout << "Title_Loading . . ." << endl;
	cout << "===============================================================================" << endl;

	// �ʼ� �޽� �ҷ�������
	//============================================================================================================
	cout << "�ʼ� �޽� �ҷ�������" << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Essential_Path.dat");

	// �ؽ��� �ҷ����� ��
	//============================================================================================================
	cout << "�ؽ��� �ҷ�������" << endl;
	g_pManagement->LoadTex_FromPath(m_pGraphicDev, L"../../Data/Tex_Path.dat");

	//============================================================================================================

	// ����Ʈ �޽� �ҷ�������
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Effect_Path.dat");
	// ����Ʈ ���� ����
	//============================================================================================================
	cout << "����Ʈ ���� ������" << endl;
	Ready_Effect();

	// UI ���� ����
	//============================================================================================================
	cout << " UI ���� ������" << endl;
	CUI_Manager::Get_Instance()->Add_UI_Prototype(m_pGraphicDev);
	CItem_Manager::Get_Instance()->Add_Item_Prototype(m_pGraphicDev);

	//============================================================================================================

	m_bFinish = true;

	system("cls");
	cout << "===============================================================================" << endl;
	cout << "Ÿ��Ʋ �ε� �Ϸ�" << endl;
	cout << "===============================================================================" << endl;
	cout << " 1. �� ���� Ű�� ������ �ɼ��� Ȱ��ȭ �˴ϴ�." << endl;
	cout << " 2. (Load_StaticMesh) �� (false) �̸� �������� ���� ���� �ȵ˴ϴ�." << endl;
	cout << " 3. Space �� ������ ���� ���������� �Ѿ�ϴ�." << endl;
	cout << " 4. ���� Stage_(n) �� ������� �ʰ� �ڵ����� Training Stage �� �Ѿ�ϴ�." << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[1] Stage_Base = true " << endl;
	cout << "[2] Stage_Training = false " << endl;
	cout << "[3] Stage_01 = false " << endl;
	cout << "[4] Stage_02 = false " << endl;
	cout << "[5] Stage_03 = false " << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[6] Load_StaticMesh = false " << endl;
	cout << "-------------------------------------------------------------------------------" << endl;


	return NO_EVENT;
}

_uint CLoading::Loading_Stage()
{
	// ����ƽ �޽� �ҷ����� ��
	//============================================================================================================
	if (m_bLoadStaticMesh)
		g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Static_Path.dat");

	// ���̳��� �޽� �ҷ����� ��
	//============================================================================================================
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Dynamic_Path.dat");


	// ���� �ҷ����� ��
	//============================================================================================================
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Weapon_Path.dat");
	// �Ϲ� ����
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_DrainWeapon_Path.dat");
	// ���� ����

	// BT_Node ���� ��
	//============================================================================================================
	if (FAILED(g_pManagement->Ready_BT_Node()))
		return E_FAIL;


	// ������Ʈ ���� ����
	//============================================================================================================

	// �÷��̾�
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	// ����
	//============================================================================================================

	// ������
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonButterfly", CPoisonButterfly::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������ 5�� ����ü
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonBullet", CPoisonBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������ ���� ����ü
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonChaseBullet", CPoisonChaseBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������ 4���� ����ü
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonRotationBullet", CPoisonRotationBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������ ���Ƽ� ��� �浹ü
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonTornado", CPoisonTornado::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������ ���
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_QueensKnight", CQueensKnight::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������ �� �����̴� �Ѿ�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_DarkGroundBullet", CDarkGroundBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// ��� ����
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_DarkBoom", CDarkBoom::Create(m_pGraphicDev))))
		return E_FAIL;
	// ��� ���� ���
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_DarkMidCol", CDarkMidCol::Create(m_pGraphicDev))))
		return E_FAIL;
	// ��������
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_IceGirl", CIceGirl::Create(m_pGraphicDev))))
		return E_FAIL;
	// �������� �ݵ��
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_ColdBeam", CColdBeam::Create(m_pGraphicDev))))
		return E_FAIL;
	// �������� �˱�߻�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordBullet", CSwordBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// �������� ������ȣ��
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_IceBarrier", CIceBarrier::Create(m_pGraphicDev))))
		return E_FAIL;
	// �ҳ���
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireBoy", CFireBoy::Create(m_pGraphicDev))))
		return E_FAIL;
	// �������� ������ �ݶ��̴�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunAttackCol", CGunAttackCol::Create(m_pGraphicDev))))
		return E_FAIL;
	// �� �Ѿ�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireBullet", CFireBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// ȭ����
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireSphere", CFireSphere::Create(m_pGraphicDev))))
		return E_FAIL;
	// Ÿ���� �ٴڿ� ȭ������
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireGround", CFireGround::Create(m_pGraphicDev))))
		return E_FAIL;
	// ȭ�� ����̵� �ݶ��̴�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireTornadoCol", CFireTornadoCol::Create(m_pGraphicDev))))
		return E_FAIL;
	// ���� ����
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_BlackUrchin", CBlackUrchin::Create(m_pGraphicDev))))
		return E_FAIL;
	// ���� ����
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_BlackWolf", CBlackWolf::Create(m_pGraphicDev))))
		return E_FAIL;
	// �˰���
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordGenji", CSwordGenji::Create(m_pGraphicDev))))
		return E_FAIL;
	// �˰��� �Ѿ�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordGenjiBullet", CSwordGenjiBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// �Ѱ���
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunGenji", CGunGenji::Create(m_pGraphicDev))))
		return E_FAIL;
	// �Ѱ��� �Ѿ�
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunGenjiBullet", CGunGenjiBullet::Create(m_pGraphicDev))))
		return E_FAIL;
	// �˹��а���
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordShieldGenji", CSwordShieldGenji::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_YachaMan", CYachaMan::Create(m_pGraphicDev))))
		return E_FAIL;
	// ������
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_Hunter", CHunter::Create(m_pGraphicDev))))
		return E_FAIL;

	// UI - Chea
	//============================================================================================================
	
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Colleague", CPlayer_Colleague::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MonsterHPUI", CMonsterUI::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MassageUI", CMassageUI::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_DamegeNumUI", CDamegeNumUI::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Get_ItemUI", CGet_ItemUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// ��Ÿ
	//============================================================================================================

	// ���� ����
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_DrainWeapon", CDrain_Weapon::Create(m_pGraphicDev))))
		return E_FAIL;
	//����
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon", CWeapon::Create(m_pGraphicDev))))
		return E_FAIL;
	//����
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Dummy", CDummy_Target::Create(m_pGraphicDev))))
		return E_FAIL;
	// Ʈ����
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SwordTrail", Engine::CTrail_VFX::Create(m_pGraphicDev))))
		return E_FAIL;
	// ��ī��
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Sky", CSky::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================
	m_bFinish = true;

	cout << "�ε� �Ϸ�" << endl;

	return NO_EVENT;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENEID eLoadingID)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eLoadingID)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}


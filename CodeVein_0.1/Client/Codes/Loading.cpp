#include "stdafx.h"
#include "Loading.h"
#include "Management.h"

#include "Sky.h"
#include "Sky_Blur.h"

#include "TexEffect.h"
#include "MeshEffect.h"
#include "DecalEffect.h"
#include "OrthoEffect.h"
#include "Effect_BloodChunk.h"
#include "Effect_LongSpark.h"
#include "Effect_FireFlower.h"
#include "Effect_Parent.h"

#include "NPC_Yakumo.h"

#include "Player.h"
#include "Costume_Hair.h"
#include "Costume_Outer.h"

#include "Weapon.h"
#include "Drain_Weapon.h"

#include "Dummy_Target.h"
#include "Trail_VFX.h"

#include "UI_Manager.h"
#include "Item_Manager.h"

#include "MonsterHeaders.h"
#include "Flag.h"
#include "ObjectPool_Manager.h"
#include "ClothDynamic.h"
#include "ClothStatic.h"

#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"

#include "MonsterUI.h"
#include "MassageUI.h"
#include "Get_ItemUI.h"
#include "PickUp_ItemUI.h"

#include "Player_Colleague.h"
#include "Colleague_Bullet.h"
#include "CollBullet_Heal.h"

#include "Colleague_UI.h"
#include "Colleague_Jack.h"

#include "DropItem.h"
#include "Haze.h"

#include "SoundManager.h"
#include "ClothManager.h"

USING(Client)

CLoading::CLoading(_Device pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
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

	case SCENE_PREPARE_ALL:
	{
		iFlag = pLoading->Loading_MainStages();
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
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_Body_Sub")))
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
	if (FAILED(Add_EffectPrototype(L"ButterFly_Crying_Distortion")))
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
	if (FAILED(Add_EffectPrototype(L"QueensKnight_JumpDown_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Floor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Floor_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Floor_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Smoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Smoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_BlackSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_LineRing_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Sphere_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Sphere_1", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Cone_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Cone_1", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Cone_2", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Cone_3", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Cone_4", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Ultimate_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_BlackFire_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Lightning_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Lightning_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Lava_Lightning_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Floor_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Floor_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_PopSmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_PopSmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_LaserBody")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_ReadyDistortion")))
		return E_FAIL;
	
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_Hand")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_LeakField_Hand_Aura")))
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
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_4")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_5")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_6")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_7")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_8")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_Teleport_Ink_9")))
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
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_Ring", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_RingBottom", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_RingRed", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"QueensKnight_DarkBoom_RingReady", true)))
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
	if (FAILED(Add_EffectPrototype(L"IceSmoke_02"))) // 이것 마지막
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_FlashParticle_Blue")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_FlashParticle_Green")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_PointParticle_Blue")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_PointParticle_Green")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_SlashSnowParticle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Main", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_01", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_02", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Main_Big", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_01_Big", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_02_Big", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Main_Mid", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_01_Mid", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Sub_02_Mid", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceBlock_Main_Small", true)))
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
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Bubble", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"IceGirl_Buff_Bubble_BreakSmoke")))
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
	
	
#pragma region FireBoy
	if (FAILED(Add_EffectPrototype(L"FireBoy_Charge_Hand_Fire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_ReadyFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_Mesh", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_Mesh_2", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_Floor_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_Floor_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_Floor_03")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_Fire_Particle_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_Fire_Particle_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireTornade_ReadyFire_Up")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_Mid")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_Big")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_Sub_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_Sub_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_GunEff")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_Particle_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireBullet_Particle_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_Floor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_ReadyFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_BoomCircle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_BoomParticle_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_BoomParticle_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_BoomFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_AfterFire_01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireGround_AfterFire_02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_BreakFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_BreakParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_OuterFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_BodyFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_BodyFire_Sub")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_Tail")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireSphere_ShotCircle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_ArmAttack_Dust")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_ArmAttack_FloorDust")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireHandBall", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireHandBall_Before_HandFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireHandBall_Dead_Light")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireHandBall_BodyFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireHandBall_BodyFire_Small")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireHandBall_Dead_FireExplosion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireFlame")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FireBoy_FireFlame_DeadFire")))
		return E_FAIL;
#pragma endregion
	
#pragma region DeerKing
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceStone_Up_Particle_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceStone_Up_Small_Particle_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_ReadySmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_ReadySmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_ReadySmoke_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_ReadySmoke_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_DeadParticle_Stone_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_DeadParticle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_Charge_Hand_Smoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_Charge_Hand_Smoke_After_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBullet_Charge_Hand_Smoke_After_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Point_ExplosionParticle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_SnowChunk_Up_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_SnowChunk_Up_Particle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_SnowChunk_Up_Particle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_SnowChunk_Up_Small_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_SnowChunk_Heavy_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Snow_Up_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Snow_Up_LongLine_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Snow_Up_LongLine_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_Mid_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_Mid_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_Small_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSmoke_Small_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Jump_In_Place_Distortion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_JumpPack_Distortion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_JumpPack_Fire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Body_PointParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Body_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Body_Smoke_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Distortion_Circle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceBlock_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_Tornade_Mesh_0", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DeerKing_IceSword", true)))
		return E_FAIL;
#pragma endregion
	
	if (FAILED(Add_EffectPrototype(L"Cocoon_TongueFire")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Fire_Bullet_Dead_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Fire_Bullet_Dead_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Fire_BulletBody")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Fire_Bullet_Dead_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Fire_Ready")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Totem_Ice_BulletBody")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Ice_Bullet_Dead_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Ice_Bullet_Dead_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Ice_Bullet_Dead_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Totem_Ice_Ready")))
		return E_FAIL;
	
	if (FAILED(Add_EffectPrototype(L"Monkey_Knife", true)))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Body_Lightning_Hor_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Body_Lightning_Ver_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Body_LinePoint_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Body_LinePoint_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Fire_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Ready_Light")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Tail_Lightning_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Dead_Flash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Dead_Lightning")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Dead_Smoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hunter_Bullet_Dead_Smoke_1")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Boss_KnockDown_Dust")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Boss_Dead_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"SpawnParticle_ForBoss")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"SpawnParticle_ForBoss_Point")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"SpawnParticle_ForBoss_Point_Sub")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Monster_DeadSmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Monster_DeadSmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Monster_DeadSmoke_2")))
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
	if (FAILED(Add_EffectPrototype(L"Blood_Chunk_0", true)))
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
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_L_0")))
		return E_FAIL;							  
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_L_1")))
		return E_FAIL;							  
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_L_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_R_0")))
		return E_FAIL;							   
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_R_1")))
		return E_FAIL;							   
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_R_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_LongSpark_Distortion_0")))
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
	if (FAILED(Add_EffectPrototype(L"MistletoeParticle_Active")))
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

	if (FAILED(Add_EffectPrototype(L"Haze_Body")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Haze_FlashParticle")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Weapon_HeavyDust")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"MapDust")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MapDust_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MapMist")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MapSnow")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"MapFog_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"FloorPlane_Black")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke_Jump")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke_DodgeBack")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Player_SpaceBar_StepParticle")))
		return E_FAIL;
	
#pragma region Colleague
	if (FAILED(Add_EffectPrototype(L"Colleague_Teleport_Flash_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Teleport_Line_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Teleport_Line_Particle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Teleport_Line_Particle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Teleport_Smoke_0")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Colleague_Heal_BulletBody_Aura")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Heal_BulletBody_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Heal_BulletBody_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Heal_HandBlood")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Heal_HandLight")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Heal_HandSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Healing_PointParticle")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_Blood")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_BloodSmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_BloodSmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_DistortionSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_HandLight_Pink_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_HandLight_Red_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_HandSmoke_Black_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_PinkSmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_PinkSmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_Start_Particle_Black_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_Start_Particle_Black_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_Start_Particle_Black_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_Start_Particle_Red_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Colleague_Skill_Start_Particle_Red_1")))
		return E_FAIL;
#pragma endregion

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
	if (FAILED(Add_EffectPrototype(L"Player_ChargeSpark_HalfCircle", true)))
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
	if (FAILED(Add_EffectPrototype(L"Player_Skill_RedOnion_3", true)))
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
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Rush_Particle_Yellow")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Rush_Particle_Orange")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Rush_Particle_White")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Rush_LaserBefore")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Rush_RedParticle_LaserBefore")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Rush_WhiteParticle_LaserBefore")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_4")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_5")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Drain_Ink_6")))
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
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BloodTornadeMesh", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BloodTornadeMesh_2", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BloodTornadeMesh_3", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BloodConeMesh", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BloodConeMesh_Explosion", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Torment_Wind_L", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Torment_Wind_R", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Torment_Wind_Distortion_L", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Torment_Wind_Distortion_R", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_FlashLight")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_PurpleLightning")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_ShotSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Smoke_Blue_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Smoke_Blue_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Smoke_DarkBlue")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Bullet_DeadFlash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Bullet_DeadShockWave")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Bullet_DeadSmoke_Move_Purple")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Bullet_DeadSmoke_Blue_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_Bullet_DeadSmoke_Blue_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_BulletBody_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Gun_BulletBody_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BottomBlood_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BottomBlood_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BottomBlood_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_BottomBlood_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_Blaster_Hor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_Blaster_Ver")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_Orange_LinePoint_Explosion_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_Orange_LinePoint_Explosion_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_RedParticle_Explosion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_ShockWave")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Player_Skill_Halberd_RedLightning_Mesh", true)))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"DrainWeapon_BlackSmoke_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_BlackSmoke_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_BlackSmoke_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_BlackSmoke_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_Counter_RedLight")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_Counter_ShockWave_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_Counter_ShockWave_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_Counter_ShockWave_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_RedLine_Explosion_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_WhiteParticle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_WhiteParticle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_WhiteParticle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"DrainWeapon_WhiteParticle_3")))
		return E_FAIL;
#pragma endregion

	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_0", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_0.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_1", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_1.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_2", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_2.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_3", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_3.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_4", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_4.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_5", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_5.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Blood_Decal_6", CDecalEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Blood_Decal_6.dat")))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"Snow_Ortho_0", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Snow_Ortho_0.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Snow_Ortho_1", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Snow_Ortho_1.dat")))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_BlackCorner_0", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Ortho_BlackCorner_0.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_BlackCorner_1", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Ortho_BlackCorner_1.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_BlackCorner_2", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Ortho_BlackCorner_2.dat")))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Blood_0", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Ortho_Blood_0.dat")))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_BossDead_BG", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Ortho_BossDead_BG.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_BossDead_Text", COrthoEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Ortho_BossDead_Text.dat")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading::Add_EffectPrototype(const _tchar* szName, _bool bIsMesh)
{
	_tchar szBuff[STR_256] = L"../../Data/EffectData/";
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
		::ReadFile(hFile, &pInfo->szColorName, sizeof(TCHAR) * STR_256, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szName, sizeof(TCHAR) * STR_256, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szGradientName, sizeof(TCHAR) * STR_256, &dwByte, nullptr);
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
	cout << "===============================================================================" << endl;
	cout << "Title_Loading . . ." << endl;
	cout << "===============================================================================" << endl;
	//============================================================================================================
	// 필수 메쉬 불러오는중
	//============================================================================================================
	cout << "Essential Static Mesh 불러오는 중 . . ." << endl;
	g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Essential_Path.dat");
	//============================================================================================================
	// 다이나믹 메쉬 불러오는 중
	//============================================================================================================
	cout << "Essential Dynamic Mesh 불러오는 중 . . ." << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Essential_Dynamic_Path.dat");

	//cout << "DynamicMesh 불러오는 중 . . ." << endl;
	//g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Dynamic_Path.dat");
	
	cout << "NPC 불러오는 중 . . ." << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_NPC_Path.dat");
	//============================================================================================================
	// 무기 불러오는 중
	//============================================================================================================
	cout << "Weapon Mesh 불러오는 중 . . ." << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Weapon_Path.dat");

	cout << "DrainWeapon Mesh 불러오는 중 . . ." << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_DrainWeapon_Path.dat");
	//============================================================================================================
	// 텍스쳐 불러오는 중
	//============================================================================================================
	cout << "All Texture 불러오는 중 . . ." << endl;
	g_pManagement->LoadTex_FromPath(m_pGraphicDev, L"../../Data/Load_TexData/Tex_Path.dat");
	//============================================================================================================
	// 이펙트 메쉬 불러오는중
	//============================================================================================================
	cout << "Effect Mesh 불러오는 중 . . ." << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Effect_Path.dat");
	//============================================================================================================
	// 이펙트 원형 생성
	//============================================================================================================
	cout << "Effect 원형 생성중" << endl;
	Ready_Effect();
	//============================================================================================================
	// 필수 파티클 생성
	//============================================================================================================
	cout << "Particle Essential 불러오는 중 . . ." << endl;
	if (FAILED(CParticleMgr::Get_Instance()->Ready_ParticleManager_Essential()))
		return E_FAIL;
	//============================================================================================================
	// 플레이어 스킬, 보스 이펙트 포함
	//============================================================================================================
	cout << "Particle Etc 불러오는 중 . . ." << endl;
	if (FAILED(CParticleMgr::Get_Instance()->Ready_ParticleManager()))
		return E_FAIL;
	//============================================================================================================
	// UI 원형 생성
	//============================================================================================================
	cout << " UI 원형 생성 중 . . ." << endl;
	CUI_Manager::Get_Instance()->Add_UI_Prototype(m_pGraphicDev);
	CItem_Manager::Get_Instance()->Add_Item_Prototype(m_pGraphicDev);
	//============================================================================================================
	// 기타
	//============================================================================================================
	cout << "Essential Protorypes 추가 중 . . ."  << endl;
	//============================================================================================================
	// 흡혈 무기
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_DrainWeapon", CDrain_Weapon::Create(m_pGraphicDev))))
		return E_FAIL;
	//무기
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon", CWeapon::Create(m_pGraphicDev))))
		return E_FAIL;
	// 트레일
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SwordTrail", Engine::CTrail_VFX::Create(m_pGraphicDev))))
		return E_FAIL;
	// 스카이
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Sky", CSky::Create(m_pGraphicDev))))
		return E_FAIL;
	// 스카이
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Sky_Blur", CSky_Blur::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================
	// 사운드
	//============================================================================================================
	g_pSoundManager->Load_Directory_SouneFile_W(L"BGM");
	//============================================================================================================
	// 옷
	//============================================================================================================
	cout << " 옷 생성 중 . . ." << endl;
	g_pClothManager->Ready_ClothManager();
	//============================================================================================================

	m_bFinish = true;

	system("cls");
	cout << "===============================================================================" << endl;
	cout << " 1. 위 숫자 키를 누르면 옵션이 활성화 됩니다." << endl;
	cout << " 2. (Load_StaticMesh) 가 (false) 이면 스테이지 들어가도 생성 안됩니다." << endl;
	cout << " 3. Space 를 누르면 다음 스테이지로 넘어갑니다." << endl;
	cout << " 4. 트레이닝 맵은 별도로 Load_StaticMesh 안해도 넘어갑니다." << endl;
	cout << " 5. 기본 설정된 맵은 Stage_Traing 입니다." << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << " #  [ 릴리즈 모드 ] 는 메쉬로드와 스테이지 진행이 실제 게임처럼 됩니다." << endl;
	cout << " #  [ 릴리즈 모드 ] 는 강제로 [ Stage_Base ] 부터 시작하게 됩니다." << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[1] Stage_Training = true " << endl;
	cout << "[2] Stage_Base = false " << endl;
	cout << "[3] Stage_01 = false " << endl;
	cout << "[4] Stage_02 = false " << endl;
	cout << "[5] Stage_03 = false " << endl;
	cout << "[6] Stage_04 = false " << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[8] Load_StaticMesh = ";
	cout << (m_bLoadStaticMesh ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[9] # 릴리즈 모드 # ";
	cout << (g_bReleaseMode ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;

	//g_pSoundManager->Stop_Sound(CSoundManager::Background_01);
	//g_pSoundManager->Play_Sound(L"Title_Intro_02.wav", CSoundManager::Background_01, CSoundManager::Master_Sound);
	//g_pSoundManager->Play_Sound(L"Gwan_Cchak.wav", CSoundManager::Background_01, CSoundManager::BGM_Sound);
	//g_pSoundManager->Play_BGM(L"Gwan_Cchak.wav");

	return NO_EVENT;
}

_uint CLoading::Loading_Stage()
{
	//============================================================================================================
	// 트레이닝이면 안불러와요
	//============================================================================================================
	if (0 != g_sStageIdx_Cur)
	{
		// 공통 스태틱 메쉬
		cout << "Common Static Mesh 불러오는 중 . . ." << endl;
		g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Common_Path.dat");

		// 기믹 메쉬
		cout << "Field Gimmick Static Mesh 불러오는 중 . . ." << endl;
		g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Field_Gimmick_Path.dat");
	}
	//============================================================================================================
	// 스테이지 별 로드
	//============================================================================================================
	switch (g_sStageIdx_Cur)
	{
	// 스테이지 - 트레이닝
	case 0:
	{
		// 1번 - 트레이닝
		if (false == g_bOnStage[1])
		{
			g_bOnStage[1] = true;
		}

		break;
	}
	// 스테이지 - 베이스
	case 1:
	{
		// 2번 - 베이스
		if (false == g_bOnStage[2])
		{
			cout << "Mesh Stage_00 Load. . ." << endl;
			g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage00_Path.dat");

			g_bOnStage[2] = true;
		}

		break;
	}
	// 스테이지 - 01
	case 2:
	{
		// 3번 - 스테이지 01
		if (false == g_bOnStage[3])
		{
			cout << "Load Stage_01 StaticMesh . . ." << endl;
			g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage01_Path.dat");

			cout << "Clone Stage_01 GameObject . . ." << endl;
			g_pManagement->LoadCreateObject_FromPath(m_pGraphicDev, L"Object_Stage_01.dat");

			g_bOnStage[3] = true;
		}

		break;
	}
	// 스테이지 - 02
	case 3:
	{
		// 4번 - 스테이지 02
		if (false == g_bOnStage[4])
		{
			g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage02_Path.dat");

			g_bOnStage[4] = true;
		}

		break;
	}
	// 스테이지 - 03
	case 4:
	{
		// 4번 - 스테이지 03
		if (false == g_bOnStage[5])
		{
			g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage03_Path.dat");

			g_bOnStage[5] = true;
		}
		break;
	}
	// 스테이지 - 04
	case 5:
	{
		// 4번 - 스테이지 04
		if (false == g_bOnStage[6])
		{
			g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage04_Path.dat");

			g_bOnStage[6] = true;
		}
		break;
	}
	}
	//============================================================================================================
	// Essential System Operating . . .
	// 최초 로딩 호출 시 1번만 불러옵니다.
	//============================================================================================================
	if (false == g_bOnStage[0])
	{
		//============================================================================================================
		// 노드 생성
		//============================================================================================================
		cout << "BT_Node 생성 중 . . ." << endl;
		//============================================================================================================
		if (FAILED(g_pManagement->Ready_BT_Node()))
			return E_FAIL;
		//============================================================================================================
		// 오브젝트 원형 생성
		//============================================================================================================
		cout << "Physics Test Flag 생성 중 . . ." << endl;
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Flag", CFlag::Create(m_pGraphicDev))))
			return E_FAIL;


		//============================================================================================================
		// 몬스터 프로토 타입
		cout << "Monster Prototype 생성 중 . . ." << endl;
		//============================================================================================================
		if (FAILED(Ready_Intro_MonsterPrototype()))
			return E_FAIL;
		//============================================================================================================
		// Colleague - Chea
		//============================================================================================================
		// 동료

		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Colleague", CPlayer_Colleague::Create(m_pGraphicDev))))
			return E_FAIL;
		// 동료 총알
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ColleagueBullet", CColleague_Bullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_ColleagueBullet", L"GameObject_ColleagueBullet", 100);
		// 힐용
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CollBullet_Heal", CCollBullet_Heal::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_CollBullet_Heal", L"GameObject_CollBullet_Heal", 100);
		////============================================================================================================
		// UI - Chea
		//============================================================================================================
		cout << "UI Sub Prototype 생성 중 . . ." << endl;
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MonsterHPUI", CMonsterUI::Create(m_pGraphicDev))))
			return E_FAIL;
		
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MassageUI", CMassageUI::Create(m_pGraphicDev))))
			return E_FAIL;
		
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Get_ItemUI", CGet_ItemUI::Create(m_pGraphicDev))))
			return E_FAIL;
		
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Colleague_UI", CColleague_UI::Create(m_pGraphicDev))))
			return E_FAIL;
		
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Colleague_Jack", CColleague_Jack::Create(m_pGraphicDev))))
			return E_FAIL;
		//============================================================================================================
		// ETC
		//============================================================================================================
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Haze", CHaze::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_Haze", L"GameObject_Haze", 200);

		// Effect_BloodChunk
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodChunk", CEffect_BloodChunk::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_BloodChunk", L"GameObject_BloodChunk", 200);

		// Effect_LongSpark
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LongSpark", CEffect_LongSpark::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_LongSpark", L"GameObject_LongSpark", 3000);

		// Effect_FireFlower
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_FireFlower", CEffect_FireFlower::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_FireFlower", L"GameObject_FireFlower", 3000);

		// Effect_Parent
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_EffParent", CEffect_Parent::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_EffParent", L"GameObject_EffParent", 100);

		///////test로 만들어둔 드롭아이템입니다 수정 Test 실험 심규명
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_DropItem", CDropItem::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"GameObject_DropItem", L"GameObject_DropItem", 200);

		//============================================================================================================
		// NPC
		//============================================================================================================
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_NPC_Yakumo", CNPC_Yakumo::Create(m_pGraphicDev))))
			return E_FAIL;
		//============================================================================================================

		// 옷
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Costume_Hair", CCostume_Hair::Create(m_pGraphicDev))))
			return E_FAIL;
		if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Costume_Outer", CCostume_Outer::Create(m_pGraphicDev))))
			return E_FAIL;

		g_bOnStage[0] = true;
	}

	m_bFinish = true;

	cout << "로딩 완료 . . . !" << endl;

	return NO_EVENT;
}

_uint CLoading::Loading_MainStages()
{
	// 3번 - 스테이지 01
	if (false == g_bOnStage[3])
	{
		cout << "Load Stage_01 StaticMesh . . ." << endl;
		g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage01_Path.dat");

		g_bOnStage[3] = true;
	}

	// 3번 - 스테이지 01
	if (false == g_bOnStage[4])
	{
		cout << "Load Stage_02 StaticMesh . . ." << endl;
		g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage02_Path.dat");

		g_bOnStage[4] = true;
	}

	// 3번 - 스테이지 01
	if (false == g_bOnStage[5])
	{
		cout << "Load Stage_03 StaticMesh . . ." << endl;
		g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage03_Path.dat");

		g_bOnStage[5] = true;
	}

	// 3번 - 스테이지 01
	if (false == g_bOnStage[6])
	{
		cout << "Load Stage_04 StaticMesh . . ." << endl;
		g_pManagement->LoadMesh_FilesFromPath_AddProtoRenderObj(m_pGraphicDev, L"../../Data/Load_MeshData/Mesh_Static_Stage04_Path.dat");

		g_bOnStage[6] = true;
	}

	m_bFinish = true;

	cout << "전부 로딩 완료 . . . !!!" << endl;

	return NO_EVENT;
}

HRESULT CLoading::Ready_Intro_MonsterPrototype()
{
	//============================================================================================================================================
	// 독나방
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonButterfly", CPoisonButterfly::Create(m_pGraphicDev))))
			return E_FAIL;
		// 독나방 5발 투사체
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonBullet", CPoisonBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_PoisonBullet", L"Monster_PoisonBullet", 20);
		// 독나방 추적 투사체
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonChaseBullet", CPoisonChaseBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_PoisonChaseBullet", L"Monster_PoisonChaseBullet", 5);
		// 독나방 4방향 투사체
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonRotationBullet", CPoisonRotationBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_PoisonRotationBullet", L"Monster_PoisonRotationBullet", 8);
		// 독나방 기모아서 쏘기 충돌체
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonTornado", CPoisonTornado::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_PoisonTornado", L"Monster_PoisonTornado", 3);
	}
	//============================================================================================================================================
	// 여왕의 기사
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_QueensKnight", CQueensKnight::Create(m_pGraphicDev))))
			return E_FAIL;
		// 땅에서 찌릿찌릿 움직이는 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DarkGroundBullet", CDarkGroundBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DarkGroundBullet", L"Monster_DarkGroundBullet", 16);
		// 어둠 폭발
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DarkBoom", CDarkBoom::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DarkBoom", L"Monster_DarkBoom", 16);
		// 기사 주위 결계
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DarkMidCol", CDarkMidCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DarkMidCol", L"Monster_DarkMidCol", 3);
		// 리크 필드
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_LeakField", CLeakField::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_LeakField", L"Monster_LeakField", 3);
	}
	//============================================================================================================================================
	//// 얼음여자
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_IceGirl", CIceGirl::Create(m_pGraphicDev))))
			return E_FAIL;
		// 얼음여자 콜드빔 작은 크기
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_ColdBeamSmall", CColdBeamSmall::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_ColdBeamSmall", L"Monster_ColdBeamSmall", 10);
		// 얼음여자 콜드빔 중간 크기
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_ColdBeamMiddle", CColdBeamMiddle::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_ColdBeamMiddle", L"Monster_ColdBeamMiddle", 8);
		// 얼음여자 콜드빔 큰 크기
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_ColdBeamBig", CColdBeamBig::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_ColdBeamBig", L"Monster_ColdBeamBig", 20);
		// 얼음여자 콜드빔 날카로운것
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_ColdBeamSharp", CColdBeamSharp::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_ColdBeamSharp", L"Monster_ColdBeamSharp", 10);
		// 얼음여자 검기발사
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordBullet", CSwordBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_SwordBullet", L"Monster_SwordBullet", 3);
		// 얼음여자 얼음보호막
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_IceBarrier", CIceBarrier::Create(m_pGraphicDev))))
			return E_FAIL;
		//// 보류
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_IceBarrier", L"Monster_IceBarrier", 2);
	}
	//============================================================================================================================================
	// 불남자
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireBoy", CFireBoy::Create(m_pGraphicDev))))
			return E_FAIL;
		//포신으로 때리기 콜라이더
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunAttackCol", CGunAttackCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_GunAttackCol", L"Monster_GunAttackCol", 3);
		
		// 불 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireBullet", CFireBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_FireBullet", L"Monster_FireBullet", 80);
		
		// 화염구
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireSphere", CFireSphere::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_FireSphere", L"Monster_FireSphere", 3);
		
		// 타겟의 바닥에 화염폭발
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireGround", CFireGround::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_FireGround", L"Monster_FireGround", 15);
		
		// 화염 토네이도 콜라이더
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireTornadoCol", CFireTornadoCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_FireTornadoCol", L"Monster_FireTornadoCol", 3);
		
		// 왼손에 화염구 소환
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireHandBall", CFireHandBall::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_FireHandBall", L"Monster_FireHandBall", 3);
		
		// 왼손에 화염구 죽은 후 생성되는 불꽃
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_FireFlame", CFireFlame::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_FireFlame", L"Monster_FireFlame", 48);
	}
	//============================================================================================================================================
	// 사슴왕
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKing", CDeerKing::Create(m_pGraphicDev))))
			return E_FAIL;
		// 사슴왕  투사체
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingBullet", CDeerKingBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingBullet", L"Monster_DeerKingBullet", 24);
		// 사슴왕  뾰족 얼음
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingColdBeam", CDeerKingColdBeam::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingColdBeam", L"Monster_DeerKingColdBeam", 6);
		// 사슴왕  점프주먹 콜라이더
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingJumpFistCol", CDeerKingJumpFistCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingJumpFistCol", L"Monster_DeerKingJumpFistCol", 3);
		// 사슴왕  제자리 점프 콜라이더
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingJumpInPlaceCol", CDeerKingJumpInPlaceCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingJumpInPlaceCol", L"Monster_DeerKingJumpInPlaceCol", 3);
		// 사슴왕  돌진 콜라이더
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingRushCol", CDeerKingRushCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingRushCol", L"Monster_DeerKingRushCol", 3);
		// 사슴왕  얼음검 소환
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingIceSword", CDeerKingIceSword::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingIceSword", L"Monster_DeerKingIceSword", 3);
		// 사슴왕  오른손 충돌체
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_DeerKingRightHandCol", CDeerKingRightHandCol::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_DeerKingRightHandCol", L"Monster_DeerKingRightHandCol", 3);
	}
	//============================================================================================================================================
	// 검겐지
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordGenji", CSwordGenji::Create(m_pGraphicDev))))
			return E_FAIL;
		// 검겐지 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordGenjiBullet", CSwordGenjiBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_SwordGenjiBullet", L"Monster_SwordGenjiBullet", 100);
		// 총겐지
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunGenji", CGunGenji::Create(m_pGraphicDev))))
			return E_FAIL;
		// 총겐지 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunGenjiBullet", CGunGenjiBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_GunGenjiBullet", L"Monster_GunGenjiBullet", 100);
		// 검방패겐지
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordShieldGenji", CSwordShieldGenji::Create(m_pGraphicDev))))
			return E_FAIL;
	}
	//============================================================================================================================================
	// 토템
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_Cocoon", CCocoon::Create(m_pGraphicDev))))
			return E_FAIL;
		// 토템 방사총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_CocoonBullet", CCocoonBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_CocoonBullet", L"Monster_CocoonBullet", 800);
		// 토템 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_CocoonBigBullet", CCocoonBigBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_CocoonBigBullet", L"Monster_CocoonBigBullet", 100);
	}
	//============================================================================================================================================
	// 김헌터
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_Hunter", CHunter::Create(m_pGraphicDev))))
			return E_FAIL;
		// 김헌터 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_HunterBullet", CHunterBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_HunterBullet", L"Monster_HunterBullet", 100);
	}
	//============================================================================================================================================
	// 몽키.D.루피
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_Monkey", CMonkey::Create(m_pGraphicDev))))
			return E_FAIL;
		// 몽키.D.총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_MonkeyBullet", CMonkeyBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_MonkeyBullet", L"Monster_MonkeyBullet", 100);
	}
	//============================================================================================================================================
	// 따이맨
	//============================================================================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_ThaiMan", CThaiMan::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================================================
	// 성게
	//============================================================================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_Urchin", CUrchin::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================================================
	// 늑대
	//============================================================================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_Wolf", CWolf::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================================================
	// 야차맨
	//============================================================================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_YachaMan", CYachaMan::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================================================
	// 예티
	//============================================================================================================================================
	{
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_Yeti", CYeti::Create(m_pGraphicDev))))
			return E_FAIL;
		// 예티 총알
		if (FAILED(g_pManagement->Add_Prototype(L"Monster_YetiBullet", CYetiBullet::Create(m_pGraphicDev))))
			return E_FAIL;
		CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Monster_YetiBullet", L"Monster_YetiBullet", 100);
	}
	//============================================================================================================================================
	// 펫_독나비
	//============================================================================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"Pet_PoisonButterFly", CPet_PoisonButterFly::Create(m_pGraphicDev))))
		return E_FAIL;
	// 펫 총알 원본
	if (FAILED(g_pManagement->Add_Prototype(L"Pet_Bullet", CPet_Bullet::Create(m_pGraphicDev))))
		return E_FAIL;
	CObjectPool_Manager::Get_Instance()->Create_ObjectPool(L"Pet_Bullet", L"Pet_Bullet", 100);
	//============================================================================================================================================


	//============================================================================================================================================
	// 옷
	//============================================================================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"Cloth_Static", CClothStatic::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Cloth_Dynamic", CClothDynamic::Create(m_pGraphicDev))))
		return E_FAIL;
	//============================================================================================================================================

	return S_OK;
}

CLoading* CLoading::Create(_Device pGraphicDev, SCENEID eLoadingID)
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


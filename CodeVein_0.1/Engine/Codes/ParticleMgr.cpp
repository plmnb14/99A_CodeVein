#include "..\Headers\ParticleMgr.h"
#include "Trail_VFX.h"
#include "Effect.h"


IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
{
}


HRESULT CParticleMgr::Ready_ParticleManager_Essential()
{
	m_pManagement = CManagement::Get_Instance();
	if (nullptr == m_pManagement)
		return E_FAIL;

	Input_Pool(L"ItemGet_Particle", 100);

	Input_Pool(L"Player_FootSmoke", 50);
	Input_Pool(L"Player_FootSmoke_Jump", 30);
	Input_Pool(L"Player_FootSmoke_DodgeBack", 90);
	Input_Pool(L"Player_SpaceBar_StepParticle", 500);

	Input_Pool(L"Player_ChargeSpark_Small", 10);
	Input_Pool(L"Player_ChargeSpark_Big", 10);
	Input_Pool(L"Player_ChargeSpark_Circle", 30);
	Input_Pool(L"Player_ChargeSpark_HalfCircle", 50);
	Input_Pool(L"Player_ChargeSpark_Flash", 10);
	Input_Pool(L"Player_ChargeSpark_ShockWave", 10);
	Input_Pool(L"Player_ChargeSpark_Particle", 10);
	Input_Pool(L"Player_ChargeSpark_BlastMesh", 10);
	Input_Pool(L"Player_Heal_RedLight", 3);
	Input_Pool(L"Player_Heal_Particle", 40);
	Input_Pool(L"Player_Buff_HandLight", 3);
	Input_Pool(L"Player_Buff_HandSmoke", 30);
	Input_Pool(L"Player_Buff_Particle", 40);
	Input_Pool(L"Player_Buff_Flash", 70);

	Input_Pool(L"Monster_DeadSmoke_0", 1000);
	//Input_Pool(L"Monster_DeadSmoke_1", 1000);
	//Input_Pool(L"Monster_DeadSmoke_2", 1000);

	Input_Pool(L"Cocoon_TongueFire", 200);
	Input_Pool(L"Totem_Fire_Bullet_Dead_0", 200);
	Input_Pool(L"Totem_Fire_Bullet_Dead_1", 200);
	Input_Pool(L"Totem_Fire_BulletBody", 200);
	Input_Pool(L"Totem_Fire_Bullet_Dead_Particle", 200);

	Input_Pool(L"Monkey_Knife", 60);

	Input_Pool(L"Hit_BloodParticle_0", 200);
	Input_Pool(L"Hit_BloodParticle_1", 200);
	Input_Pool(L"Hit_BloodParticle_2", 200);
	Input_Pool(L"Hit_BloodParticle_3", 200);
	Input_Pool(L"Hit_BloodParticle_4", 200);
	Input_Pool(L"Hit_BloodParticle_5", 200);
	Input_Pool(L"Hit_Blood_Direction_0", 100);
	Input_Pool(L"Hit_Blood_Direction_1", 100);
	Input_Pool(L"Hit_Blood_Direction_2", 100);
	Input_Pool(L"Hit_Blood_Direction_3", 100);
	Input_Pool(L"Hit_Blood_Direction_4", 100);
	Input_Pool(L"Hit_Blood_Direction_5", 100);
	Input_Pool(L"Hit_Blood_Direction_6", 100);

	Input_Pool(L"Hit_BloodDecal_0", 50);

	Input_Pool(L"Blood_Chunk_0", 50);

	Input_Pool(L"Hit_Slash_Particle_0", 50);
	Input_Pool(L"Hit_Slash_Particle_1", 50);
	Input_Pool(L"Hit_Slash_Particle_2", 50);
	Input_Pool(L"Hit_Slash_Particle_3", 50);
	Input_Pool(L"Hit_Slash_0", 50);
	Input_Pool(L"Hit_Slash_1", 50);
	Input_Pool(L"Hit_Slash_2", 50);
	Input_Pool(L"Hit_Slash_3", 50);
	Input_Pool(L"Hit_SlashLine_0", 50);
	Input_Pool(L"Hit_Particle_Red", 100);
	Input_Pool(L"Hit_Particle_Yellow", 100);

	Input_Pool(L"Hit_LongSpark_0", 100);
	Input_Pool(L"Hit_LongSpark_1", 100);
	Input_Pool(L"Hit_LongSpark_2", 100);
	Input_Pool(L"Hit_LongSpark_Test_0", 100);

	Input_Pool(L"SpawnParticle", 1000);
	Input_Pool(L"SpawnParticle_Sub", 1000);

	Input_Pool(L"Bullet_Body", 30);
	Input_Pool(L"Bullet_Body_Aura", 100);
	Input_Pool(L"Bullet_DeadFlash", 30);
	Input_Pool(L"Bullet_DeadSmoke_Base", 50);
	Input_Pool(L"Bullet_DeadSmoke_Black", 100);
	Input_Pool(L"Bullet_Fire_Flash", 10);
	Input_Pool(L"Bullet_Ready_Aura", 100);
	Input_Pool(L"Bullet_Ready_Flash", 10);
	Input_Pool(L"Bullet_Tail_Particle", 100);
	Input_Pool(L"RockBullet_Body", 10);

	Input_Pool(L"MistletoeParticle", 500);
	Input_Pool(L"MistletoeParticle_Sub", 500);
	Input_Pool(L"MistletoeParticle_Active", 500);

	Input_Pool(L"ItemObject", 100);
	Input_Pool(L"ItemObject_Red", 100);
	Input_Pool(L"ItemObject_Green", 100);
	Input_Pool(L"ItemObject_Blue", 100);
	Input_Pool(L"ItemObject_Purple", 100);
	Input_Pool(L"ItemObject_Yellow", 100);

	Input_Pool(L"Haze_Body", 50);
	Input_Pool(L"Haze_FlashParticle", 2500);

	Input_Pool(L"Weapon_HeavyDust", 300);

	Input_Pool(L"MapDust", 100);
	Input_Pool(L"MapDust_2", 100);
	Input_Pool(L"MapMist", 1000);
	Input_Pool(L"MapSnow", 2500);

	Input_Pool(L"Blood_Decal_0", 100);
	Input_Pool(L"Blood_Decal_1", 100);
	Input_Pool(L"Blood_Decal_2", 100);
	Input_Pool(L"Blood_Decal_3", 100);
	Input_Pool(L"Blood_Decal_4", 100);
	Input_Pool(L"Blood_Decal_5", 100);
	Input_Pool(L"Blood_Decal_6", 100);

	return S_OK;
}

HRESULT CParticleMgr::Ready_ParticleManager()
{
	Input_Pool(L"Player_Drain_Ink_0", 80);
	Input_Pool(L"Player_Drain_Ink_1", 80);
	Input_Pool(L"Player_Drain_Ink_2", 80);
	Input_Pool(L"Player_Drain_Ink_3", 80);
	Input_Pool(L"Player_Drain_Ink_4", 80);
	Input_Pool(L"Player_Drain_Ink_5", 80);
	Input_Pool(L"Player_Drain_Ink_6", 80);

	Input_Pool(L"Player_Skill_Scratch_Hor", 10);
	Input_Pool(L"Player_Skill_Scratch_Ver", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Hor", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Ver", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Sub_Hor", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Sub_Ver", 10);
	Input_Pool(L"Player_Skill_ShadowAssault_ScratchBlur", 10);
	Input_Pool(L"Player_Skill_ShadowAssault_Scratch", 10);
	Input_Pool(L"Player_Skill_DarkSmokeAura", 50);
	Input_Pool(L"Player_Skill_Ring_Hor", 10);
	Input_Pool(L"Player_Skill_Ring_Ver", 10);
	Input_Pool(L"Player_Skill_Particle_Explosion", 110);
	Input_Pool(L"Player_Skill_RedParticle_Explosion", 110);
	Input_Pool(L"Player_Skill_RedParticle_Upper", 300);
	Input_Pool(L"Player_Skill_RedCircle_Flash", 10);
	Input_Pool(L"Player_Skill_Distortion_Circle", 10);
	Input_Pool(L"Player_Skill_Distortion_Water", 10);
	Input_Pool(L"Player_Skill_Distortion_Blaster", 10);
	Input_Pool(L"Player_Skill_RedOnion", 40);
	Input_Pool(L"Player_Skill_RedOnion_3", 100);
	Input_Pool(L"Player_Skill_Floor_BlackRing", 50);
	Input_Pool(L"Player_Skill_Floor_RedRing", 50);
	Input_Pool(L"Player_Skill_SplitAssert_LaserBefore", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserAfter", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserBody", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserAfter_Smoke", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserAfter_RingLine", 5);
	Input_Pool(L"Player_Skill_DarkRedOnion_Explosion", 50);
	Input_Pool(L"Player_Skill_Blood_Explosion", 100);
	Input_Pool(L"Player_Skill_RotYRing_Black", 40);
	Input_Pool(L"Player_Skill_RotYRing_Red", 40);
	Input_Pool(L"Player_Skill_WindMesh", 40);
	Input_Pool(L"Player_Skill_WindTornadeMesh", 60);
	Input_Pool(L"Player_Skill_BloodTornadeMesh", 60);
	Input_Pool(L"Player_Skill_BloodTornadeMesh_2", 60);
	Input_Pool(L"Player_Skill_BloodTornadeMesh_3", 60);
	Input_Pool(L"Player_Skill_BloodConeMesh", 60);
	Input_Pool(L"Player_Skill_BloodConeMesh_Explosion", 60);
	Input_Pool(L"Player_Skill_Rush_Particle_Yellow", 300);
	Input_Pool(L"Player_Skill_Rush_Particle_Orange", 300);
	Input_Pool(L"Player_Skill_Rush_Particle_White", 300);
	Input_Pool(L"Player_Skill_Rush_LaserBefore", 10);
	Input_Pool(L"Player_Skill_Rush_RedParticle_LaserBefore", 80);
	Input_Pool(L"Player_Skill_Rush_WhiteParticle_LaserBefore", 80);
	Input_Pool(L"Player_Skill_Torment_Wind_L", 10);
	Input_Pool(L"Player_Skill_Torment_Wind_R", 10);
	Input_Pool(L"Player_Skill_Torment_Wind_Distortion_L", 10);
	Input_Pool(L"Player_Skill_Torment_Wind_Distortion_R", 10);

	Input_Pool(L"ButterFly_SoftSmoke", 2000);
	Input_Pool(L"ButterFly_PointParticle", 3500);
	Input_Pool(L"ButterFly_PointParticle_Plum", 1000);
	Input_Pool(L"ButterFly_RingLine", 10);
	Input_Pool(L"ButterFly_RingLine_Distortion", 10);
	Input_Pool(L"ButterFly_Distortion", 10);
	Input_Pool(L"ButterFly_Distortion_Circle", 50);
	Input_Pool(L"ButterFly_Distortion_Smoke", 600);
	Input_Pool(L"ButterFly_Distortion_SmokeGravity", 50);
	Input_Pool(L"ButterFly_SoftSmoke_Bottom", 600);
	Input_Pool(L"ButterFly_Smoke_Red_Once", 200);
	Input_Pool(L"ButterFly_Smoke_Red_Particle", 1000);
	Input_Pool(L"ButterFly_SoftSmoke_Floor", 1000);
	Input_Pool(L"ButterFly_SoftSmoke_Ready_1", 200);
	Input_Pool(L"ButterFly_SoftSmoke_Ready_2", 200);
	Input_Pool(L"ButterFly_SoftSmoke_Mist", 1500);
	Input_Pool(L"ButterFly_SoftSmoke_Chunk", 500);
	Input_Pool(L"ButterFly_WaterSplash", 100);
	Input_Pool(L"ButterFly_GlitterSand", 300);

	Input_Pool(L"ButterFly_VenomShot", 1000);
	Input_Pool(L"ButterFly_VenomShot_Body", 100);
	Input_Pool(L"ButterFly_VenomShot_Body_Sub", 800);
	Input_Pool(L"ButterFly_VenomShot_SubSmoke", 1000);
	Input_Pool(L"ButterFly_VenomShot_Distortion", 500);
	Input_Pool(L"ButterFly_VenomShot_Chunk", 1000);
	Input_Pool(L"ButterFly_VenomShot_PointParticle", 2500);
	Input_Pool(L"ButterFly_VenomShot_Tail", 500);
	Input_Pool(L"ButterFly_VenomShot_DeadMist", 100);
	Input_Pool(L"ButterFly_VenomShot_DeadSmoke", 100);
	Input_Pool(L"ButterFly_VenomShot_DeadSplash", 100);
	Input_Pool(L"ButterFly_Crying_Distortion", 100);
	
	Input_Pool(L"ButterFly_BackStepSand01", 50);
	Input_Pool(L"ButterFly_BackStepSand02", 50);
	Input_Pool(L"ButterFly_BackStepSand_Floor", 50);
	Input_Pool(L"ButterFly_PopSand", 50);

	Input_Pool(L"QueensKnight_2Phase_SwordCrash_Chunk", 100);
	Input_Pool(L"QueensKnight_2Phase_SwordCrash_Chunk_Dark", 100);
	Input_Pool(L"QueensKnight_2Phase_SwordCrash_ShockWave", 10);
	Input_Pool(L"QueensKnight_DistortionCircle", 50);
	Input_Pool(L"QueensKnight_Intro_Smoke_0", 500);
	Input_Pool(L"QueensKnight_Intro_Smoke_1", 500);
	Input_Pool(L"QueensKnight_JumpDown_Particle_Red", 500);
	Input_Pool(L"QueensKnight_JumpDown_ShockWave", 50);
	Input_Pool(L"QueensKnight_JumpDown_Smoke_Black", 500);
	Input_Pool(L"QueensKnight_JumpDown_Smoke_Red", 500);
	Input_Pool(L"QueensKnight_JumpDown_Smoke", 500);
	Input_Pool(L"QueensKnight_Ultimate_Floor", 500);
	Input_Pool(L"QueensKnight_Lava_Floor_0", 550);
	Input_Pool(L"QueensKnight_Lava_Floor_1", 550);
	Input_Pool(L"QueensKnight_Lava_Smoke_0", 550);
	Input_Pool(L"QueensKnight_DarkBoom_PopSmoke_0", 150);
	Input_Pool(L"QueensKnight_DarkBoom_PopSmoke_1", 150);
	Input_Pool(L"QueensKnight_DarkBoom_Smoke_0", 550);
	Input_Pool(L"QueensKnight_DarkBoom_BlackSmoke", 550);
	Input_Pool(L"QueensKnight_DarkBoom_LineRing_0", 100);
	Input_Pool(L"QueensKnight_DarkBoom_Sphere_0", 20);
	Input_Pool(L"QueensKnight_DarkBoom_Sphere_1", 50);
	Input_Pool(L"QueensKnight_Ultimate_Cone_0", 50);
	Input_Pool(L"QueensKnight_Ultimate_Cone_1", 50);
	Input_Pool(L"QueensKnight_Ultimate_Cone_2", 50);
	Input_Pool(L"QueensKnight_Ultimate_Cone_3", 50);
	Input_Pool(L"QueensKnight_Ultimate_Cone_4", 50);
	Input_Pool(L"QueensKnight_Ultimate_Smoke", 350);
	Input_Pool(L"QueensKnight_DarkBoom_Ring", 50);
	Input_Pool(L"QueensKnight_DarkBoom_RingBottom", 50);
	Input_Pool(L"QueensKnight_DarkBoom_RingRed", 50);
	Input_Pool(L"QueensKnight_DarkBoom_RingReady", 50);
	Input_Pool(L"QueensKnight_DarkBoom_BlackFire_0", 600);
	Input_Pool(L"QueensKnight_DarkBoom_LaserBody", 30);
	Input_Pool(L"QueensKnight_Lava_Lightning_0", 600);
	Input_Pool(L"QueensKnight_Lava_Lightning_1", 600);
	Input_Pool(L"QueensKnight_Lava_Lightning_2", 600);
	Input_Pool(L"QueensKnight_DarkBoom_Floor_0", 600);
	Input_Pool(L"QueensKnight_DarkBoom_Floor_1", 600);
	Input_Pool(L"QueensKnight_DarkBoom_Particle", 1000);
	Input_Pool(L"QueensKnight_DarkBoom_ReadyDistortion", 200);
	Input_Pool(L"QueensKnight_LeakField_0", 350);
	Input_Pool(L"QueensKnight_LeakField_1", 350);
	Input_Pool(L"QueensKnight_LeakField_Hand", 150);
	Input_Pool(L"QueensKnight_LeakField_Hand_Aura", 150);
	Input_Pool(L"QueensKnight_LeakField_Particle", 450);
	Input_Pool(L"QueensKnight_Lightning_0", 50);
	Input_Pool(L"QueensKnight_Lightning_1", 50);
	Input_Pool(L"QueensKnight_Lightning_2", 50);
	Input_Pool(L"QueensKnight_Lightning_3", 50);
	Input_Pool(L"QueensKnight_Lightning_4", 50);
	Input_Pool(L"QueensKnight_Lightning_5", 50);
	Input_Pool(L"QueensKnight_LightningBody_0", 50);
	Input_Pool(L"QueensKnight_ShieldAttack_Distortion", 30);
	Input_Pool(L"QueensKnight_ShieldAttack_Particle", 300);
	Input_Pool(L"QueensKnight_ShieldAttack_RedLight", 10);
	Input_Pool(L"QueensKnight_SwordCrash_Particle", 600);
	Input_Pool(L"QueensKnight_SwordCrash_Particle_Orange", 600);
	Input_Pool(L"QueensKnight_Teleport_Particle", 5000);
	Input_Pool(L"QueensKnight_Teleport_Particle_Black", 1000);
	Input_Pool(L"QueensKnight_Teleport_Smoke", 200);
	Input_Pool(L"QueensKnight_Teleport_Smoke_Red", 200);
	Input_Pool(L"QueensKnight_Teleport_DistortionSmoke", 300);
	Input_Pool(L"QueensKnight_Teleport_Ink_0", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_1", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_2", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_3", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_4", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_5", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_6", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_7", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_8", 30);
	Input_Pool(L"QueensKnight_Teleport_Ink_9", 30);
	Input_Pool(L"QueensKnight_Trail_Lightning_0", 200);
	Input_Pool(L"QueensKnight_Trail_Lightning_1", 200);
	Input_Pool(L"QueensKnight_Trail_Lightning_2", 500);
	Input_Pool(L"QueensKnight_Trail_Lightning_2_Dark", 200);
	Input_Pool(L"QueensKnight_WhirlWind_Smoke", 300);
	Input_Pool(L"QueensKnight_Sting_Tornade", 5);

	Input_Pool(L"IceFloorAura_01", 100);
	Input_Pool(L"IceFloorAura_02", 100);
	Input_Pool(L"IceFloorAura_03", 100);
	Input_Pool(L"IceSmoke_01", 100);
	Input_Pool(L"IceSmoke_02", 100);
	Input_Pool(L"IceGirl_FlashParticle_Blue", 5000);
	Input_Pool(L"IceGirl_FlashParticle_Green", 5000);
	Input_Pool(L"IceGirl_PointParticle_Blue", 5000);
	Input_Pool(L"IceGirl_PointParticle_Green", 5000);
	Input_Pool(L"IceBlock_Main", 30);
	Input_Pool(L"IceBlock_Sub_01", 30);
	Input_Pool(L"IceBlock_Sub_02", 30);
	Input_Pool(L"IceBlock_Main_Big", 30);
	Input_Pool(L"IceBlock_Sub_01_Big", 30);
	Input_Pool(L"IceBlock_Sub_02_Big", 30);
	Input_Pool(L"IceBlock_Main_Mid", 30);
	Input_Pool(L"IceBlock_Sub_01_Mid", 30);
	Input_Pool(L"IceBlock_Sub_02_Mid", 30);
	Input_Pool(L"IceBlock_Main_Small", 30);
	Input_Pool(L"IceCrystal_01", 100);
	Input_Pool(L"IceCrystal_02", 100);
	Input_Pool(L"IceCrystal_03", 100);
	Input_Pool(L"IceBlock_Break", 100);
	Input_Pool(L"IceGirl_SwordBullet_Mesh", 10);
	Input_Pool(L"IceGirl_SwordBullet_Mesh_2", 10);
	Input_Pool(L"IceGirl_Charge_Hand_Particle"	, 300);
	Input_Pool(L"IceGirl_Charge_Hand_Smoke"		, 300);
	Input_Pool(L"IceGirl_Charge_Hand_Smoke_2", 300);
	Input_Pool(L"IceGirl_Buff_Aura", 500);
	Input_Pool(L"IceGirl_Buff_Aura_2", 300);
	Input_Pool(L"IceGirl_Buff_Aura_3", 300);
	Input_Pool(L"IceGirl_Buff_Bubble_BreakSmoke", 300);
	Input_Pool(L"IceGirl_Buff_Bubble", 10);
	Input_Pool(L"IceBlock_Particle", 300);
	Input_Pool(L"IceGirl_Buff_Break_1"	, 300);
	Input_Pool(L"IceGirl_Buff_Break_2"	, 300);
	Input_Pool(L"IceBlock_FloorAura_01", 300);
	Input_Pool(L"IceBlock_FloorAura_02", 300);
	Input_Pool(L"IceBlock_FloorAura_03", 300);
	Input_Pool(L"IceBlock_Smoke_01", 600);
	Input_Pool(L"IceBlock_Smoke_02", 600);
	Input_Pool(L"IceGirl_Buff_Break_Lightning", 100);
	Input_Pool(L"IceGirl_Buff_Charge_Smoke_01", 100);

	Input_Pool(L"FireBoy_Charge_Hand_Fire", 150);
	Input_Pool(L"FireBoy_FireTornade_ReadyFire", 50);
	Input_Pool(L"FireBoy_FireTornade_Mesh", 30);
	Input_Pool(L"FireBoy_FireTornade_Mesh_2", 30);
	Input_Pool(L"FireBoy_FireTornade_Floor_01", 100);
	Input_Pool(L"FireBoy_FireTornade_Floor_02", 100);
	Input_Pool(L"FireBoy_FireTornade_Floor_03", 100);
	Input_Pool(L"FireBoy_Fire_Particle_01", 500);
	Input_Pool(L"FireBoy_Fire_Particle_02", 500);
	Input_Pool(L"FireBoy_FireTornade_ReadyFire_Up", 100);
	Input_Pool(L"FireBoy_FireBullet", 500);
	Input_Pool(L"FireBoy_FireBullet_Mid", 500);
	Input_Pool(L"FireBoy_FireBullet_Big", 500);
	Input_Pool(L"FireBoy_FireBullet_Sub_0", 500);
	Input_Pool(L"FireBoy_FireBullet_Sub_1", 500);
	Input_Pool(L"FireBoy_FireBullet_GunEff", 300);
	Input_Pool(L"FireBoy_FireBullet_Particle_01", 4000);
	Input_Pool(L"FireBoy_FireBullet_Particle_02", 4000);
	Input_Pool(L"FireBoy_FireGround_Floor", 30);
	Input_Pool(L"FireBoy_FireGround_Particle", 600);
	Input_Pool(L"FireBoy_FireGround_BoomCircle", 30);
	Input_Pool(L"FireBoy_FireGround_BoomParticle_01", 300);
	Input_Pool(L"FireBoy_FireGround_BoomParticle_02", 300);
	Input_Pool(L"FireBoy_FireGround_ReadyFire", 300);
	Input_Pool(L"FireBoy_FireGround_BoomFire", 100);
	Input_Pool(L"FireBoy_FireGround_AfterFire_01", 300);
	Input_Pool(L"FireBoy_FireGround_AfterFire_02", 300);
	Input_Pool(L"FireBoy_FireSphere", 10);
	Input_Pool(L"FireBoy_FireSphere_BreakFire", 300);
	Input_Pool(L"FireBoy_FireSphere_BreakParticle", 300);
	Input_Pool(L"FireBoy_FireSphere_OuterFire", 300);
	Input_Pool(L"FireBoy_FireSphere_BodyFire", 200);
	Input_Pool(L"FireBoy_FireSphere_BodyFire_Sub", 200);
	Input_Pool(L"FireBoy_FireSphere_Tail", 300);
	Input_Pool(L"FireBoy_FireSphere_ShotCircle", 20);
	Input_Pool(L"FireBoy_ArmAttack_Dust", 100);
	Input_Pool(L"FireBoy_ArmAttack_FloorDust", 100);

	Input_Pool(L"DeerKing_IceStone_Up_Particle_0", 100);
	Input_Pool(L"DeerKing_IceStone_Up_Small_Particle_0", 100);
	Input_Pool(L"DeerKing_IceBullet_0", 30);
	Input_Pool(L"DeerKing_IceBullet_ReadySmoke_0", 300);
	Input_Pool(L"DeerKing_IceBullet_ReadySmoke_1", 300);
	Input_Pool(L"DeerKing_IceBullet_ReadySmoke_2", 300); // 빛나는 연기
	Input_Pool(L"DeerKing_IceBullet_ReadySmoke_3", 300);
	Input_Pool(L"DeerKing_IceBullet_Charge_Hand_Smoke_0", 100);
	Input_Pool(L"DeerKing_IceBullet_Charge_Hand_Smoke_After_0", 100);
	Input_Pool(L"DeerKing_IceBullet_Charge_Hand_Smoke_After_1", 100);
	Input_Pool(L"DeerKing_Point_ExplosionParticle_0", 10000);
	Input_Pool(L"DeerKing_Body_PointParticle", 500);
	Input_Pool(L"DeerKing_Body_Smoke", 500);
	Input_Pool(L"DeerKing_Body_Smoke_2", 500);
	Input_Pool(L"DeerKing_Distortion_Circle", 10);
	Input_Pool(L"DeerKing_SnowChunk_Up_Particle_0", 100);
	Input_Pool(L"DeerKing_SnowChunk_Up_Particle_1", 100);
	Input_Pool(L"DeerKing_SnowChunk_Up_Particle_2", 100);
	Input_Pool(L"DeerKing_SnowChunk_Up_Small_Particle_0", 300);
	Input_Pool(L"DeerKing_SnowChunk_Heavy_Particle_0", 100);
	Input_Pool(L"DeerKing_Snow_Up_Particle_0", 10000);
	Input_Pool(L"DeerKing_Snow_Up_LongLine_0", 200);
	Input_Pool(L"DeerKing_Snow_Up_LongLine_1", 500);
	Input_Pool(L"DeerKing_IceSmoke_0", 300);
	Input_Pool(L"DeerKing_IceSmoke_1", 300);
	Input_Pool(L"DeerKing_IceSmoke_2", 300);
	Input_Pool(L"DeerKing_IceSmoke_Mid_0", 300);
	Input_Pool(L"DeerKing_IceSmoke_Mid_1", 300);
	Input_Pool(L"DeerKing_IceSmoke_Small_0", 300);
	Input_Pool(L"DeerKing_IceSmoke_Small_1", 300);
	Input_Pool(L"DeerKing_IceBullet_DeadParticle_Stone_0", 300);
	Input_Pool(L"DeerKing_IceBullet_DeadParticle_0", 300);
	Input_Pool(L"DeerKing_Jump_In_Place_Distortion", 20);
	Input_Pool(L"DeerKing_IceBlock_0", 20);
	Input_Pool(L"DeerKing_Tornade_Mesh_0", 20);
	Input_Pool(L"DeerKing_JumpPack_Distortion", 20);
	Input_Pool(L"DeerKing_JumpPack_Fire", 700);

	Input_Pool(L"Boss_KnockDown_Dust", 10);
	Input_Pool(L"Boss_Dead_Particle", 11000);

	Input_Pool(L"SpawnParticle_ForBoss", 5000);
	//Input_Pool(L"SpawnParticle_ForBoss_Point", 5000);
	//Input_Pool(L"SpawnParticle_ForBoss_Point_Sub", 5000);

	return S_OK;
}

HRESULT CParticleMgr::Ready_Trail()
{
	Input_Pool_Trail(L"GameObject_SwordTrail", 1000);

	return S_OK;
}

HRESULT CParticleMgr::Update_ParticleManager(const _double TimeDelta)
{
	//if (GetAsyncKeyState('B') & 0x8000)
	//{
	//	Create_Effect_Delay(L"QueensKnight_Sting_Tornade", 0.f, _v3(0.f, 1.3f, 0.f), nullptr);
	//}

	if (CInput_Device::Get_Instance()->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		CParticleMgr::Get_Instance()->Create_Effect_Decal(L"Blood_Decal_0", V3_NULL);
	}
	if (CInput_Device::Get_Instance()->Get_DIMouseState(CInput_Device::DIM_RB))
	{
		_tchar szBuff[256] = L"";
		wsprintf(szBuff, L"Blood_Decal_%d", CCalculater::Random_Num(0, 6));
	
		CParticleMgr::Get_Instance()->Create_Effect_Decal(szBuff, V3_NULL);
	}

	auto& iter_begin = m_vecParticle.begin();
	auto& iter_end = m_vecParticle.end();

	if (m_vecParticle.size() <= 0)
	{
		Update_Effect(TimeDelta);
		//Update_Trail(TimeDelta);
		return S_OK;
	}

	for (; iter_begin != iter_end;)
	{
		m_fCreateDelay_Check -= _float(TimeDelta);
		if (nullptr != *iter_begin && 0.f >= m_fCreateDelay_Check)
		{
			m_fCreateDelay_Check = 0.3f;

			if ((*iter_begin)->fLifeTime > 0)
			{
				(*iter_begin)->fLifeTime -= _float(TimeDelta);

				queue<CEffect*>* pFindedQueue = Find_Queue((*iter_begin)->szName);
				if (pFindedQueue == nullptr)
					return E_FAIL;

				 // 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
				for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
				{
					if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
					{
						//_tchar szTemp[256];
						//lstrcpy(szTemp, pFindedQueue->front()->Get_ParticleName());
						//CHAR szForPrint[MAX_STR] = "";
						//WideCharToMultiByte(CP_ACP, 0, szTemp, MAX_STR, szForPrint, MAX_STR, NULL, NULL);
						//cout << "Particle Create" << szForPrint << endl;

						_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
						CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
						pEffect->Set_ParticleName(szEffName);
						pEffect->Set_Desc((*iter_begin)->vCreatePos, (*iter_begin)->pFollowTrans);
						pEffect->Set_TargetMatrix((*iter_begin)->pTargetMatrix);
						pEffect->Set_Delay(((*iter_begin)->fDelayTime != 0), (*iter_begin)->fDelayTime);
						pEffect->Set_AutoFind((*iter_begin)->bAutoFind);
						if ((*iter_begin)->bFinishPos) pEffect->Set_FinishPos((*iter_begin)->vFinishPos);
						pEffect->Reset_Init();

						m_EffectList.push_back(pEffect);
						continue;
					}

					m_EffectList.push_back(pFindedQueue->front());
					pFindedQueue->front()->Set_Desc((*iter_begin)->vCreatePos, (*iter_begin)->pFollowTrans);
					pFindedQueue->front()->Set_TargetMatrix((*iter_begin)->pTargetMatrix);
					pFindedQueue->front()->Set_Delay(((*iter_begin)->fDelayTime != 0), (*iter_begin)->fDelayTime);
					pFindedQueue->front()->Set_AutoFind((*iter_begin)->bAutoFind);
					if((*iter_begin)->bFinishPos) pFindedQueue->front()->Set_FinishPos((*iter_begin)->vFinishPos);
					pFindedQueue->front()->Reset_Init(); // 사용 전 초기화
					pFindedQueue->pop();
				}

				iter_begin++;
			}
			else
			{
				Safe_Delete((*iter_begin));
				iter_begin = m_vecParticle.erase(iter_begin);
			}
		}
	}

	Update_Effect(TimeDelta);
	//Update_Trail(TimeDelta);

	return S_OK;
}

HRESULT CParticleMgr::Update_Trail(const _double TimeDelta)
{
	_int iProgress = 0;


	for (auto& iter = m_TrailList.begin(); iter != m_TrailList.end(); )
	{
		//iProgress = (*iter)->Update_GameObject(TimeDelta);
		_bool bDead = (*iter)->Get_Dead();

		//if (DEAD_OBJ == iProgress)
		if (bDead)
		{
			m_TrailPool[L"GameObject_SwordTrail"].push((*iter));

			iter = m_TrailList.erase(iter);
		}
		else
			iter++;

	}

	return S_OK;
}

void CParticleMgr::Create_ParticleEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_ParticleEffect_Delay(_tchar * szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform * pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->fDelayTime = fDelay;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_ParticleEffect_Delay(_tchar * szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform * pFollowTrans, _mat * pTargetMat)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->fDelayTime = fDelay;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->pTargetMatrix = pTargetMat;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_ParticleEffect_FinishPos(_tchar* szName, _float fLifeTime, _float fDelay, _v3 vPos, _v3 vFinishPos, CTransform * pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->fDelayTime = fDelay;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->vFinishPos = vFinishPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = true;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_Effect(_tchar* szName, _v3 vPos, CTransform * pFollowTrans, _v3 vDir, _v3 vAngle)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar szTemp[256];
			lstrcpy(szTemp, pFindedQueue->front()->Get_ParticleName());
			CHAR szForPrint[MAX_STR] = "";
			WideCharToMultiByte(CP_ACP, 0, szTemp, MAX_STR, szForPrint, MAX_STR, NULL, NULL);
			if(GetAsyncKeyState('B') & 0x8000)
				cout << "Effect Create : " << szForPrint << endl;

			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Dir(vDir);
			pEffect->Set_Angle(vAngle);
			pEffect->Set_Delay(false);
			pEffect->Reset_Init();

			m_EffectList.push_back(pEffect);
			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());
		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Dir(vDir);
		pFindedQueue->front()->Set_Angle(vAngle);
		pFindedQueue->front()->Set_Delay(false);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화
		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_DirEffect(_tchar * szName, _v3 vPos, _v3 vDir, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
			
			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Dir(vDir);
			pEffect->Set_Delay(false);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Dir(vDir);
		pFindedQueue->front()->Set_Delay(false);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_AngleEffect(_tchar * szName, _v3 vPos, _v3 vAngle, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Delay(false);
			pEffect->Set_Angle(vAngle);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Delay(false);
		pFindedQueue->front()->Set_Angle(vAngle);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_AutoFindEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = true;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_Effect_NoPool(_tchar* szName, _v3 vPos, CTransform* pFollowTrans)
{
	CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
	pEffect->Set_Desc(vPos, pFollowTrans);
	pEffect->Set_Delay(false);
	pEffect->Reset_Init();
	m_pManagement->Add_GameOject_ToLayer_NoClone(pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);
}

void CParticleMgr::Create_Effect_Offset(_tchar* szName, _float fOffset, _v3 vPos, CTransform * pFollowTrans)
{
	auto	iter = find_if(m_mapEffectOffset.begin(), m_mapEffectOffset.end(), CTag_Finder(szName));
	if (iter == m_mapEffectOffset.end())
		m_mapEffectOffset.insert(pair<_tchar*, float>(szName, fOffset));

	m_mapEffectOffset[szName] += DELTA_60;
	if (m_mapEffectOffset[szName]  < fOffset)
		return;

	m_mapEffectOffset[szName] = 0.f;
	Create_Effect(szName, vPos, pFollowTrans);
}

void CParticleMgr::Create_Effect_Delay(_tchar * szName, _float fDelay, _v3 vPos, CTransform * pFollowTrans, _v3 vAngle)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Delay(true, fDelay);
			pEffect->Set_Angle(vAngle);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Delay(true, fDelay);
		pFindedQueue->front()->Set_Angle(vAngle);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_Effect_Delay(_tchar * szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _mat * pTargetMat)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_TargetMatrix(pTargetMat);
			pEffect->Set_Delay(true, fDelay);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_TargetMatrix(pTargetMat);
		pFindedQueue->front()->Set_Delay(true, fDelay);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_Effect_FinishPos(_tchar * szName, _float fDelay, _v3 vPos, _v3 vFinishPos, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Delay(true, fDelay);
			pEffect->Set_FinishPos(vFinishPos);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Delay(true, fDelay);
		pFindedQueue->front()->Set_FinishPos(vFinishPos);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_Effect_Curve(_tchar* szName, _v3 vPos, CTransform * pTargetTrans, _float fPower)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, nullptr);
			pEffect->Set_Curve(pTargetTrans, fPower);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, nullptr);
		pFindedQueue->front()->Set_Curve(pTargetTrans, fPower);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_Effect_Decal(_tchar* szName, _v3 vPos)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	m_EffectList.push_back(pFindedQueue->front());

	pFindedQueue->front()->Set_Desc(vPos, nullptr);
	pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

	pFindedQueue->pop();
}

void CParticleMgr::Create_Hit_Effect(CCollider* pAttackCol, CCollider* pHittedCol, CTransform* pHittedTrans, _float fPower)
{
	_v3 vAttackPos = pAttackCol->Get_CenterPos();
	_v3 vHittedPos = pHittedCol->Get_CenterPos(); // pHittedTrans->Get_Pos() + _v3(0.f, 1.f, 0.f);

	_v3 vRight = V3_NULL;
	memcpy(&vRight, &pHittedTrans->Get_WorldMat().m[0][0], sizeof(_v3));

	_v3 vDir = vHittedPos - vAttackPos;
	vDir.y *= 0.5f; // y는 반감
	D3DXVec3Normalize(&vDir, &vDir);
	_float fAngle = acosf(D3DXVec3Dot(&vDir, &vRight));
	fAngle *= -1.f;

	//cout << D3DXToDegree(fAngle) << endl;
	//cout << "[Attack Pos] " << vAttackPos.x << ", " << vAttackPos.y << ", " << vAttackPos.z << endl;
	//cout << "[Hitted Pos] " << vHittedPos.x << ", " << vHittedPos.y << ", " << vHittedPos.z << endl;
	//cout << vDir.x<<", "<< vDir.y<<", "<< vDir.z << endl;

	// 피가 칼에서 나옴

	_v3 vAngle = pHittedTrans->Get_Angle();
	vAngle.z += D3DXToDegree(fAngle);
	Create_AngleEffect(L"Hit_SlashLine_0", vAttackPos, vAngle);
	
	_tchar szBuff[256] = L"";
	wsprintf(szBuff, L"Hit_Slash_Particle_%d", CCalculater::Random_Num(0, 3));
	Create_Effect(szBuff, vAttackPos);
	
	Create_Effect(L"Hit_Slash_0", vAttackPos);
	Create_Effect(L"Hit_Slash_1", vAttackPos);
	Create_Effect(L"Hit_Slash_2", vAttackPos);
	Create_Effect(L"Hit_Slash_3", vAttackPos);
	Create_Effect(L"Hit_Particle_Red", vAttackPos);
	Create_Effect(L"Hit_Particle_Yellow", vAttackPos);
	
	Create_Effect(L"Hit_BloodParticle_0", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_1", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_2", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_3", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_4", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_5", vAttackPos);
	
	_float fMinus = (fAngle > 0) ? 1.f : -1.f;
	_v3 vMyDir = pHittedTrans->Get_Axis(AXIS_Z);
	_v3 vBloodDir = vDir * fPower;// +(vRight * fMinus);
	
	Create_DirEffect(L"Hit_Blood_Direction_0", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_1", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_2", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_3", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_4", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_5", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_6", vAttackPos, vBloodDir);

	EFF_INFO tInfo;
	tInfo.vCreatePos = vAttackPos;
	tInfo.vDirection = vBloodDir;
	CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_BloodChunk", &tInfo);

	tInfo.vCreatePos = pHittedTrans->Get_Pos();
	tInfo.vDirection = vBloodDir;
	for (_int i = 0; i < 70; i++)
		CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_LongSpark", &tInfo);

	//for (_int i = 0; i < 4; i++)
	//{
	//	wsprintf(szBuff, L"Blood_Decal_%d", CCalculater::Random_Num(0, 6));
	//	Create_Effect_Decal(szBuff, vAttackPos + vDir + _v3(0.f, -1.3f, 0.f));
	//}
}

void CParticleMgr::Create_Spawn_Effect(_float fDelay, _v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans)
{
	Create_ParticleEffect_FinishPos(L"SpawnParticle", 2.f, fDelay, vPos, vFinishPos, pFollowTrans);
	Create_ParticleEffect_FinishPos(L"SpawnParticle_Sub", 2.f, fDelay, vPos, vFinishPos, pFollowTrans);

}

void CParticleMgr::Create_FootSmoke_Effect(_v3 vPos, _float fOffset)
{
	m_fFootSmokeeDelay_Check += DELTA_60;
	if (m_fFootSmokeeDelay_Check < fOffset)
		return;

	m_fFootSmokeeDelay_Check = 0.f;
	Create_Effect(L"Player_FootSmoke", vPos);
}

void CParticleMgr::Create_BossDeadParticle_Effect(_v3 vPos, _float fDelay, _float fLength)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(L"Boss_Dead_Particle");
	if (pFindedQueue == nullptr)
		return;

	for (_int i = 0; i < 5000; i++)
	{
		if (pFindedQueue->size() < 20)
			return;

		_mat matRotX, matRotY, matRotZ;
		_v3 vDir = _v3(1.f, 1.f, 1.f);
		D3DXMatrixIdentity(&matRotX);
		D3DXMatrixIdentity(&matRotY);
		D3DXMatrixIdentity(&matRotZ);

		D3DXMatrixRotationX(&matRotX, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotX);
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotZ);
		D3DXVec3Normalize(&vDir, &vDir);

		_v3 vRandPos = vDir * (fLength);

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos + vRandPos, nullptr);
		pFindedQueue->front()->Set_Delay(true, fDelay);
		pFindedQueue->front()->Set_Dir(vDir);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

CTrail_VFX* CParticleMgr::Create_Trail()
{
	_tchar* szName = L"GameObject_SwordTrail";

	CTrail_VFX* pTrail = nullptr;

	queue<CTrail_VFX*>* pFindedQueue = Find_TrailQueue(szName);
	if (pFindedQueue == nullptr)
		return nullptr;

	if (10 > pFindedQueue->size())
	{
		 pTrail = static_cast<CTrail_VFX*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
		 m_TrailList_ForDelete.push_back(pTrail);
	}
	else
	{
		pTrail = pFindedQueue->front();
		//pTrail->Reset_Info();

		pFindedQueue->pop();
	}

	m_TrailList.push_back(pTrail);

	return pTrail;
}

HRESULT CParticleMgr::Update_Effect(const _double TimeDelta)
{
	_int iProgress;
	auto& iter_begin = m_EffectList.begin();
	auto& iter_end = m_EffectList.end();
	for (; iter_begin != iter_end; )
	{
		if (nullptr != *iter_begin)
		{
			iProgress = (*iter_begin)->Update_GameObject(TimeDelta);

			if (DEAD_OBJ == iProgress)
			{
				queue<CEffect*>* pFindedQueue = Find_Queue((*iter_begin)->Get_ParticleName());
				if (pFindedQueue == nullptr)
					return E_FAIL;

				//cout << "Queue Size : " << Find_Queue((*iter_begin)->Get_ParticleName())->size() << endl;

				// 삭제하지 않고 큐에 다시 넣기
				if (pFindedQueue->size() < 11000) // 임시...
					pFindedQueue->push((*iter_begin));
				else
					Safe_Release((*iter_begin));
				
				iter_begin = m_EffectList.erase(iter_begin);
			}
			else
				iter_begin++;
		}
	}

	for (auto& pGameObject : m_EffectList)
	{
		if (nullptr != pGameObject)
		{
			iProgress = pGameObject->Late_Update_GameObject(TimeDelta);

			if (0 > iProgress)
				break;
		}
	}

	return S_OK;
}

void CParticleMgr::Input_Pool(_tchar* szName, _int iCount)
{
	m_EffectPool.emplace(szName, queue<CEffect*>());

	for (_int i = 0; i <iCount; ++i)
	{
		// 미리 클론만 해놓기
		CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
		pEffect->Set_ParticleName(szName); // 이펙트 Info 안의 이름과 오브젝트 Tag이름이 달라서 이렇게 해줌. (해당 큐 안에 다시 넣으려고)
		m_EffectPool[szName].push(pEffect);
	}
}

void CParticleMgr::Input_Pool_Trail(_tchar* szName, _int iCount)
{
	m_TrailPool.emplace(szName, queue<CTrail_VFX*>());

	for (_int i = 0; i <iCount; ++i)
	{
		// 미리 클론만 해놓기
		CTrail_VFX* pTrail = static_cast<CTrail_VFX*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
		m_TrailPool[szName].push(pTrail);

		m_TrailList_ForDelete.push_back(pTrail);
	}
}

queue<CEffect*>* CParticleMgr::Find_Queue(_tchar* szName)
{
	for (auto& iter : m_EffectPool)
	{
		if (!lstrcmp(iter.first, szName))
			return &iter.second;
	}

	return nullptr;
}

queue<CTrail_VFX*>* CParticleMgr::Find_TrailQueue(_tchar * szName)
{
	for (auto& iter : m_TrailPool)
	{
		if (!lstrcmp(iter.first, szName))
			return &iter.second;
	}

	return nullptr;
}

void CParticleMgr::Free()
{
	for (auto& iter : m_EffectPool)
	{
		_int iQueueSize = _int(iter.second.size());
		for (_int i = 0; i < iQueueSize; ++i)
		{
			Safe_Release(iter.second.front());
			iter.second.pop();
		}
	}
	m_EffectPool.clear();

	//for (auto& iter : m_TrailPool)
	//{
	//	_int iQueueSize = _int(iter.second.size());
	//	for (_int i = 0; i < iQueueSize; ++i)
	//	{
	//		Safe_Release(iter.second.front());
	//		iter.second.pop();
	//	}
	//}
	m_TrailPool.clear();

	//for (auto& iter : m_TrailList)
	//	Safe_Release(iter);

	for (auto& iter : m_TrailList_ForDelete)
		Safe_Release(iter);
	m_TrailList_ForDelete.clear();

	for (auto& iter : m_EffectList)
		Safe_Release(iter);
	m_EffectList.clear();

	for (auto& iter : m_vecParticle)
	{
		Safe_Delete(iter);
	}
	m_vecParticle.clear();

	//Safe_Release(m_pManagement);
}


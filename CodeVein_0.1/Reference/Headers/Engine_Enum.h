#pragma once

enum Skill_Index
{
	Skill_OneHand_Active_01, Skill_OneHand_Active_02, Skill_OneHand_Active_03, Skill_OneHand_Active_04,
	Skill_TwoHand_Active_01, Skill_TwoHand_Active_02, Skill_TwoHand_Active_03, Skill_TwoHand_Active_04,
	Skill_Halverd_Single, Skill_Gun_Single,
	Skill_Buff_Enchant_01, Skill_Buff_Enchant_02, Skill_Buff_Enchant_03,
	Skill_End
};

enum RAY_CALC 
{ 
	RAY_TO_WORLD, RAY_TO_VIEW, 
	RAY_TO_END 
};

typedef enum EXECUTION_WEAPON
{
	EXECUTION_Wpn_Orge, EXECUTION_Wpn_Stinger , EXECUTION_Wpn_Hounds ,EXECUTION_Wpn_Ivy,
	EXECUTION_Wpn_End

}EXE_WPN;

typedef enum EXECUTION_TYPE
{
	EXECUTION_Front, EXECUTION_FrontCinema,
	EXECUTION_Back, EXECUTION_BackCinema, 
	EXECUTION_ComboFront, EXECUTION_ComboBack, EXECUTION_ComboCinema,
	EXECUTION_Type_End

}EXE_TYPE;

enum RENDERID
{
	RENDER_PRIORITY, RENDER_FOG,
	RENDER_RENDER_ONLY,
	RENDER_NONALPHA, 
	RENDER_SHADOWTARGET, RENDER_MOTIONBLURTARGET,
	RENDER_ALPHA, RENDER_ALPHA_TRAIL, RENDER_ALPHA_UI,
	RENDER_EFFECT, RENDER_INSTANCE, RENDER_DISTORTION, RENDER_ORTHO,
	RENDER_UI, RENDER_3DUI, RENDER_UI_BACK,
	RENDER_END
};

enum SCENEID 
{ 
	SCENE_STATIC, SCENE_LOGO, SCENE_TITLE, 
	SCENE_STAGE, 
	SCENE_STAGE_BASE,
	SCENE_STAGE_TRAINING,
	SCENE_STAGE_01, SCENE_STAGE_02, SCENE_STAGE_03, SCENE_STAGE_04,
	SCENE_MORTAL, SCENE_PREPARE_ALL,
	SCENE_END 
};

enum AXIS_TYPE
{
	AXIS_X, AXIS_Y, AXIS_Z,

	AXIS_END
};


enum COL_TYPE
{
	COL_SPHERE, COL_AABB, COL_OBB, COL_CAPSULE, COL_RAY,
	COL_END
};

enum CELL_PARAM
{
	NORMAL, WATER, EVENT_TRIGGER, CINEMA_TRIGGER, NEXT_SUBSET_FALL, NEXT_SUBSET, UPPER_CELL, SPAWN_TRIGGER,
	CELL_END
};

enum CameraMode
{
	DEFAULT_MODE, RANGE_MODE, RANGE_AIM_MODE, AIM_MODE, EXECUTION_MODE, CINEMATIC_MODE, FLY_MODE,
	CAMMODE_END
};

enum CameraView
{
	FIRST_VIEW, THIRD_VIEW, BACK_VIEW, SHOLDER_VIEW, SIDE_VIEW, TOP_VIEW, QUATER_VIEW, TOOL_VIEW, CINEMA_VIEW,
	CAMVIEW_END
};

enum CameraClass
{
	STATIC_CAM, DYNAMIC_CAM, FREE_CAM,
	CAMCLASS_END
};

enum TEXTURETYPE
{
	TEX_DIFFUSE, TEX_NORMAL, TEX_CUBE, 
	TEX_END
};

enum MESHTYPE
{
	MESHTYPE_STATIC, MESHTYPE_DYNAMIC, MESHTYPE_NAVI,
	MESHTYPE_END
};

enum PHYSIC_COL
{
	PCOL_Head, PCOL_BodyUpper , PCOL_BodyLower
};

enum WEAPON_STATE
{
	WEAPON_None, WEAPON_SSword, WEAPON_LSword, WEAPON_Hammer, WEAPON_Halberd, WEAPON_Gun, WEAPON_Shield,
	WEAPON_End
};

enum WEAPON_DATA
{
	Wpn_SSword, Wpn_SSword_Black, Wpn_SSword_Military, Wpn_SSword_Slave,
	Wpn_LSword, Wpn_LSword_Black, Wpn_LSword_Military, Wpn_LSword_Slave,
	Wpn_Halberd, Wpn_Halberd_Black, Wpn_Halberd_Military, Wpn_Halberd_Slave,
	Wpn_Hammer, Wpn_Hammer_Black, Wpn_Hammer_Military, Wpn_Hammer_Slave,
	Wpn_Gun, Wpn_Gun_Black, Wpn_Gun_Military, Wpn_Gun_Slave,
	WPN_Shield_Normal, WPN_QueenShield, WPN_QueenLance, WPN_DeerKingShield, WPN_FrostBlood_IceGirl,
	WPN_DATA_End
};

enum WEAPON_ALL_DATA
{
	WpnAll_Gun_Bayonet,
	WpnAll_Gun_ReverTardor,
	WpnAll_Gun_Nakil,
	WpnAll_Gun_DarkBayonet,
	WpnAll_Halberd_SolidPiercer,
	WpnAll_Halberd_Black,
	WpnAll_Halberd_DarkBodish,
	WpnAll_Halberd_Nakil,
	WpnAll_Hammer_ImperseAnker,
	WpnAll_Hammer_Black,
	WpnAll_Hammer_DarkHeavyAxe,
	WpnAll_Hammer_Nakil,
	WpnAll_LSword_Tsubai,
	WpnAll_LSword_Black,
	WpnAll_LSword_Nakil,
	WpnAll_LSword_DarkTsubai,
	WpnAll_SSword_Red,
	WpnAll_SSword_Executioner,
	WpnAll_SSword_Nakil,
	WpnAll_SSword_DarkBrodeSword,

	WpnAll_END
};

enum ARMOR_All_DATA
{
	ArmorAll_DarkBlood,
	ArmorAll_DrakNight,
	ArmorAll_Spike,

	ArmorAll_END
};

// �Һ�, ��� ������ �̿�
enum ITEM_ALL_DATA
{
	ITEM_EXPEND_HPMAXUP,			// ����������� Ȯ������(��� ������ ������ ����) 
	ITEM_EXPEND_HP,					// ��� ������(hp ����)
	ITEM_EXPEND_RETURNER,			// ���ͳ�(��ȯ ������)
	ITEM_EXPEND_BLOOD,				// ���� �����(���� ȸ��)
	ITEM_EXPEND_CHEET,				// ġƮ ������(ü��, ���� ��� ����)
	ITEM_EXPEND_SUPERARMER,			// ���� �Ƹ�(���� ����)
	ITEM_MTRL_QUEENSTEEL,			// �� ��ƿ(��ȭ���)
	ITEM_MTRL_QUEENTITAN,			// �� Ƽź(��ȭ���)
	ITEM_MTRL_QUEENTUNGSTEN,		// �� �ֽ���(��ȭ���)
	ITEM_DATA_END
};

enum Teleport_ID
{
	TeleportID_Tutorial,
	TeleportID_Home_1,
	TeleportID_St01_1, TeleportID_St01_2, TeleportID_St01_3,
	TeleportID_St02_1, TeleportID_St02_2, TeleportID_St02_3,
	TeleportID_St03_1, TeleportID_St03_2,
	TeleportID_St04_1, TeleportID_St04_2,
	TeleportID_End
};

enum BloodCode_ID
{
	BloodCode_Artemis,
	BloodCode_Assassin,
	BloodCode_DarkKnight,
	BloodCode_Queen,
	BloodCode_Berserker,
	BloodCode_Hephaestus,
	BloodCode_Fighter,
	BloodCode_Heimdal,
	BloodCode_Hermes,
	BloodCode_Atlas,
	BloodCode_Prometheus,
	BloodCode_End
};

enum Skill_ID
{
	Dragon_Lunge, // ���溹ȣ
	Vanishing_Hollow, // ��Ͻ� �ҷο�
	Circulating_Pulse, // ���� ��ŧ��
	Triple_Annihilator, // ����
	Shadow_Assault, // ������ ���Ʈ
	Severing_Abyss, // ���ø����
	Swallow_Cutter, // ���� ����߸���
	Tormenting_Blast, // ���Ʈ ����Ʈ
	Phantom_Assault, // ���� ���Ʈ
	Legion_Punisher, // ���� �۴ϼ�
	Fusillade_Rondo, // �ѽ�ų
	Chariot_Rush, // â��ų
	SkillID_End
};

enum PET_TYPE
{
	Poison_Butterfly_B,
	Poison_Butterfly_A,
	Poison_Butterfly_S,
	Poison_Butterfly_SS,
	Deer_King_B,
	Deer_King_A,
	Deer_King_S,
	Deer_King_SS,
	PetType_End
};

enum ARMOR_TYPE
{
	ARMOR_Gauntlet,
	ARMOR_LongCoat,
	ARMOR_Muffler,
	ARMOR_End
};
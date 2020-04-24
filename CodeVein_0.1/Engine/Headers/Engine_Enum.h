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

enum RENDERID
{
	RENDER_PRIORITY, RENDER_NONALPHA, RENDER_SHADOWTARGET, RENDER_MOTIONBLURTARGET, RENDER_ALPHA, RENDER_EFFECT, RENDER_INSTANCE, RENDER_DISTORTION, RENDER_UI,
	RENDER_END
};

enum SCENEID 
{ 
	SCENE_STATIC, SCENE_LOGO, SCENE_TITLE, SCENE_STAGE, SCENE_STAGE_BASE, SCENE_STAGE_01, SCENE_STAGE_02, SCENE_STAGE_03, SCENE_STAGE_TRAINING,
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
	WEAPON_None, WEAPON_Ssword, WEAPON_LSword, WEAPON_Hammer, WEAPON_Halverd, WEAPON_Gun, WEAPON_Shield,
	WEAPON_End
};

enum Teleport_ID
{
	TeleportID_Home_1, TeleportID_Home_2,
	TeleportID_St01_1, TeleportID_St01_2, TeleportID_St01_3,
	TeleportID_St02_1, TeleportID_St02_2, TeleportID_St02_3,
	TeleportID_St03_1, TeleportID_St03_2,
	TeleportID_St04_1, TeleportID_St04_2,
};

enum BloodCode_ID
{
	BloodCode_Fighter, // 파이터
	BloodCode_Caster, // 캐스터
	BloodCode_Berserker, // 광전사(올리버)
	BloodCode_Prometheus, // 프로메테우스(루이)
	BloodCode_Eos, // 에오스(이오)
	BloodCode_End
};
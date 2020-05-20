#pragma once

#include "Monster.h"

BEGIN(Client)

class CThaiMan final : public CMonster
{
public:
	enum ATK_NORMAL_TYPE
	{
		NORMAL_RDIAGONAL, //n1
		NORMAL_L,
		NORMAL_BackDumpling,
		NORMAL_DropKick, //n4
		NORMAL_LkBk, //s1
		NORMAL_Capoeira,
		NORMAL_RDigonalLUpperRAccel, //sp1
		NORMAL_LkBkFk,
		NORMAL_RDigonalRRScrewRchop
	};

	enum THAIMAN_ANI 
	{
		Idle,
		NF_Sit,
		NF_Sit_End,
		NF_Scratch,
		NF_Scratch_End,
		NF_Crouch,
		NF_Crouch_End,

		Run,
		Walk_R,
		Walk_L,
		Walk_F,
		Walk_B,
		Dodge_R,
		Dodge_L,

		Dodge,
		Stun_Start,
		Stun_Loop,
		Stun_End,

		Down_S_Loop,
		Down_S_End,

		Down_P_Loop,
		Down_P_End,
		DmgStrike_S,
		DmgStrike_P,
		DmgBlow_F,
		DmgBlow_B,

		Dmg_S_FR,
		Dmg_S_FL,
		Dmg_S_BR,
		Dmg_S_BL,

		Dmg_N_FR,
		Dmg_N_FL,
		Dmg_N_BR,
		Dmg_N_BL,

		Dmg_W_FR,
		Dmg_W_FL,
		Dmg_W_BR,
		Dmg_W_BL,

		Death_F,
		Death_B,
		Death,

		Atk_StimPack,
		Atk_Add2,
		Atk_Add1,
		Atk_SP03,
		Atk_SP02,
		Atk_SP01,
		Atk_S02,
		Atk_S01,
		Atk_N04,
		Atk_N03,
		Atk_N02,
		Atk_N01
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Body,
		Bone_Head, 
		Bone_LeftHand, //0
		Bone_RightHand, //1
		Bone_LeftToeBase, //2
		Bone_RightToeBase, //3
		Bone_End 
	};

protected:
	explicit CThaiMan(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CThaiMan(const CThaiMan& rhs);
	virtual ~CThaiMan() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	void Update_Collider();
	void Render_Collider();

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);
	
	void Play_RandomAtkNormal();
	void Play_RDiagonal(); //n1 ¿ì´ë°¢
	void Play_L(); //n2 ÁÂÈ¾
	void Play_BackDumpling(); //n3 ¹é´ýºí¸µ
	void Play_DropKick(); //n4
	void Play_LkBk(); //s1 ¿·,µÚµ¹¾ÆÂ÷±â
	void Play_Capoeira(); //s2
	void Play_RDigonalLUpperRAccel(); //sp1 ¿ì´ë°¢,ÁÂÈ¾,¾Ç¼¿Á¡ÇÁatk
	void Play_RkBkFk(); //sp2 R¿·,LµÚ,R¾ÕÂ÷±â
	void Play_RDigonalRRScrewRchop(); //sp3 ¿ì´ë°¢,¿ìÈ¾,¿ì½Â·æ±Ç,¿ì³»¸®Ä¡±â

	void Play_Idle();
	void Play_Move();
	void Play_Hit(); 
	void Play_CC();
	void Play_Dead();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(CShader* pShader);
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Rigid();

public:
	static CThaiMan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*					m_matBone[Bone_End];
	THAIMAN_ANI				m_eState;

};

END


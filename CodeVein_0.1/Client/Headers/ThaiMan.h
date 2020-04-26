#pragma once

#include "Info_Monster.h"

BEGIN(Client)

class CThaiMan final : public CGameObject
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
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

private:
	void Update_Collider();
	void Render_Collider();
	void Enter_Collision();
	void Check_CollisionPush();
	void Check_CollisionEvent(list<CGameObject*> plistGameObject);

	void Function_FBLR();
	void Function_RotateBody();
	void Function_MoveAround(_float _fSpeed, _v3 _vDir = { V3_NULL });
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	
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
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CThaiMan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CMonsterUI*			m_pMonsterUI = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CTransform*			m_pTargetTransform = nullptr;

	_mat*					m_matBone[Bone_End];
	MONSTER_STATETYPE		m_eFirstCategory;
	MONSTER_IDLETYPE		m_eSecondCategory_IDLE;
	MONSTER_MOVETYPE		m_eSecondCategory_MOVE;
	MONSTER_ATKTYPE			m_eSecondCategory_ATK;
	MONSTER_HITTYPE			m_eSecondCategory_HIT;
	MONSTER_CCTYPE			m_eSecondCategory_CC;
	MONSTER_DEADTYPE		m_eSecondCategory_DEAD;

	WEAPON_STATE			m_eWeaponState;
	FBLR					m_eFBLR;
	THAIMAN_ANI				m_eState;

	_bool	m_bEventTrigger[20] = {};
	_bool	m_bCanPlayDead;
	_bool	m_bInRecognitionRange;
	_bool	m_bInAtkRange;
	_bool	m_bCanChase;
	_bool	m_bCanCoolDown;
	_bool	m_bIsCoolDown;
	_bool	m_bCanChooseAtkType;
	_bool	m_bIsCombo;
	_bool	m_bCanIdle;
	_bool	m_bIsIdle;
	_bool	m_bCanMoveAround;
	_bool	m_bIsMoveAround;

	_double	m_dTimeDelta;
	_double	m_dAniPlayMul;

	_float	m_fSkillMoveSpeed_Cur;
	_float	m_fSkillMoveSpeed_Max;
	_float	m_fSkillMoveAccel_Cur;
	_float	m_fSkillMoveAccel_Max;
	_float	m_fSkillMoveMultiply;

	_float	m_fRecognitionRange;
	_float	m_fShotRange;
	_float	m_fAtkRange;
	_float	m_fPersonalRange;
	_float	m_fCoolDownMax;
	_float	m_fCoolDownCur;

	_int	m_iRandom;
	_int	m_iDodgeCount;
	_int	m_iDodgeCountMax;

};

END


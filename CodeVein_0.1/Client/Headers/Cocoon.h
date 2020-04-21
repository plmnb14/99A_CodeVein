#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

#include "MonsterUI.h"

BEGIN(Client)

class CCocoon final : public CGameObject
{
public:
	enum FBLR { FRONT, FRONTLEFT, FRONTRIGHT, BACK, BACKLEFT, BACKRIGHT, LEFT, RIGHT };
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };

	enum COCOON_ANI 
	{
		Idle,
		Threat,
		Dead,
		Dmg_B,
		Dmg_F,
		Shot,
		Mist
	};

	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_Head, Bone_Neck, Bone_Jaw_Tongue, Bone_End };

public:
	struct INITSTRUCT
	{
		INITSTRUCT(
			_float _fDMG,
			_float _fHpMax,
			_float _fArmorMax,
			_float _fKnowRange,
			_float _fShotRange,
			_float _fAtkRange)
		{
			tMonterStatus.fDamage = _fDMG;
			tMonterStatus.fHp_Max = _fHpMax;
			tMonterStatus.fArmor_Max = _fArmorMax;

			fKonwingRange = _fKnowRange;
			fShotRange = _fShotRange;
			fCanAttackRange = _fAtkRange;
		}

		OBJECT_PARAM		tMonterStatus;
		_float				fKonwingRange = 20.f;
		_float				fShotRange = 10.f;
		_float				fCanAttackRange = 5.f;
	};
protected:
	explicit CCocoon(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCocoon(const CCocoon& rhs);
	virtual ~CCocoon() = default;

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

	void Function_RotateBody();
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Check_PosY();
	void Check_Hit();
	void Check_FBLR();
	void Check_Dist();
	void Check_AniEvent();

	void Play_Idle();
	void Play_Shot();
	void Play_Mist();
	void Play_Hit();
	void Play_Dead();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CCocoon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CTransform*			m_pTargetTransform = nullptr;
	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dTimeDelta;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory;
	FBLR				m_eFBLR;
	COCOON_ANI			m_eState;

	_bool				m_bEventTrigger[10] = {};
	_bool				m_bCanPlayDead = false;
	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;
	_bool				m_bCanChase = false;
	_bool				m_bCanRandomAtk = true;
	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;
	_bool				m_bCanIdle = true;
	_bool				m_bIsIdle = false;

	_float				m_fRecognitionRange = 25.f;
	_float				m_fShotRange = 15.f;
	_float				m_fAtkRange = 5.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0; //랜덤 받을 숫자

	// 몬스터 HP바 UI
	CMonsterUI*			m_pMonsterUI = nullptr;
};

END


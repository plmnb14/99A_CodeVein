#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
public:
	enum MONSTER_ANITYPE {IDLE, MOVE, ATTACK, HIT, DOWN, DEAD}; //down�� ���� �Ѿ��� ���� ���� �����ϱ�

	enum WOLF_ANI
	{
		Idle = 3, //�Ϲ� idle ���ֱ�
		Sit_End = 4,
		Sit = 5,
		Eat_End = 6,
		Eat = 7,

		Alert = 15, //��� idle
		Threat = 6, //�����ִ�

		Run = 0,
		Walk = 7,
		Dodge = 22,

		BiteLRL = 27,
		RDodgeAtk,
		LDodgeAtk,
		Frisbee,
		Dead_Strong=23,
		Dead,
		Dmg_F,
		Dmg_B,

		Down_Weak_Start = 16,
		Down_Weak_Loop,
		Down_weak_End,

		Down_Strong_Start = 19,
		Down_Strong_Loop,
		Down_Strong_End
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Body,
		Bone_Head,
		Bone_End
	};

	enum Cardinal_Point
	{
		East,
		West,
		South,
		North,
	};

protected:
	explicit CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBlackWolf(const CBlackWolf& rhs);
	virtual ~CBlackWolf() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Update_Collider();
	void Render_Collider();
	void Enter_CollisionEvent();
	void Check_CollisionEvent(list<CGameObject*> plistGameObject);

	void Check_Hit();
	void Check_Dist();
	void Set_AniEvent();

	void Skill_RotateBody();
	void Skill_CoolDown();
	void Skill_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);
	void Reset_BattleState();

	void Play_Idle();//�ϻ� �ִ�
	void Play_Eat();
	void Play_Sit();
	void Play_Walk(); //�̵� �ִ�
	void Play_Run();
	void Play_Dodge();
	void Play_RandomAtk(); //���� �ִ�
	void Play_Bite_LRL();
	void Play_RDodgeAtk();
	void Play_LDodgeAtk();
	void Play_Frisbee();
	void Play_Hit(); //�ǰ� �ִ�
	void Play_Down_Strong();
	void Play_Down_Weak();
	void Play_Dead(); //���� �ִ�
	void Play_Dead_Strong();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CTransform*			m_pTargetTransform = nullptr;

	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstIdentify; //��з�
	WOLF_ANI			m_eState; //�ִ� �з�
	_bool				m_bEventTrigger[10] = {}; //�̺�Ʈ ���� ����

	_bool				m_bInRecognitionRange = false; //���� ���� ����
	_bool				m_bInAttackRange = false; //���� ���� ����
	_bool				m_bCanChase = false; //�߰� ����
	_bool				m_bIsCoolDown = false; //��Ÿ�� ������ ����
	_bool				m_bIsDodge = false; //ȸ�� ������ ����

	_float				m_fRecognitionRange = 10.f; //���� ����
	_float				m_fAttackRange = 4.f; //���� ����

	_float				m_fCoolDown = 0.f; //��Ÿ��

	_int				m_iAttackRandomNumber = 0; //���ݾִ� ����ȭ
	_int				m_iIdleRandomNumber = 0;//�ϻ� �ִ� ����ȭ
	_int				m_iDodgeCount = 0; //nȸ �ǰݽ� �ٷ� ȸ��

	//_float			m_fHitDmgLimit = 50.f; //���� ������ �ۼ�Ʈ�� ���� ��� ���ϰ� ������, ��ġ
	//_int				m_iHitCountLimit = 5; //5�� ������ ���ϰ� ������, ��ġ
	//_int				m_iHitCount = 0;  //���������� �����ϱ�
	//Cardinal_Point	m_eTargetCardinal; //����

};

END
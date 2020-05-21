#pragma once

#include "Monster.h"

BEGIN(Client)
class CBossHP;
class CPoisonButterfly final : public CMonster
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_float _fFov, _float _fMaxLength, _float _fMinLength,
			_bool _bSpawn = false, _v3 vPos = V3_NULL, _v3 vAngle = V3_NULL, _ushort sStageIdx = 0)//, _ushort sSubsetIdx = 0, _ushort sCellIdx = 0)
			: fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
			, bSpawnOnTrigger(_bSpawn), vPos(vPos), vAngle(vAngle), sStageIdx(sStageIdx)// , sSubSetIdx(sSubSetIdx), sCellIdx(sCellIdx)
		{}

		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;

		//=======================================================
		// Ʈ���� ��ȯ��
		//=======================================================
		_bool					bSpawnOnTrigger = false;
		_v3						vPos = {};
		_v3						vAngle = {};
		_ushort					sStageIdx = 0;
		//=======================================================
	}INFO;

private:
	enum Ani {
		Ani_Idle = 0, Ani_Death = 14, /*Ani_Dmg01_B = 14, */Ani_Appearance = 2, Ani_Appearance_End = 3, Ani_Down_Start = 9,
		Ani_Down_Loop = 10, Ani_Down_End = 11
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPoisonButterfly(const CPoisonButterfly& rhs);
	virtual ~CPoisonButterfly() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:	//���ϵ�
	// �ٰŸ�
	// 1. ���� ��
	CBT_Composite_Node* Left_Eat(_float fWeight = 0.95f);		//�浹��	���� �Ÿ� 5
	// 2. ������ ��
	CBT_Composite_Node* Right_Eat(_float fWeight = 0.95f);	//�浹��	���� �Ÿ� 5
	// 3. ��-> �ڵ��Ƽ� ����
	CBT_Composite_Node* Eat_TurnEat();	//�浹�� ����Ʈ ���� �ٶ�, ��� ������,
	// 4. ���Ƽ� �� �ҿ뵹��
	CBT_Composite_Node* Poison_Tornado_After_Charging();	//�浹�� ���ݹ��� 3 �̻�
	// 5. ���� ���� �ѹ��� �� �߻�
	CBT_Composite_Node* OneTurn_Poison();	//�浹��	���� �Ÿ� 3
	// 6. �� ȸ��
	CBT_Composite_Node* Eat_Turn();	// �浹��
	// 7. Ʈ���ٹ̾�
	CBT_Composite_Node* WhirlWind();	// �浹��

	// ���Ÿ�
	// 1. ����, �̵��Ÿ� : 13
	CBT_Composite_Node* Rush();		//�浹��
	// 2. ����ź
	CBT_Composite_Node* Fire_5Bullet();		//��
	// 3. ���� ��
	CBT_Composite_Node* Fire_ChaseBullet();	// �߻�ü �߰� ��
	// 4. ���鼭 ������� �� ������
	CBT_Composite_Node* Turn_4PoisonShot(); // �浹��

	/////// ����
	CBT_Composite_Node* Smart_Eat();
	CBT_Composite_Node* Rush_And_ChaseBullet();

	//
	CBT_Composite_Node*		Start_Game();

	// ü�� 70�� �̻�	
	CBT_Composite_Node* More_Than_HP_70();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP70();
	CBT_Composite_Node* FarAttack_More_Than_HP70();

	// ü�� 70�� �̸�
	CBT_Composite_Node* More_Than_HP_40();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP40();
	CBT_Composite_Node* FarAttack_More_Than_HP40();

	// ü�� 40�� �̸�
	CBT_Composite_Node* HP_Final();
	CBT_Composite_Node* NearAttack_Dist5_Final();
	CBT_Composite_Node* FarAttack_Fianl();

	//////////////////// �ÿ�ȸ��

	CBT_Composite_Node*		Start_Show();

	// �þ߰����� ������ ����(ù ���Ϻ��� ���ʴ��), ������ ����
	CBT_Composite_Node*		Show_ChaseAndNearAttack();
	CBT_Composite_Node*		Show_TurnAndFarAttack();
	CBT_Composite_Node*		Show_NearAttack();	// 7��, ���� ������� �ٰŸ� ����	
	CBT_Composite_Node*		Show_FarAttack();	// 4��, ���� ������� ���Ÿ� ����

	//////////////////// 

private:
	void	Down();		// ü�� 70�� ���ϸ� �ٿ�

private:
	// �ǰݰ��� ���� �ð�
	_double				m_dHitTime = 0;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;
	_bool				m_bAppearanceEffect = false;

private:	// UI(����)
	CBossHP*			m_pBossUI = nullptr;
	_bool				m_bUITrigger = false;

private:	// �ٿ� ���¸� ���� ����
	_bool				m_bDown_Start = false;
	
	_bool				m_bDown_StartAni = false;
	_bool				m_bDown_LoopAni = false;
	_bool				m_bDown_EndAni = false;
	_double				m_dDownTime = 0;

	_byte				m_iDownCount = 0;

private:
	// �浹ü�� ���� ���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	// �����忡�� ���� Pos �����
	_v3					m_vTail2 = _v3(0.f, 0.f, 0.f);	//Tail2
	_v3					m_vTail4 = _v3(0.f, 0.f, 0.f);	//Tail4
	_v3					m_vTail6 = _v3(0.f, 0.f, 0.f);	//Tail6
	_v3					m_vTail6_Tongue2 = _v3(0.f, 0.f, 0.f);	//Tail6_Tongue2
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//Spine2
	_v3					m_vHead = _v3(0.f, 0.f, 0.f);	//Head

	// �� �ּ�
	D3DXFRAME_DERIVED*	m_pTail6Frame = nullptr;
	D3DXFRAME_DERIVED*	m_pTail4Frame = nullptr;
	D3DXFRAME_DERIVED*	m_pTail2Frame = nullptr;
	D3DXFRAME_DERIVED*	m_pHeadFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pTail6_Tongue2Frame = nullptr;

	_bool				m_bFinishCamShake = false;

private:	// ���ʻ��� ����
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// �÷��̾� ����

private:
	map<_int, const TCHAR*>	m_mapSound;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_NF();
	HRESULT Update_Collider();

private:
	void Check_PhyCollider();
	void Push_Collider();

private:
	void OnCollisionEnter();
	void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(CShader* pShader);

	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);
	HRESULT Ready_Sound();

public:
	static CPoisonButterfly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
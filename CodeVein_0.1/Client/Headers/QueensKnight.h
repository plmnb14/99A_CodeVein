#pragma once

#include "Monster.h"

BEGIN(Client)

class CWeapon;
class CBossHP;
class CQueensKnight final : public CMonster
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_float _fFov, _float _fMaxLength, _float _fMinLength,
			_bool _bSpawn = false, _v3 vPos = V3_NULL, _v3 vAngle = V3_NULL, _ushort eStageIdx = 0)//, _ushort sSubsetIdx = 0, _ushort sCellIdx = 0)
			: fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
			, bSpawnOnTrigger(_bSpawn), vPos(vPos), vAngle(vAngle), eStageIdx(eStageIdx)// , sSubSetIdx(sSubSetIdx), sCellIdx(sCellIdx)
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
		_ushort					eStageIdx = 0;
		//=======================================================
	}INFO;

private:
	enum Ani {
		Ani_Idle = 0, Ani_Death = 20, Ani_Appearance = 2, Ani_Appearance_End = 3, Ani_Down_Start = 13, Ani_Down_Loop = 14, Ani_Down_End = 15,
	
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CQueensKnight(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CQueensKnight(const CQueensKnight& rhs);
	virtual ~CQueensKnight() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:	// ���ϵ�
	// 1. Į ���η� �ֵθ���
	CBT_Composite_Node* Normal_HorizontalCut1();	// �浹��
	// 2. Į ���η� ����ġ��
	CBT_Composite_Node* Normal_VerticalCut1();		//�浹��
	// 3. Į �ι� �ֵθ���
	CBT_Composite_Node* TwoCombo_Cut();	//�浹��
	// 4. Į �ι� �ֵθ��� ����ġ��
	CBT_Composite_Node* ThreeCombo_Cut();	// �浹��
	// 5. �ڷ� �����鼭 ����
	CBT_Composite_Node* BackStep_Cut();	// �浹��
	// 6. ���
	CBT_Composite_Node* Sting();	// �浹��
	// 7. ȸ�� ����ġ��
	CBT_Composite_Node* Wing_Attack();	//�浹��
	// 8. ����
	CBT_Composite_Node* Rush();	//�浹��
	// 9. ����ġ��
	CBT_Composite_Node* Shield_Attack();	//�浹��

	// 10. ��ũ �ʵ�
	CBT_Composite_Node* LeakField();

	// ���� ����
	// 1. ����
	CBT_Composite_Node* Flash();

	// 1. ����ġ��
	CBT_Composite_Node* Flash_Wing_Attack();	//�浹��
	// 2. ����
	CBT_Composite_Node* Flash_Rush();	// �浹��
	// 3. ������ �������
	CBT_Composite_Node* Flash_Jump_Attack();	// �浹��
	// 4. �ѹ� ���� 
	CBT_Composite_Node* Flash_Cut();	// �浹��
	// 5. ���� �� �߾����� ����ġ��
	CBT_Composite_Node* Flash_Middle_Ground();	// �浹��


	///////////// ����
	CBT_Composite_Node* Smart_ThreeCombo_Cut();
	CBT_Composite_Node* Create_LeakField_Or_Not();


	//// ���� ����
	CBT_Composite_Node* Start_Game();

	// ü�� 70�� �̻�	// ���� ����
	CBT_Composite_Node* More_Than_HP_70();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP70();
	CBT_Composite_Node* FarAttack_More_Than_HP70();

	// ü�� 70�� �̸�	// ���� �߰�
	CBT_Composite_Node* More_Than_HP_40();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP40();
	CBT_Composite_Node* FarAttack_More_Than_HP40();

	// ü�� 40�� �̸�	// ���� ���� ��
	CBT_Composite_Node* HP_Final();
	CBT_Composite_Node* NearAttack_Dist5_Final();
	CBT_Composite_Node* FarAttack_Fianl();

	//////// �ÿ�ȸ��
	CBT_Composite_Node* Start_Show();
	CBT_Composite_Node* Show_ChaseAndNearAttack();
	CBT_Composite_Node* Show_NearAttack();	// 7��
	CBT_Composite_Node* Show_FarAttack();	// 5��, �Ϲݵ��� ����


private:
	void	Down();		// ü�� 70�� ���ϸ� �ٿ�

private:

	CWeapon*			m_pSword = nullptr;
	CWeapon*			m_pShield = nullptr;

	// �ǰݰ��� ���� �ð�
	_double				m_dHitTime = 0;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;

private:	// UI(����)
	CBossHP*			m_pBossUI = nullptr;
	_bool				m_bUITrigger = false;
	_float				m_fBGMFade = 1.f;

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
	_v3					m_vWing = _v3(0.f, 0.f, 0.f);
	_v3					m_vLeftHand = _v3(0.f, 0.f, 0.f);

	// �� �ּ�
	D3DXFRAME_DERIVED*	m_pWingFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pLeftHandFrame = nullptr;

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

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);
	HRESULT Ready_Sound();

public:
	static CQueensKnight* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
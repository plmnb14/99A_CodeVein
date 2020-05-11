#pragma once

#include "Monster.h"

BEGIN(Client)

class CWeapon;
class CBossHP;
class CIceGirl final : public CMonster
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
		Ani_Idle = 0, Ani_Death = 20, Ani_Appearance = 2, Ani_Appearance_End = 3, Ani_Down_Start = 12, Ani_Down_Loop = 13, Ani_Down_End = 14
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CIceGirl(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CIceGirl(const CIceGirl& rhs);
	virtual ~CIceGirl() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:	// ���ϵ�
	// 1. ȸ������
	CBT_Composite_Node* Turn_Cut(_float fWeight);
	// 2. ��ܺ���,  �� + �� + ��
	CBT_Composite_Node* ThreeCombo_Cut1();
	// 3. ��ܺ���,  �� + �� + ȸ������
	CBT_Composite_Node* ThreeCombo_Cut2();
	// 4. �Ϲ� ���� ����
	CBT_Composite_Node* Jump_Attack();
	// 5. �� �ֵθ��� ���� ũ�� �ݵ�� ��ȯ
	CBT_Composite_Node* Ice_Cut();
	// 6. ���� ��ȣ��
	CBT_Composite_Node* Ice_Barrier();
	// 7. �÷��̾� ������ ū ũ�� �ݵ��
	CBT_Composite_Node* ColdBeam_RandomPos();
	// 8. �ڽ� ���� 4����  �߰� ũ�� �ݵ��
	CBT_Composite_Node* ColdBeam_Around_Me();
	// 9. ���� ���� + �������� �ݵ��
	CBT_Composite_Node* Jump_Attack_And_ColdBeam();

	// ���Ÿ�
	// 1. ���� �� ���� ���
	CBT_Composite_Node* Charge_Rush();


	// ȸ��
	// ��
	CBT_Composite_Node* Dodge_Left();
	// ��
	CBT_Composite_Node* Dodge_Right();
	// ��
	CBT_Composite_Node* Dodge_Front();
	// ��
	CBT_Composite_Node* Dodge_Back();


	//////////////// ����
	CBT_Composite_Node* Chase_ThreeCombo_Cut1();	// �����ϸ鼭 ����
	CBT_Composite_Node* Chase_ThreeCombo_Cut2();	// �����ϸ鼭 ����
	CBT_Composite_Node* Create_IceBarrier_Or_Not();	// ������ȣ�� ������ ����
	CBT_Composite_Node* Cut_BackDodge();			// ���� �ڷ� ����
	CBT_Composite_Node* Dash_To_Target();			// Ÿ�ٿ��� �뽬
	

	CBT_Composite_Node* Chase_Timer(_double dRunTime, _float fSpeed);

	// ���� ��ġ ȸ��
	CBT_Composite_Node* Random_Dodge();

	///// ���� ����
	CBT_Composite_Node* Start_Game();

	// ü�� 70�� �̻�	// �ܼ� ����
	CBT_Composite_Node* More_Than_HP_70();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP70();
	CBT_Composite_Node* FarAttack_More_Than_HP70();

	// ü�� 70�� �̸�	// ���� ����, ȸ��, ���� ��ȣ�� �߰�
	CBT_Composite_Node* More_Than_HP_40();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP40();
	CBT_Composite_Node* FarAttack_More_Than_HP40();

	// ü�� 40�� �̸�	// ���� ���� ��
	CBT_Composite_Node* HP_Final();
	CBT_Composite_Node* NearAttack_Dist5_Final();
	CBT_Composite_Node* FarAttack_Fianl();


	//// �ÿ�ȸ��
	CBT_Composite_Node* Start_Show();
	CBT_Composite_Node* Show_RotationAndNearAttack();
	CBT_Composite_Node* Show_NearAttack();
	CBT_Composite_Node* Show_FarAttack();


private:
	void Down();

private:

	CWeapon*			m_pSword = nullptr;

	// �ǰݰ��� ���� �ð�
	_double				m_dHitTime = 0;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;

private:	// UI(����)
	CBossHP*			m_pBossUI = nullptr;

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
	_v3					m_vLeftHand = _v3(0.f, 0.f, 0.f);	//LeftHand
	_v3					m_vHips = _v3(0.f, 0.f, 0.f);		//Hips

	// �� �ּ�
	D3DXFRAME_DERIVED*	m_pLeftHandFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pHipsFrame = nullptr;


private:	// ���ʻ��� ����
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// �÷��̾� ����

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
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CIceGirl* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END
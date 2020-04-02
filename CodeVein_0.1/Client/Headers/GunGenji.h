#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CGunGenji final : public CGameObject
{
protected:
	explicit CGunGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGunGenji(const CGunGenji& rhs);
	virtual ~CGunGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//���ϵ�
	// ���Ÿ�
	// 1. �ѽ��
	CBT_Composite_Node* Shot();
	// 2. ���ڱ� �ѽ��
	CBT_Composite_Node* Sudden_Shot();

	// �ٰŸ�
	// 1. ���Ӹ��� ġ��, �̵��Ÿ� : 1.6
	CBT_Composite_Node* Upper_Slash();
	// 2. �Ȳ�ġ ġ��, �̵��Ÿ� : 1
	CBT_Composite_Node* Arm_Attack();
	// 3. ���, �̵��Ÿ� : 0.3
	CBT_Composite_Node* Sting_Attack();
	// 4. ���������� ����, �̵��Ÿ� : 0.5
	CBT_Composite_Node* Cut_To_Right();
	// 5. �Һ� �ѽ��, �̵��Ÿ� : -1.3
	CBT_Composite_Node* Tumbling_Shot();

	// ȸ��
	// 1. �ڷ� ȸ��
	CBT_Composite_Node* Dodge_B();


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();	// �Ĵٺ��� ������ ��������, �ָ� ���Ÿ�����

	
	CBT_Composite_Node*		LookPlayer_NearAttack();	// �÷��̾� �ٶ� �� ���� ���� ����
	CBT_Composite_Node*		LookPlayer_FarAttack();		// �÷��̾� �ٶ� �� ���� ���Ÿ� ����
	CBT_Composite_Node*		Chase();					// ����

	CBT_Composite_Node*		NearAttack();	// ���� ���� ����
	CBT_Composite_Node*		FarAttack();	// ���� ���Ÿ� ����


	//////////////////// �ÿ�ȸ��

	CBT_Composite_Node*	Start_Show();

	CBT_Composite_Node* Show_ChaseAttack();
	CBT_Composite_Node* Show_Attack();	// ���� ������� ����

	//////////////////// 


private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	CWeapon*			m_pGun = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Weapon();

public:
	static CGunGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CSwordShieldGenji final : public CGameObject
{
protected:
	explicit CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordShieldGenji(const CSwordShieldGenji& rhs);
	virtual ~CSwordShieldGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//���ϵ�
	// �ٰŸ�
	// 1. ���� ���۽�����, �̵��Ÿ� : 4
	CBT_Composite_Node* Upper_Slash();
	// 2. �� ������ ���, �̵��Ÿ� : 1
	CBT_Composite_Node* LongDelay_Sting();
	// 3. ����ġ��, �̵��Ÿ� : 0.5
	CBT_Composite_Node* Shield_Attack();
	// 4. �ѹ��� ���鼭 ����, �̵��Ÿ� : 2
	CBT_Composite_Node* Turning_Cut();
	// 5. ª�� ������ ���, �̵��Ÿ� : 0.6
	CBT_Composite_Node* ShortDelay_Sting();

	// ����
	// 1. ���е�� ���ϰ� ����
	CBT_Composite_Node* Hit_Weakly();
	// 2. ���е�� ���ϰ� ����
	CBT_Composite_Node* Hit_Strongly();

	// ����
	CBT_Composite_Node* Guard(_double dGuardTime);

	// ���е�� �ȱ�	�÷��̾�� �Ÿ� ��� �� �ȴ´�.



private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CSwordShieldGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
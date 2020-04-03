#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CSwordShieldGenji final : public CGameObject
{
public:
	enum Type { White, Jungle, Normal };

	typedef struct tagInitInfo
	{
		tagInitInfo(CSwordShieldGenji::Type _eType)
			: eType(_eType)
		{}

		CSwordShieldGenji::Type	eType = Normal;

	}INFO;

private:
	enum Ani {
		Ani_Idle = 42,
		Ani_Death = 64,
		Ani_Dmg01_FL = 122
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

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

	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		Chase_Guard_NearAttack();
	CBT_Composite_Node*		Chase();
	CBT_Composite_Node*		NearAttack();	// ���� ���� ����



private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;

	CWeapon*			m_pSpear = nullptr;
	CWeapon*			m_pShied = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	//���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = false;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_Collider();

private:
	void Check_Collider();

	HRESULT Draw_Collider();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();

public:
	static CSwordShieldGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
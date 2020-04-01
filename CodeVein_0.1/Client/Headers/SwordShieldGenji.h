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

private:	//패턴들
	// 근거리
	// 1. 돌진 어퍼슬래쉬, 이동거리 : 4
	CBT_Composite_Node* Upper_Slash();
	// 2. 긴 딜레이 찌르기, 이동거리 : 1
	CBT_Composite_Node* LongDelay_Sting();
	// 3. 방패치기, 이동거리 : 0.5
	CBT_Composite_Node* Shield_Attack();
	// 4. 한바퀴 돌면서 베기, 이동거리 : 2
	CBT_Composite_Node* Turning_Cut();
	// 5. 짧은 딜레이 찌르기, 이동거리 : 0.6
	CBT_Composite_Node* ShortDelay_Sting();

	// 맞음
	// 1. 방패들고 약하게 맞음
	CBT_Composite_Node* Hit_Weakly();
	// 2. 방패들고 강하게 맞음
	CBT_Composite_Node* Hit_Strongly();

	// 가드
	CBT_Composite_Node* Guard(_double dGuardTime);

	// 방패들고 걷기	플레이어와 거리 계산 후 걷는다.



private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
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
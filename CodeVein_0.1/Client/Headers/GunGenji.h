#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

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

private:	//패턴들
	// 원거리
	// 1. 총쏘기
	CBT_Composite_Node* Shot();
	// 2. 텀블링 총쏘기
	CBT_Composite_Node* Tumbling_Shot();
	// 3. 갑자기 총쏘기
	CBT_Composite_Node* Sudden_Shot();

	// 근거리
	// 1. 개머리판 치기, 이동거리 : 1.6
	CBT_Composite_Node* Upper_Slash();
	// 2. 팔꿈치 치기, 이동거리 : 1
	CBT_Composite_Node* Arm_Attack();
	// 3. 찌르기, 이동거리 : 0.3
	CBT_Composite_Node* Sting_Attack();
	// 4. 오른쪽으로 베기, 이동거리 : 0.5
	CBT_Composite_Node* Cut_To_Right();


	//////////////////// 시연회용

	CBT_Composite_Node*	Start_Show();

	CBT_Composite_Node* Show_ChaseAttack();
	CBT_Composite_Node* Show_Attack();	// 패턴 순서대로 공격

	//////////////////// 


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
	static CGunGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
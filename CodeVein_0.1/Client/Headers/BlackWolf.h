#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
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

public:
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	// 1. 프리스비 == 점프하면서 냠
	CBT_Composite_Node* Frisbee();
	// 2. 좌회피 후 물기
	CBT_Composite_Node* JumpDodgeL_And_Bite();
	// 3. 우회피 후 물기
	CBT_Composite_Node* JumpDodgeR_And_Bite();
	// 4. 좌 우 좌
	CBT_Composite_Node* Bite_LeftRightLeft();

	//Near
	//시야각 이내인 경우 == 4개중 1개 공격
	//시야각 밖인 경우 == 회전

	//Far
	//시야각 이내인 경우 == 추적
	//시야각 밖인 경우 == 일상행동

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;
	_v3					m_vHeadTeeth = _v3(0.f, 0.f, 0.f);	//머리
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//척추


};

END
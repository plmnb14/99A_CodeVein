#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CPoisonButterfly final : public CGameObject
{
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

private:	//패턴들
	// 근거리
	// 1. 왼쪽 냠
	CBT_Composite_Node* Left_Eat();
	// 2. 오른쪽 냠
	CBT_Composite_Node* Right_Eat();
	// 3. 냠-> 빙그르르냠
	CBT_Composite_Node* Eat_Turn_Eat();
	// 4. 기모아서 독 소용돌이
	CBT_Composite_Node* Poison_Tornado_After_Chaing();
	// 5. 좁은 범위 한바퀴 독 발사
	CBT_Composite_Node* OneTurn_Poison();

	// 원거리
	// 1. 돌진
	CBT_Composite_Node* Rush();
	// 2. 오발탄
	CBT_Composite_Node* Fire_5Bullet();
	// 3. 저격 샷
	CBT_Composite_Node* Fire_ChaseBullet();
	// 4. 트린다미어
	CBT_Composite_Node* WhirlWind();

	//응용
	// 시야각내에 있으면 공격, 없으면 추적
	CBT_Sequence*		AttackOrChase();


private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPoisonButterfly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
#pragma once

#include "Component.h"
#include "../Headers/BehaviorTree.h"
#include "../Headers/BlackBoard.h"

BEGIN(Engine)

class ENGINE_DLL CAIController final : public CComponent
{
private:
	explicit CAIController(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CAIController(const CAIController& rhs);
	virtual ~CAIController() = default;


public:
	HRESULT Set_BeHaviorTree(CBehaviorTree* pBehaviorTree);
	HRESULT Set_BlackBoard(CBlackBoard* pBlackBoard);

public:
	const CBlackBoard* Get_BlackBoard() const;


public:
	HRESULT Update_AIController(_double TimeDelta);

public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);

private:
	CBehaviorTree*		m_pBehaviorTree = nullptr;
	CBlackBoard*		m_pBlackBoard = nullptr;

public:
	static CAIController* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
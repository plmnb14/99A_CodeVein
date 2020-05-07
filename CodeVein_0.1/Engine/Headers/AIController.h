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

	CBlackBoard::OUTPUT Set_Value_Of_BlackBoard(const _tchar* pName, _bool bValue);
	CBlackBoard::OUTPUT Set_Value_Of_BlackBoard(const _tchar* pName, _float fValue);
	CBlackBoard::OUTPUT Set_Value_Of_BlackBoard(const _tchar* pName, _int iValue);
	CBlackBoard::OUTPUT Set_Value_Of_BlackBoard(const _tchar* pName, _v3 vValue);
	CBlackBoard::OUTPUT Set_Value_Of_BlackBoard(const _tchar* pName, _mat vValue);

	const _bool Get_BoolValue(const _tchar* pName) const;
	const _float Get_FloatValue(const _tchar* pName) const;
	const _int Get_IntValue(const _tchar* pName) const;
	const _v3 Get_V3Value(const _tchar* pName) const;
	const _mat Get_MatValue(const _tchar* pName) const;
public:
	void Reset_BT();

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
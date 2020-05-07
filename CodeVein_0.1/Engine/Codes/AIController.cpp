#include "../Headers/AIController.h"


CAIController::CAIController(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CAIController::CAIController(const CAIController & rhs)
	: CComponent(rhs) , m_pBehaviorTree(nullptr), m_pBlackBoard(nullptr)
{
}

HRESULT CAIController::Update_AIController(_double TimeDelta)
{
	if(m_pBehaviorTree)
		m_pBehaviorTree->Update_BeHaviorTree(TimeDelta, m_pBlackBoard);

	return S_OK;
}

HRESULT CAIController::Set_BeHaviorTree(CBehaviorTree * pBehaviorTree)
{
	if (nullptr == pBehaviorTree)
		return E_FAIL;

	Safe_Release(m_pBehaviorTree);

	m_pBehaviorTree = pBehaviorTree;

	return S_OK;
}

HRESULT CAIController::Set_BlackBoard(CBlackBoard * pBlackBoard)
{
	if (nullptr == pBlackBoard)
		return E_FAIL;

	Safe_Release(m_pBlackBoard);

	m_pBlackBoard = pBlackBoard;

	return S_OK;
}

CBlackBoard::OUTPUT CAIController::Set_Value_Of_BlackBoard(const _tchar * pName, _bool bValue)
{
	if (nullptr == m_pBlackBoard)
		return CBlackBoard::NONE;

	return m_pBlackBoard->Set_Value(pName, bValue);
}

CBlackBoard::OUTPUT CAIController::Set_Value_Of_BlackBoard(const _tchar * pName, _float fValue)
{
	if (nullptr == m_pBlackBoard)
		return CBlackBoard::NONE;

	return m_pBlackBoard->Set_Value(pName, fValue);
}

CBlackBoard::OUTPUT CAIController::Set_Value_Of_BlackBoard(const _tchar * pName, _int iValue)
{
	if (nullptr == m_pBlackBoard)
		return CBlackBoard::NONE;

	return m_pBlackBoard->Set_Value(pName, iValue);
}

CBlackBoard::OUTPUT CAIController::Set_Value_Of_BlackBoard(const _tchar* pName, _v3 vValue)
{
	if (nullptr == m_pBlackBoard)
		return CBlackBoard::NONE;

	return m_pBlackBoard->Set_Value(pName, vValue);
}

CBlackBoard::OUTPUT CAIController::Set_Value_Of_BlackBoard(const _tchar * pName, _mat vValue)
{
	if (nullptr == m_pBlackBoard)
		return CBlackBoard::NONE;

	return m_pBlackBoard->Set_Value(pName, vValue);
}

const _bool CAIController::Get_BoolValue(const _tchar * pName) const
{
	return m_pBlackBoard->Get_BoolValue(pName);
}

const _float CAIController::Get_FloatValue(const _tchar * pName) const
{
	return m_pBlackBoard->Get_FloatValue(pName);
}

const _int CAIController::Get_IntValue(const _tchar * pName) const
{
	return m_pBlackBoard->Get_IntValue(pName);
}

const _v3 CAIController::Get_V3Value(const _tchar * pName) const
{
	return m_pBlackBoard->Get_V3Value(pName);
}

const _mat CAIController::Get_MatValue(const _tchar * pName) const
{
	return m_pBlackBoard->Get_MatValue(pName);
}

void CAIController::Reset_BT()
{
	if (nullptr == m_pBehaviorTree)
		return;

	m_pBehaviorTree->Reset_BT(m_pBlackBoard);
}

HRESULT CAIController::Ready_Component_Prototype()
{
	return S_OK;;
}

HRESULT CAIController::Ready_Component(void * pArg)
{
	return S_OK;
}

CAIController * CAIController::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAIController* pInstance = new CAIController(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("Failed To Creating CAIController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CAIController::Clone_Component(void * pArg)
{
	CAIController* pInstance = new CAIController(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("Failed To Cloned CAIController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAIController::Free()
{
	Safe_Release(m_pBehaviorTree);

	Safe_Release(m_pBlackBoard);

	CComponent::Free();
}

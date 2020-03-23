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

const CBlackBoard * CAIController::Get_BlackBoard() const
{
	return m_pBlackBoard;
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

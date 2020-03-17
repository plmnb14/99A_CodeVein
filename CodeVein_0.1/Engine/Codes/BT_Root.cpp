#include "..\Headers\BT_Root.h"

CBT_Root::CBT_Root()
{
}

HRESULT CBT_Root::Set_Child(CBT_Composite_Node * pComposite_Node)
{
	if (nullptr == pComposite_Node)
		return E_FAIL;

	//m_pChildNode = static_cast<CBT_Node*>(pComposite_Node);
	m_pChildNode = (CBT_Node*)pComposite_Node;

	return S_OK;
}

HRESULT CBT_Root::Set_Child(CBT_Task_Node * pTask_Node)
{
	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Root::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack)
{
	if(m_pChildNode)
		return m_pChildNode->Update_Node(TimeDelta, pNodeStack);

	return BT_NODE_STATE::FAILED;
}

CBT_Root * CBT_Root::Create()
{
	return new CBT_Root;
}

void CBT_Root::Free()
{
	if(m_pChildNode)
		m_pChildNode->Free();
	
	Safe_Release(m_pChildNode);
	
	CBT_Node::Free();
}

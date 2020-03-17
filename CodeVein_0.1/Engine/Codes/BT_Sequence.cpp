#include "..\Headers\BT_Sequence.h"


CBT_Sequence::CBT_Sequence()
{
}

CBT_Sequence::CBT_Sequence(const CBT_Sequence & rhs)
{
}

HRESULT CBT_Sequence::Add_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChildren.emplace_back(pNode);
	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Sequence::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack)
{
	/*
	왼쪽부터 자식들을 검사해서 성공한 자식을 수행한다.
	*/
	Start_Node(pNodeStack);
	
	//for (CBT_Node* pChild : m_pChildren)
	//for ( ; m_pCurIndex < m_pChildren.size(); ++m_pCurIndex)
	if(m_pCurIndex < m_pChildren.size())
	{
		switch (m_pChildren[m_pCurIndex++]->Update_Node(TimeDelta, pNodeStack))
		{
			case BT_NODE_STATE::ABORTED:
			case BT_NODE_STATE::FAILED:
				End_Node(pNodeStack);
				return BT_NODE_STATE::FAILED;

			case BT_NODE_STATE::INPROGRESS:
				return BT_NODE_STATE::INPROGRESS;

			case BT_NODE_STATE::SUCCEEDED:
			case BT_NODE_STATE::SERVICE:
				break;
		}
	}

	End_Node(pNodeStack);
	return BT_NODE_STATE::SUCCEEDED;
}

HRESULT CBT_Sequence::Ready_Clone_Node()
{
	return S_OK;
}

void CBT_Sequence::Start_Node(vector<CBT_Node*>* pNodeStack)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);

		m_pCurIndex = 0;
		m_bInit = false;
	}
}

void CBT_Sequence::End_Node(vector<CBT_Node*>* pNodeStack)
{
	pNodeStack->pop_back();
	m_pCurIndex = 0;
	m_bInit = true;
}

CBT_Sequence * CBT_Sequence::Create_Prototype()
{
	return new CBT_Sequence();
}

CBT_Node * CBT_Sequence::Clone(void* pInit_Struct)
{
	CBT_Sequence* pInstance = new CBT_Sequence(*this);

	if (FAILED(pInstance->Ready_Clone_Node()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBT_Sequence::Free()
{
	for (CBT_Node* pChild : m_pChildren)
	{
		pChild->Free();
		Safe_Release(pChild);
	}
	m_pChildren.clear();
}

#include "..\Headers\BT_Selector.h"


CBT_Selector::CBT_Selector()
{
}

CBT_Selector::CBT_Selector(const CBT_Selector & rhs)
{
}

HRESULT CBT_Selector::Add_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChildren.emplace_back(pNode);
	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Selector::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack)
{
	/*
	왼쪽부터 자식들을 검사해서 성공한 자식을 수행한다.
	*/
	Start_Node(pNodeStack);

	//for (CBT_Node* pChild : m_pChildren)
	if (m_pCurIndex < m_pChildren.size())
	{
		switch (m_pChildren[m_pCurIndex++]->Update_Node(TimeDelta, pNodeStack))
		{
			case BT_NODE_STATE::ABORTED:
			case BT_NODE_STATE::FAILED:
			case BT_NODE_STATE::SERVICE:
				break;

			case BT_NODE_STATE::INPROGRESS:

			case BT_NODE_STATE::SUCCEEDED:
				End_Node(pNodeStack);
				return BT_NODE_STATE::SUCCEEDED;
		}
	}

	End_Node(pNodeStack);
	return BT_NODE_STATE::FAILED;
}

void CBT_Selector::Start_Node(vector<CBT_Node*>* pNodeStack)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);

		m_bInit = false;
	}
}

void CBT_Selector::End_Node(vector<CBT_Node*>* pNodeStack)
{
	pNodeStack->pop_back();
	m_bInit = true;
}

HRESULT CBT_Selector::Ready_Clone_Node(void* pInit_Struct)
{
	return S_OK;
}

CBT_Selector * CBT_Selector::Create_Prototype()
{
	return new CBT_Selector();
}

CBT_Node * CBT_Selector::Clone(void* pInit_Struct)
{
	CBT_Selector* pInstance = new CBT_Selector(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Selector");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Selector::Free()
{
	for (CBT_Node* pChild : m_pChildren)
	{
		pChild->Free();

		int a = 0;

		Safe_Release(pChild);
	}
	m_pChildren.clear();
}

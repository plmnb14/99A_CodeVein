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

CBT_Node::BT_NODE_STATE CBT_Selector::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	/*
	왼쪽부터 자식들을 검사해서 성공한 자식을 수행한다.
	*/
	Start_Node(pNodeStack, bDebugging);

	//for (CBT_Node* pChild : m_pChildren)
	if (m_pCurIndex < m_pChildren.size())
	{
		switch(m_eChild_State)
		{
			case BT_NODE_STATE::FAILED:
			case BT_NODE_STATE::SERVICE:
				++m_pCurIndex;
				this->m_eChild_State = BT_NODE_STATE::INPROGRESS;
				return BT_NODE_STATE::INPROGRESS;

			case BT_NODE_STATE::INPROGRESS:
				return m_pChildren[m_pCurIndex]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, bDebugging);

			case BT_NODE_STATE::SUCCEEDED:			
				return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		}
	}
	
	return End_Node(pNodeStack, BT_NODE_STATE::FAILED, bDebugging);
}

void CBT_Selector::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Selector } " << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_bInit = false;

	}
}

CBT_Node::BT_NODE_STATE CBT_Selector::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	m_bInit = true;

	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Selector } " << endl;

	}

	return eState;
}

HRESULT CBT_Selector::Ready_Clone_Node(void* pInit_Struct)
{
	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
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

		Safe_Release(pChild);
	}
	m_pChildren.clear();
}

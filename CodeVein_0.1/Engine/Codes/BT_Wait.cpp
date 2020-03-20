#include "../Headers/BT_Wait.h"

CBT_Wait::CBT_Wait()
{
}

CBT_Wait::CBT_Wait(const CBT_Wait & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_Wait::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	Start_Node(pNodeStack, bDebugging);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
	{		
		return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Wait::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = 0;

		m_bInit = false;

		if (bDebugging)
		{
			cout << "[" << m_iNodeNumber << "]" <<  "time start" << endl;
		}
	}

}

CBT_Node::BT_NODE_STATE CBT_Wait::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if(!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	m_bInit = true;

	if (bDebugging)
	{
		cout << "[" << m_iNodeNumber << "]" << "time end : " << m_dCurTime << endl;
	}
	m_dCurTime = 0;

	return eState;
}

HRESULT CBT_Wait::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_dMaxTime = temp.Target_dMaxTime;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}


CBT_Wait * CBT_Wait::Create_Prototype()
{
	return new CBT_Wait();
}

CBT_Node * CBT_Wait::Clone(void* pInit_Struct)
{
	CBT_Wait* pInstance = new CBT_Wait(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Wait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Wait::Free()
{
}

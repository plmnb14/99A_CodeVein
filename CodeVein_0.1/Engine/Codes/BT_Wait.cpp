#include "../Headers/BT_Wait.h"

CBT_Wait::CBT_Wait()
{
}

CBT_Wait::CBT_Wait(const CBT_Wait & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_Wait::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack)
{
	Start_Node(pNodeStack);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
	{
		End_Node(pNodeStack);
		return BT_NODE_STATE::SUCCEEDED;
	}

	return BT_NODE_STATE::INPROGRESS;
}

HRESULT CBT_Wait::Initialize_Node(_double dMaxTime)
{
	m_dMaxTime = dMaxTime;

	return S_OK;
}

void CBT_Wait::Start_Node(vector<CBT_Node*>* pNodeStack)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);


		m_bInit = false;
	}

}

void CBT_Wait::End_Node(vector<CBT_Node*>* pNodeStack)
{
	pNodeStack->pop_back();
	m_bInit = true;

	m_dCurTime = 0;
}

HRESULT CBT_Wait::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_dMaxTime = temp.m_dMaxTime;

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

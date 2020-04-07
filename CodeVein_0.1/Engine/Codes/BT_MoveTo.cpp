#include "../Headers/BT_MoveTo.h"
#include "../Headers/Transform.h"

CBT_MoveTo::CBT_MoveTo()
{
}

CBT_MoveTo::CBT_MoveTo(const CBT_MoveTo & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_MoveTo::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack,CBlackBoard* pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMovingTime)
	{
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);
	}
	m_pTransform->Add_Pos(_float(m_dMoveSpeed * TimeDelta));

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_MoveTo::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Move : " << "Speed " << m_dMoveSpeed << ", Time " << m_dMovingTime << " }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_MoveTo::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Move : " << "Speed " << m_dMoveSpeed << ", Time " << m_dMovingTime << " }" << endl;
	}

	return eState;
}

HRESULT CBT_MoveTo::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);

	m_pTransform = temp.Target_pTargetTransform;
	Safe_AddRef(m_pTransform);

	m_dMoveSpeed = temp.Target_dMoveSpeed;
	m_dMovingTime = temp.Target_dMovingTime;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_MoveTo * CBT_MoveTo::Create_Prototype()
{
	return new CBT_MoveTo();
}

CBT_Node * CBT_MoveTo::Clone(void* pInit_Struct)
{
	CBT_MoveTo* pInstance = new CBT_MoveTo(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_MoveTo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_MoveTo::Free()
{
	Safe_Release(m_pTransform);
}

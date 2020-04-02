#include "BT_UpdatePos.h"
#include "Transform.h"
CBT_UpdatePos::CBT_UpdatePos()
{
}

CBT_UpdatePos::CBT_UpdatePos(const CBT_UpdatePos & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_UpdatePos::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	if (nullptr == m_pTarget_Transform)
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, false);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
	{
		switch (m_eMode)
		{
			// 积己 冉荐 力茄
		case CBT_Service_Node::Finite:
			if (m_iCur_Count_Of_Execution > m_iMax_Count_Of_Execution)
				break;
			else
			{
				pBlackBoard->Set_Value(m_pTargetKey, m_pTarget_Transform->Get_Pos());
				++m_iCur_Count_Of_Execution;
			}
			break;

			// 积己 冉荐 公力茄
		case CBT_Service_Node::Infinite:
			pBlackBoard->Set_Value(m_pTargetKey, m_pTarget_Transform->Get_Pos());
			End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, false);
			break;
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_UpdatePos::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : Transform }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dUpdateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_UpdatePos::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : Transform }" << endl;
	}

	m_bInit = true;

	return eState;
}

HRESULT CBT_UpdatePos::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_pTargetKey, temp.Target_Key);
	m_dUpdateTime = temp.Target_dUpdateTime;
	m_dOffset = temp.Target_dOffset;
	m_pTarget_Transform = temp.Target_Transform;
	Safe_AddRef(m_pTarget_Transform);
	m_iMax_Count_Of_Execution = temp.Count_Of_Execution;
	m_eMode = temp.eMode;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_UpdatePos * CBT_UpdatePos::Create_Prototype()
{
	return new CBT_UpdatePos();
}

CBT_UpdatePos * CBT_UpdatePos::Clone(void * pInit_Struct)
{
	CBT_UpdatePos* pInstance = new CBT_UpdatePos(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_UpdatePos");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_UpdatePos::Free()
{
	Safe_Release(m_pTarget_Transform);
}

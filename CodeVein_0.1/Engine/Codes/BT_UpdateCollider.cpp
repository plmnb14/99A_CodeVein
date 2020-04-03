#include "BT_UpdateCollider.h"
#include "Collider.h"

CBT_UpdateCollider::CBT_UpdateCollider()
{
}

CBT_UpdateCollider::CBT_UpdateCollider(const CBT_UpdateCollider & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_UpdateCollider::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, false);

	m_dCurTime += TimeDelta;

	if (false == m_bService_Start)
	{
		if (m_dService_StartTime < m_dCurTime)
			m_bService_Start = true;
	}
	else
	{
		if (m_dCurTime > m_dMaxTime)
		{
			switch (m_eMode)
			{
				// 생성 횟수 제한
			case CBT_Service_Node::Finite:
				if (m_iCur_Count_Of_Execution >= m_iMax_Count_Of_Execution)
				{
					m_pTarget_ObjParam->bCanAttack = false;
					//cout << "HIt End " << m_dCurTime << endl;
					break;
				}
				else
				{
					//cout << "Hit On" << m_dCurTime << endl;

					m_pTarget_ObjParam->bCanAttack = true;

					++m_iCur_Count_Of_Execution;
					m_dCurTime = 0;
				}
				break;

				// 생성 횟수 무제한,  콜라이더는 필요없음
			//case CBT_Service_Node::Infinite:
			//	m_pTarget_ObjParam->bCanAttack = true;
			//	End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, false);
			//	break;
			}
		}
	}
	return BT_NODE_STATE::INPROGRESS;
}

void CBT_UpdateCollider::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : AttackOn }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dUpdateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_UpdateCollider::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : AttackOn }" << endl;
	}

	m_pTarget_ObjParam->bCanAttack = false;

	m_bService_Start = false;
	m_bInit = true;

	return eState;
}

HRESULT CBT_UpdateCollider::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	m_dUpdateTime = temp.Target_dUpdateTime;
	m_dOffset = temp.Target_dOffset;
	m_pTarget_ObjParam = temp.Target_ObjParam;
	m_iMax_Count_Of_Execution = temp.Count_Of_Execution;
	m_eMode = temp.eMode;
	m_dService_StartTime = temp.Service_Start_Time;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_UpdateCollider * CBT_UpdateCollider::Create_Prototype()
{
	return new CBT_UpdateCollider();
}

CBT_UpdateCollider * CBT_UpdateCollider::Clone(void * pInit_Struct)
{
	CBT_UpdateCollider* pInstance = new CBT_UpdateCollider(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_UpdateCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_UpdateCollider::Free()
{
}

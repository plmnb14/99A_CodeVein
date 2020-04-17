#include "BT_UpdateParam.h"
#include "Collider.h"

CBT_UpdateParam::CBT_UpdateParam()
{
}

CBT_UpdateParam::CBT_UpdateParam(const CBT_UpdateParam & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_UpdateParam::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, false);

	if(true == m_bService_End)
		return BT_NODE_STATE::INPROGRESS;

	m_dCurTime += TimeDelta;

	if (false == m_bService_Start)
	{
		if (m_dService_StartTime < m_dCurTime)
		{
			m_bService_Start = true;
			m_dCurTime = 100000;		// 서비스 시작시 처음 실행을 위해 큰값을 줌.
		}
	}
	else
	{
		if (m_dCurTime > m_dMaxTime)
		{
			switch (m_eMode)
			{
				// 생성 횟수 제한
			case CBT_Service_Node::Finite:
				// 루틴 끝날때 마지막 행동
				if (m_iCur_Count_Of_Execution >= m_iMax_Count_Of_Execution)
				{
					switch (m_eParam)
					{
					case CBT_UpdateParam::Collider:
						m_pTarget_ObjParam->bCanAttack = false;
						break;

					}

					m_bService_End = true;
				}
				else
				{
					switch (m_eParam)
					{
					case Engine::CBT_UpdateParam::Collider:
						m_pTarget_ObjParam->bCanAttack = true;
						//cout << "HIt On" << endl;
						break;

					case CBT_UpdateParam::Dodge:
						m_pTarget_ObjParam->bIsDodge = true;
						break;
					}

					++m_iCur_Count_Of_Execution;
					m_dCurTime = 0;
				}
				break;

				// 생성 횟수 무제한,  콜라이더는 필요없음
			case CBT_Service_Node::Infinite:
				break;
			}
		}
	}
	return BT_NODE_STATE::INPROGRESS;
}

void CBT_UpdateParam::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
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
		m_bService_End = false;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_UpdateParam::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
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

HRESULT CBT_UpdateParam::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	m_dUpdateTime = temp.Target_dUpdateTime;
	m_dOffset = temp.Target_dOffset;
	m_pTarget_ObjParam = temp.Target_ObjParam;
	m_eParam = temp.eParam;
	m_iMax_Count_Of_Execution = temp.Count_Of_Execution;
	m_eMode = temp.eMode;
	m_dService_StartTime = temp.Service_Start_Time;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_UpdateParam * CBT_UpdateParam::Create_Prototype()
{
	return new CBT_UpdateParam();
}

CBT_UpdateParam * CBT_UpdateParam::Clone(void * pInit_Struct)
{
	CBT_UpdateParam* pInstance = new CBT_UpdateParam(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_UpdateParam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_UpdateParam::Free()
{
}

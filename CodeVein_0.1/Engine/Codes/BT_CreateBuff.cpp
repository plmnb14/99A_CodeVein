#include "BT_CreateBuff.h"
#include "Object_Manager.h"
#include "..\Headers\Transform.h"
#include "..\Headers\AIController.h"

CBT_CreateBuff::CBT_CreateBuff()
{
}

CBT_CreateBuff::CBT_CreateBuff(const CBT_CreateBuff & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_CreateBuff::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, false);

	m_dCurTime += TimeDelta;

	if (false == m_bService_Start)
	{
		if (m_dService_StartTime < m_dCurTime)
		{
			m_bService_Start = true;
			m_dCurTime = 100000;
		}
	}
	else
	{
		if (m_dCurTime > m_dMaxTime)
		{
			switch (m_eMode)
			{
				// 积己 冉荐 力茄
			case CBT_Service_Node::Finite:
				if (m_iCur_Count_Of_Execution >= m_iMax_Count_Of_Execution)
					break;
				else
				{
					CObject_Manager::Get_Instance()->Add_GameObject_ToLayer(m_pObject_Tag, SCENE_STAGE, L"Layer_MonsterProjectile", &BUFF_INFO(m_pTransform, m_pAIcontroller, m_dLifeTime));
					++m_iCur_Count_Of_Execution;
					m_dCurTime = 0;
				}
				break;

				// 积己 冉荐 公力茄
			case CBT_Service_Node::Infinite:
				CObject_Manager::Get_Instance()->Add_GameObject_ToLayer(m_pObject_Tag, SCENE_STAGE, L"Layer_MonsterProjectile", &BUFF_INFO(m_pTransform, m_pAIcontroller, m_dLifeTime));

				End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, false);
				break;
			}
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_CreateBuff::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : CreateBuff }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dCreateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_CreateBuff::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : CreateBuff Count " << m_iCur_Count_Of_Execution << " }" << endl;
	}

	m_bService_Start = false;
	m_bInit = true;

	return eState;
}

HRESULT CBT_CreateBuff::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_pObject_Tag, temp.Object_Tag);

	m_pTransform = temp.pTransform;
	//Safe_AddRef(m_pTransform);

	m_pAIcontroller = temp.pAIController;
	//Safe_AddRef(m_pAIcontroller);

	m_dLifeTime = temp.dLifeTime;

	m_dCreateTime = temp.Target_dCreateTime;
	m_dOffset = temp.Target_dOffset;
	m_dService_StartTime = temp.Service_Start_Time;
	m_iMax_Count_Of_Execution = temp.MaxCount_Of_Execution;
	m_eMode = temp.eMode;
	

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_CreateBuff * CBT_CreateBuff::Create_Prototype()
{
	return new CBT_CreateBuff();
}

CBT_CreateBuff * CBT_CreateBuff::Clone(void * pInit_Struct)
{
	CBT_CreateBuff* pInstance = new CBT_CreateBuff(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_CreateBuff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_CreateBuff::Free()
{
	//Safe_Release(m_pTransform);
	//Safe_Release(m_pAIcontroller);
}

#include "BT_CreateBullet.h"
#include "ParticleMgr.h"
#include "Object_Manager.h"

CBT_CreateBullet::CBT_CreateBullet()
{
}

CBT_CreateBullet::CBT_CreateBullet(const CBT_CreateBullet & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_CreateBullet::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
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
				// 积己 冉荐 力茄
			case CBT_Service_Node::Finite:
				if (m_iCur_Count_Of_Execution >= m_iMax_Count_Of_Execution)
					break;
				else
				{
					m_vCreate_Pos = pBlackBoard->Get_V3Value(m_pCreate_Pos_Key);
					m_vDir = pBlackBoard->Get_V3Value(m_pDir_Key);

					CObject_Manager::Get_Instance()->Add_GameObject_ToLayer(m_pObject_Tag, SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(m_vCreate_Pos, m_vDir, m_fSpeed, m_dLifeTime));
					++m_iCur_Count_Of_Execution;
				}
				break;

				// 积己 冉荐 公力茄
			case CBT_Service_Node::Infinite:
				m_vCreate_Pos = pBlackBoard->Get_V3Value(m_pCreate_Pos_Key);
				m_vDir = pBlackBoard->Get_V3Value(m_pDir_Key);

				CObject_Manager::Get_Instance()->Add_GameObject_ToLayer(m_pObject_Tag, SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(m_vCreate_Pos, m_vDir, m_fSpeed, m_dLifeTime));

				End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, false);
				break;
			}
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_CreateBullet::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : CreateBullet }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dCreateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_CreateBullet::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : CreateBullet Count " << m_iCur_Count_Of_Execution << " }" << endl;
	}

	m_bService_Start = false;
	m_bInit = true;

	return eState;
}

HRESULT CBT_CreateBullet::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_pObject_Tag, temp.Object_Tag);

	lstrcpy(m_pCreate_Pos_Key, temp.Create_Pos_Key);
	lstrcpy(m_pDir_Key, temp.Dir_Key);
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_dCreateTime = temp.Target_dCreateTime;
	m_dOffset = temp.Target_dOffset;
	m_dService_StartTime = temp.Service_Start_Time;
	m_iMax_Count_Of_Execution = temp.MaxCount_Of_Execution;
	m_eMode = temp.eMode;
	

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_CreateBullet * CBT_CreateBullet::Create_Prototype()
{
	return new CBT_CreateBullet();
}

CBT_CreateBullet * CBT_CreateBullet::Clone(void * pInit_Struct)
{
	CBT_CreateBullet* pInstance = new CBT_CreateBullet(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_CreateBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_CreateBullet::Free()
{
}

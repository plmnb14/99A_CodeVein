#include "BT_CreateEffect.h"
#include "ParticleMgr.h"

CBT_CreateEffect::CBT_CreateEffect()
{
}

CBT_CreateEffect::CBT_CreateEffect(const CBT_CreateEffect & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_CreateEffect::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
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
					break;
				else
				{
					++m_iCur_Count_Of_Execution;

					if (Version::Old == m_eVersion)
					{
						switch (m_eEffectMode)
						{
						case CBT_CreateEffect::One:
							CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag, pBlackBoard->Get_V3Value(m_vEffect_Pos_Key));
							break;
						case CBT_CreateEffect::Particle:
							CParticleMgr::Get_Instance()->Create_ParticleEffect(m_pEffect_Tag, m_fEffect_lifeTime, pBlackBoard->Get_V3Value(m_vEffect_Pos_Key));
							break;
						}
					}
					else if (Version::New == m_eVersion)
					{
						CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag, pBlackBoard->Get_V3Value(m_vEffect_Pos_Key), m_pEffect_TransformCom, pBlackBoard->Get_V3Value(m_vEffect_Dir_Key), pBlackBoard->Get_V3Value(m_vEffect_Angle_Key));
					}
				}
				break;
				// 생성 횟수 무제한
			case CBT_Service_Node::Infinite:
				switch (m_eEffectMode)
				{
				case CBT_CreateEffect::One:
					CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag, pBlackBoard->Get_V3Value(m_vEffect_Pos_Key));
					break;
				case CBT_CreateEffect::Particle:
					CParticleMgr::Get_Instance()->Create_ParticleEffect(m_pEffect_Tag, m_fEffect_lifeTime, pBlackBoard->Get_V3Value(m_vEffect_Pos_Key));
					break;
				}

				End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, false);
				break;


			}
		}

	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_CreateEffect::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : Effect }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dCreateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_CreateEffect::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : Effect }" << endl;
	}

	m_bService_Start = false;
	m_bInit = true;

	return eState;
}

HRESULT CBT_CreateEffect::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;
	INFO2 temp2 = *(INFO2*)pInit_Struct;

	m_eVersion = temp.eVersion;

	// 버젼 확인
	if (Version::Old == m_eVersion)
	{
		strcpy_s<STR_128>(m_pNodeName, temp.Target_NodeName);
		lstrcpy(m_pEffect_Tag, temp.Effect_Tag);
		lstrcpy(m_vEffect_Pos_Key, temp.Create_Pos_Key);

		switch (m_eEffectMode)
		{
		case CBT_CreateEffect::One:
			break;
		case CBT_CreateEffect::Particle:
			m_fEffect_lifeTime = temp.Effect_Life_Time;
			break;
		}

		m_dCreateTime = temp.Target_dCreateTime;
		m_dOffset = temp.Target_dOffset;
		m_iMax_Count_Of_Execution = temp.MaxCount_Of_Execution;
		m_eMode = temp.eMode;
		m_dService_StartTime = temp.Service_Start_Time;
	}
	else if (Version::New == m_eVersion)
	{
		strcpy_s<STR_128>(m_pNodeName, temp2.Target_NodeName);
		lstrcpy(m_pEffect_Tag, temp2.Effect_Tag);
		lstrcpy(m_vEffect_Pos_Key, temp2.Create_Pos_Key);
		m_pEffect_TransformCom = temp2.pTransform;
		lstrcpy(m_vEffect_Dir_Key, temp2.Dir_Key);
		lstrcpy(m_vEffect_Angle_Key, temp2.Angle_Key);

		switch (m_eEffectMode)
		{
		case CBT_CreateEffect::One:
			break;
		case CBT_CreateEffect::Particle:
			m_fEffect_lifeTime = temp2.Effect_Life_Time;
			break;
		}

		m_dCreateTime = temp2.Target_dCreateTime;
		m_dOffset = temp2.Target_dOffset;
		m_iMax_Count_Of_Execution = temp2.MaxCount_Of_Execution;
		m_eMode = temp2.eMode;
		m_dService_StartTime = temp2.Service_Start_Time;

	}
	else
		return E_FAIL;


	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_CreateEffect * CBT_CreateEffect::Create_Prototype()
{
	return new CBT_CreateEffect();
}

CBT_CreateEffect * CBT_CreateEffect::Clone(void * pInit_Struct)
{
	CBT_CreateEffect* pInstance = new CBT_CreateEffect(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_CreateEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_CreateEffect::Free()
{
	//Safe_Release(m_pEffect_TransformCom);
}

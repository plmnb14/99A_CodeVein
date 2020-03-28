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
	switch (m_eMode)
	{
	case CBT_Service_Node::Finite:
		if (m_iCur_Count_Of_Execution > m_iMax_Count_Of_Execution)
			return BT_NODE_STATE::FAILED;
		break;

	case CBT_Service_Node::Infinite:
		break;
	}

	Start_Node(pNodeStack, plistSubNodeStack, false);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
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

		End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, false);
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
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : Transform }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dCreateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_CreateEffect::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : Transform }" << endl;
	}

	switch (m_eMode)
	{
	case CBT_Service_Node::Finite:
		++m_iCur_Count_Of_Execution;
		break;

	case CBT_Service_Node::Infinite:
		break;
	}

	m_bInit = true;

	return eState;
}

HRESULT CBT_CreateEffect::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
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
}

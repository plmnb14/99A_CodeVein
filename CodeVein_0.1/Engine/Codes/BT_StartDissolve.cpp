#include "BT_StartDissolve.h"
#include "GameObject.h"

CBT_StartDissolve::CBT_StartDissolve()
{
}

CBT_StartDissolve::CBT_StartDissolve(const CBT_StartDissolve & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_StartDissolve::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, false);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
	{
		m_pThisObject->Start_Dissolve(m_fFxSpeed, m_bFadeIn, false);
		m_dMaxTime = 1000000;
	}


	return BT_NODE_STATE::INPROGRESS;
}

void CBT_StartDissolve::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : Dissolve , FadeIn : " << m_bFadeIn << " }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dService_StartTime;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_StartDissolve::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : Dissolve , FadeIn : " << m_bFadeIn << " }" << endl;
	}

	m_bInit = true;

	return eState;
}

HRESULT CBT_StartDissolve::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);

	m_pThisObject = temp.Target_Object;
	//Safe_AddRef(m_pThisObject);

	m_eMode = temp.eMode;
	m_dService_StartTime = temp.Service_Start_Time;
	m_fFxSpeed = temp.fFxSpeed;
	m_bFadeIn = temp._bFadeIn;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_StartDissolve * CBT_StartDissolve::Create_Prototype()
{
	return new CBT_StartDissolve();
}

CBT_StartDissolve * CBT_StartDissolve::Clone(void * pInit_Struct)
{
	CBT_StartDissolve* pInstance = new CBT_StartDissolve(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_StartDissolve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_StartDissolve::Free()
{
	//Safe_Release(m_pThisObject);
}

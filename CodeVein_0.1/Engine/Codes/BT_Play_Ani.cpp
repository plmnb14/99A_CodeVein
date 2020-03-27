#include "../Headers/BT_Play_Ani.h"
#include "../Headers/Mesh_Dynamic.h"

CBT_Play_Ani::CBT_Play_Ani()
{
}

CBT_Play_Ani::CBT_Play_Ani(const CBT_Play_Ani & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_Play_Ani::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	if (m_pMesh_Dynamic->Is_Finish_Animation(m_fAniWeight))
	{
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Play_Ani::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Play_Ani : " << m_iAni_Index << " }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_pMesh_Dynamic->SetUp_Animation(m_iAni_Index);

		m_bInit = false;
	}

}

CBT_Node::BT_NODE_STATE CBT_Play_Ani::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Play_Ani : " << m_iAni_Index << " }" << endl;
	}

	return eState;
}

HRESULT CBT_Play_Ani::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);

	m_pMesh_Dynamic = temp.Target_pMesh_Dynamic;
	Safe_AddRef(m_pMesh_Dynamic);

	m_iAni_Index = temp.Target_iAni_iIndex;
	m_fAniWeight = temp.fAniWeight;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}


CBT_Play_Ani * CBT_Play_Ani::Create_Prototype()
{
	return new CBT_Play_Ani();
}

CBT_Node * CBT_Play_Ani::Clone(void* pInit_Struct)
{
	CBT_Play_Ani* pInstance = new CBT_Play_Ani(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Play_Ani");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Play_Ani::Free()
{
	Safe_Release(m_pMesh_Dynamic);
}

#include "../Headers/BT_MoveTo.h"
#include "../Headers/Transform.h"
#include "..\Headers\NavMesh.h"
#include "../Headers/RigidBody.h"

CBT_MoveTo::CBT_MoveTo()
{
}

CBT_MoveTo::CBT_MoveTo(const CBT_MoveTo & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_MoveTo::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack,CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (m_bInit)
		Cal_MoveSpeed(pBlackBoard);

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMovingTime)
	{
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);
	}
	//m_pTransform->Add_Pos(m_fMoveSpeed);
	m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &m_vMoveDir, m_fMoveSpeed * _float(TimeDelta))));

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_MoveTo::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Move : }" << endl;
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { MoveTo : " << m_fMoveSpeed << " }" << endl;
	}

	return eState;
}

HRESULT CBT_MoveTo::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<STR_128>(m_pNodeName, temp.Target_NodeName);

	m_pTransform = temp.pTargetTransform;
	Safe_AddRef(m_pTransform);

	m_pNavMesh = temp.pNavMesh;
	Safe_AddRef(m_pNavMesh);

	m_pRigid = temp.pRigid;
	Safe_AddRef(m_pRigid);

	lstrcpy(m_pPosKey, temp.Pos_Key);

	m_dMovingTime = temp.Target_dMovingTime;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

HRESULT CBT_MoveTo::Cal_MoveSpeed(CBlackBoard * pBB)
{
	_v3 Target_Pos = pBB->Get_V3Value(m_pPosKey);

	m_fMoveSpeed = D3DXVec3Length(&(Target_Pos - m_pTransform->Get_Pos())) / _float(m_dMovingTime);
	m_vMoveDir = *D3DXVec3Normalize(&_v3(), &(Target_Pos - m_pTransform->Get_Pos()));

	//cout << m_fMoveSpeed << endl;
	//cout << Target_Pos.x << ", " << Target_Pos.z << endl;

	_v3 temp = (Target_Pos - m_pTransform->Get_Pos());
	_float fTemp = D3DXVec3Length(&temp);
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
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pRigid);
}

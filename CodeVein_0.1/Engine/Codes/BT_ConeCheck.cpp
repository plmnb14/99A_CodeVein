#include "..\Headers\BT_ConeCheck.h"
#include "..\Headers\Transform.h"

CBT_ConeCheck::CBT_ConeCheck()
{
}

CBT_ConeCheck::CBT_ConeCheck(const CBT_ConeCheck & rhs)
{
}

HRESULT CBT_ConeCheck::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_ConeCheck::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	if (nullptr == m_pTransform)
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	//최초 평가
	if (!m_bInProgress)
	{
		_v3 vTarget_Pos = pBlackBoard->Get_V3Value(m_Target_Key);

		_v3 vLength = vTarget_Pos - m_pTransform->Get_Pos();
		vLength.y = 0.f;

		/*
		높이와 상관없이 거리를 판단함.
		타겟이 허용거리 안에 있다.  ->  시야각 판단 시작
		*/
		//	허용거리		타겟
		if (m_fMaxLength >= D3DXVec3Length(&vLength))
		{
			if (Is_InFov(m_fDegreeOfFov, m_pTransform, vTarget_Pos))
			{
				m_bInProgress = true;
				return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);
			}
			else
			{
				return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);
			}
		}
		else
		{
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);
		}
	}
	// 진행중 평가
	else
	{
		switch (m_eChild_State)
		{
		case BT_NODE_STATE::SERVICE:
		case BT_NODE_STATE::FAILED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);

		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;

		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_ConeCheck::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { ConeCheck : Degree " << m_fDegreeOfFov << "  Length " << m_fMaxLength << " }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_eChild_State = BT_NODE_STATE::INPROGRESS;
		m_bInProgress = false;
		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_ConeCheck::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if(!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	m_bInit = true;

	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { ConeCheck : Degree " << m_fDegreeOfFov << "  Length " << m_fMaxLength << " }" << endl;
	}

	return eState;
}

HRESULT CBT_ConeCheck::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	
	lstrcpy(m_Target_Key, temp.Target_Key);
	m_pTransform = temp.pTransform;
	Safe_AddRef(m_pTransform);
	m_fDegreeOfFov = temp.fDegreeOfFov;
	m_fMaxLength = temp.fMaxLength;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

_bool CBT_ConeCheck::Is_InFov(_float fDegreeOfFov, CTransform * pThisTransform, _v3 vTargetPos)
{
	_v3 vThisLook = *(_v3*)(&pThisTransform->Get_WorldMat().m[2]);
	vThisLook.y = 0.f;
	D3DXVec3Normalize(&vThisLook, &vThisLook);

	_v3 FromThisToTarget = vTargetPos - pThisTransform->Get_Pos();
	FromThisToTarget.y = 0.f;
	D3DXVec3Normalize(&FromThisToTarget, &FromThisToTarget);


	_float fDot_Temp = D3DXVec3Dot(&vThisLook, &FromThisToTarget);
	_float fRadian = acosf(fDot_Temp);

	if (D3DXToDegree(fRadian) < fDegreeOfFov * 0.5f)
		return true;

	return false;
}

CBT_ConeCheck * CBT_ConeCheck::Create_Prototype()
{
	return new CBT_ConeCheck();
}

CBT_Node * CBT_ConeCheck::Clone(void * pInit_Struct)
{
	CBT_ConeCheck* pInstance = new CBT_ConeCheck(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_ConeCheck");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_ConeCheck::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pChild);
}

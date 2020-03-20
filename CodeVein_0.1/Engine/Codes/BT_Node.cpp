#include "..\Headers\BT_Node.h"

_uint CBT_Node::m_iNode_Count = 0;

CBT_Node::CBT_Node()
{
}

HRESULT CBT_Node::_Set_Auto_Number(_uint* iNum_Variable)
{
	*iNum_Variable = m_iNode_Count++;

	return S_OK;
}

HRESULT CBT_Node::_Init_NodeNumber()
{
	m_iNode_Count = 0;

	return S_OK;
}

void CBT_Node::Notify_Parent_Of_State(CBT_Node* pParent, BT_NODE_STATE eState)
{
	pParent->m_eChild_State = eState;
	this->m_eChild_State = BT_NODE_STATE::INPROGRESS;
}

void CBT_Node::Free()
{

}

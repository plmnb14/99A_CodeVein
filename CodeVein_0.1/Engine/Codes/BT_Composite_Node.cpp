#include "..\Headers\\BT_Composite_Node.h"

CBT_Composite_Node::CBT_Composite_Node()
{
}

vector<CBT_Node*> CBT_Composite_Node::Get_Children() const
{
	return m_pChildren;
}

void CBT_Composite_Node::Add_Child(CBT_Node * pNode)
{
	m_pChildren.push_back(pNode);
}

void CBT_Composite_Node::Free()
{
	CBT_Node::Free();
}

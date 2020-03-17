#pragma once

#include "BT_Node.h"

/*
�б��� ��Ʈ�� ����, �б������ �⺻��Ģ�̴�.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Composite_Node abstract : public CBT_Node
{
protected:
	explicit CBT_Composite_Node();
	virtual ~CBT_Composite_Node() = default;

public:
	vector<CBT_Node*> Get_Children() const;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) = 0;

	void Add_Child(CBT_Node* pNode);

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack) = 0;
	virtual void End_Node(vector<CBT_Node*>* pNodeStack) = 0;

private:
	vector<CBT_Node*>	m_pChildren;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END
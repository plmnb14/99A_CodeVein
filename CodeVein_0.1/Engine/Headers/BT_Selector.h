#pragma once

#include "BT_Composite_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Selector final : public CBT_Composite_Node
{
protected:
	explicit CBT_Selector();
	explicit CBT_Selector(const CBT_Selector& rhs);
	virtual ~CBT_Selector() = default;

public:
	HRESULT Add_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) override;

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack);
	virtual void End_Node(vector<CBT_Node*>* pNodeStack);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	vector<CBT_Node*>	m_pChildren;

	size_t				m_pCurIndex = 0;

public:
	static CBT_Selector* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
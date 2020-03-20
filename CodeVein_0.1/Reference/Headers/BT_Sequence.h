#pragma once

#include "BT_Composite_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Sequence final : public CBT_Composite_Node
{
protected:
	explicit CBT_Sequence();
	explicit CBT_Sequence(const CBT_Sequence& rhs);
	virtual ~CBT_Sequence() = default;

public:
	HRESULT Add_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) override;

	HRESULT Ready_Clone_Node();

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack);
	virtual void End_Node(vector<CBT_Node*>* pNodeStack);

private:
	vector<CBT_Node*>	m_pChildren;
	
	size_t				m_pCurIndex = 0;

public:
	static CBT_Sequence* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
#pragma once

#include "..\headers\BT_Composite_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Sequence final : public CBT_Composite_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char	Target_NodeName[256] = { 0, };
	}INFO;

protected:
	explicit CBT_Sequence();
	explicit CBT_Sequence(const CBT_Sequence& rhs);
	virtual ~CBT_Sequence() = default;

public:
	HRESULT Add_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	size_t				m_pCurIndex = 0;

public:
	static CBT_Sequence* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
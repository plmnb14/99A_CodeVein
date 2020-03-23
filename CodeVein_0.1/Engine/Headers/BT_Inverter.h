#pragma once

#include "..\headers\BT_Decorator_Node.h"

/*
자식노드 결과의 반대값을 반환합니다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Inverter final : public CBT_Decorator_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char	Target_NodeName[256];
	}INFO;

protected:
	explicit CBT_Inverter();
	explicit CBT_Inverter(const CBT_Inverter& rhs);
	virtual ~CBT_Inverter() = default;

public:
	HRESULT Set_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, const CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CBT_Node*	m_pChild = nullptr;
	_bool		m_bInProgress = false;

public:
	static CBT_Inverter* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
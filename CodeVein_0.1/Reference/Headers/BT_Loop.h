#pragma once

#include "..\headers\BT_Decorator_Node.h"

/*
정해진 횟수만큼 자식노드를 반복합니다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Loop final : public CBT_Decorator_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _uint iLoopCount)
			: iMaxLoopCount(iLoopCount)
		{ strcpy_s<STR_128>(Target_NodeName, pNodeName); }

		char	Target_NodeName[STR_128] = { 0, };
		_uint	iMaxLoopCount = 0;
	}INFO;

protected:
	explicit CBT_Loop();
	explicit CBT_Loop(const CBT_Loop& rhs);
	virtual ~CBT_Loop() = default;

public:
	HRESULT Set_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CBT_Node*	m_pChild = nullptr;
	_bool		m_bInProgress = false;

	_uint		m_iCurLoopCount = 0;
	_uint		m_iMaxLoopCount = 0;

public:
	static CBT_Loop* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
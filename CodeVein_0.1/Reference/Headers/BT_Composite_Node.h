#pragma once

#include "..\headers\BT_Node.h"

/*
분기의 루트를 정의, 분기실행의 기본규칙이다.
*/

BEGIN(Engine)
class CBT_Service_Node;
class ENGINE_DLL CBT_Composite_Node abstract : public CBT_Node
{
protected:
	explicit CBT_Composite_Node();
	virtual ~CBT_Composite_Node() = default;

public:
	HRESULT Add_Service(CBT_Service_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) = 0;
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging) = 0;

protected:
	HRESULT Release_ServiceNode(list<vector<CBT_Node*>*>* m_plistNodeStack, list<vector<CBT_Node*>*>* plistServiceNode, _bool bDebugging);

protected:
	vector<CBT_Node*>	m_pChildren;

	// 서비스노드 스택을 담는 리스트
	list<vector<CBT_Node*>*>	m_listServiceNodeStack;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END
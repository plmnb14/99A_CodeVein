#pragma once

#include "Base.h"
#include "..\headers\BehaviorTree.h"
/*
Composite
Decorator
Service
Task

위 노드들의 최상위 클래스입니다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Node abstract : public CBase
{
public:
	enum class BT_NODE_STATE { FAILED, SUCCEEDED, INPROGRESS, SERVICE };

protected:
	explicit CBT_Node();
	virtual ~CBT_Node() = default;

public:
	// timedelta, &본인 멤버변수 스택, debug,  쓰레드여부 필요없음
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

public:
	/*
	Task 노드가 시작될때 Start_Node를 호출
	끝날때 End_Node를 호출
	*/
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) = 0;
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging) = 0;

	// 엔진에서만 사용
public:
	// 각 노드에 번호를 붙이기 위함.
	static HRESULT _Set_Auto_Number(_uint* iNum_Variable);
	static HRESULT _Init_NodeNumber();

protected:
	void Notify_Parent_Of_State(CBT_Node* pParent, BT_NODE_STATE eState);

	// 엔진에서만 사용
protected:
	void Cout_Indentation(vector<CBT_Node*>* pNodeStack);

protected:
	_uint			m_iNodeNumber = 0;
	BT_NODE_STATE	m_eChild_State = BT_NODE_STATE::INPROGRESS;
	char			m_pNodeName[256] = { 0, };

	_bool	m_bInit = true;
	
private:
	static _uint m_iNode_Count;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END
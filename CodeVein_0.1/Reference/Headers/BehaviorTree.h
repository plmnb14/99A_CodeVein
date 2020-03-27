#pragma once

#include "Base.h"
#include "BlackBoard.h"
/*
Ai�� �۵���Ű�� �ൿƮ���� �������̽�
*/

BEGIN(Engine)

class CBT_Decorator_Node;
class CBT_Task_Node;
class CBT_Composite_Node;
class CBT_Root;
class CBT_Node;
class ENGINE_DLL CBehaviorTree final : public CBase
{
private:
	explicit CBehaviorTree();
	virtual ~CBehaviorTree() = default;

public:
	HRESULT Set_Child(CBT_Node* pNode);

public:
	virtual void Update_BeHaviorTree(_double TimeDelta, CBlackBoard* pBlackBoard);

private:
	HRESULT Ready_BehaviorTree(_bool bDebuging);

private:
	CBT_Root*					m_pRoot = nullptr;
	//���� ������
	vector<CBT_Node*>			m_pNodeStack;
	
	/* 
	0��   : mainThread
	1�� ~ : SubThread
	*/
	list<vector<CBT_Node*>*>	m_plistNodeStack;

	_bool						m_bDebuging = false;

public:
	static CBehaviorTree* Create(_bool bDebuging = false);

	virtual void Free();
};

END
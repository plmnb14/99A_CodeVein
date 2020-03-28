#pragma once

#include "Base.h"
#include "..\headers\BehaviorTree.h"
/*
Composite
Decorator
Service
Task

�� ������ �ֻ��� Ŭ�����Դϴ�.
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
	// timedelta, &���� ������� ����, debug,  �����忩�� �ʿ����
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

public:
	/*
	Task ��尡 ���۵ɶ� Start_Node�� ȣ��
	������ End_Node�� ȣ��
	*/
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) = 0;
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging) = 0;

	// ���������� ���
public:
	// �� ��忡 ��ȣ�� ���̱� ����.
	static HRESULT _Set_Auto_Number(_uint* iNum_Variable);
	static HRESULT _Init_NodeNumber();

protected:
	void Notify_Parent_Of_State(CBT_Node* pParent, BT_NODE_STATE eState);

	// ���������� ���
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
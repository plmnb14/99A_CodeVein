#pragma once

#include "..\Headers\BT_Decorator_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Cooldown final : public CBT_Decorator_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _double dCoolTime)
			: Target_dCoolTime(dCoolTime)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }
		
		char	Target_NodeName[256] = { 0, };
		_double	Target_dCoolTime = 0;
	}INFO;

protected:
	explicit CBT_Cooldown();
	explicit CBT_Cooldown(const CBT_Cooldown& rhs);
	virtual ~CBT_Cooldown() = default;

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
	CBT_Node*			m_pChild = nullptr;
	//쓰레드로 쿨타임 돌릴 공간
	vector<CBT_Node*>	m_pSubNodeStack;
	_bool				m_bInProgress = false;
	_bool				m_bAddThread = true;

	BT_NODE_STATE		m_eCurState = BT_NODE_STATE::SUCCEEDED;
	_double				m_dCurTime = 0;
	_double				m_dCoolTime = 0;

public:
	static CBT_Cooldown* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
#pragma once

#include "..\headers\BT_Composite_Node.h"

/*
m_pChildren
[0] : 메인 행동
[1] : 서브 행동

Mode
Immediate : 메인 행동이 끝나면 서브 행동도 끝냄
Delayed : 메인 행동이 끝나도 서브 행동이 끝날 때까지 대기
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Simple_Parallel final : public CBT_Composite_Node
{
public:
	enum class Mode { Immediate, Delayed };

	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, Mode eMode)
			: eNode_Mode(eMode) 
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char	Target_NodeName[256];
		Mode eNode_Mode;
	}INFO;


protected:
	explicit CBT_Simple_Parallel();
	explicit CBT_Simple_Parallel(const CBT_Simple_Parallel& rhs);
	virtual ~CBT_Simple_Parallel() = default;

public:
	HRESULT Set_Main_Child(CBT_Node* pNode);
	HRESULT Set_Sub_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, const CBlackBoard* pBlackBoard, _bool bDebugging);

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	HRESULT Delete_SubNodeStack(list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);

private:
	Mode				m_eMode = Mode::Immediate;

	vector<CBT_Node*>	m_pSubNodeStatck;

	_bool				m_bMain_InProgress = false;
	_bool				m_bSub_InProgress = false;

	BT_NODE_STATE		m_bMainState = BT_NODE_STATE::FAILED;
	BT_NODE_STATE		m_bSubState = BT_NODE_STATE::FAILED;

public:
	static CBT_Simple_Parallel* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
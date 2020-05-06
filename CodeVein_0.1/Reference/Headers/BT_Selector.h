#pragma once

#include "..\headers\BT_Composite_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Selector final : public CBT_Composite_Node
{
public:
	enum Mode { Normal, Random };

	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, Mode _eMode)
			: eMode(_eMode)
		{ strcpy_s<STR_128>(Target_NodeName, pNodeName); }

		char	Target_NodeName[STR_128] = { 0, };
		Mode	eMode = Normal;
	}INFO;

protected:
	explicit CBT_Selector();
	explicit CBT_Selector(const CBT_Selector& rhs);
	virtual ~CBT_Selector() = default;

public:
	HRESULT Add_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	size_t				m_pCurIndex = 0;
	Mode				m_eMode = Normal;

	_int				m_iRandomNum = 0;

public:
	static CBT_Selector* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
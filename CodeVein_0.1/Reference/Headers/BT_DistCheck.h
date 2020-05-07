#pragma once

#include "BT_Decorator_Node.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CBT_DistCheck final : public CBT_Decorator_Node
{
public:
	
	/*
	fMaxLength : 시야각의 허용 길이
	*/
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _pTarget_Key, CTransform* _pTransform, _float _fMaxLength)
			: pTransform(_pTransform), fMaxLength(_fMaxLength)
		{ strcpy_s<STR_128>(Target_NodeName, pNodeName);
		lstrcpy(Target_Key, _pTarget_Key); }

		char		Target_NodeName[STR_128] = { 0, };
		_tchar		Target_Key[STR_128] = { 0, };
		CTransform*	pTransform = nullptr;
		_float		fMaxLength = 0.f;
	} INFO;

protected:
	explicit CBT_DistCheck();
	explicit CBT_DistCheck(const CBT_DistCheck& rhs);
	virtual ~CBT_DistCheck() = default;

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
	CBT_Node*		m_pChild = nullptr;
	_bool			m_bInProgress = false;

	CTransform*		m_pTransform = nullptr;
	_tchar			m_Target_Key[STR_128] = { 0, };
	_float			m_fMaxLength = 0;

public:
	static CBT_DistCheck* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
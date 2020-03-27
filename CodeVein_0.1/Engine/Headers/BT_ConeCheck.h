#pragma once

#include "BT_Decorator_Node.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CBT_ConeCheck final : public CBT_Decorator_Node
{
public:
	
	/*
	fDegreeOfFov : 시야각
	fMaxLength : 시야각의 허용 길이
	*/
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _pTarget_Key, CTransform* _pTransform, _float _fDegreeOfFov, _float _fMaxLength)
			: pTransform(_pTransform), fDegreeOfFov(_fDegreeOfFov), fMaxLength(_fMaxLength)
		{ strcpy_s<256>(Target_NodeName, pNodeName);
		lstrcpy(Target_Key, _pTarget_Key); }

		char		Target_NodeName[256] = { 0, };
		_tchar		Target_Key[256] = { 0, };
		CTransform*	pTransform = nullptr;
		_float		fDegreeOfFov = 0.f;
		_float		fMaxLength = 0.f;
	} INFO;

protected:
	explicit CBT_ConeCheck();
	explicit CBT_ConeCheck(const CBT_ConeCheck& rhs);
	virtual ~CBT_ConeCheck() = default;

public:
	HRESULT Set_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	_bool Is_InFov(_float fDegreeOfFov, CTransform* pThisTransform, _v3 TargetPos);

private:
	CBT_Node*		m_pChild = nullptr;
	_bool			m_bInProgress = false;

	CTransform*		m_pTransform = nullptr;
	_tchar			m_Target_Key[256] = { 0, };
	_float			m_fDegreeOfFov = 0;
	_float			m_fMaxLength = 0;

public:
	static CBT_ConeCheck* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
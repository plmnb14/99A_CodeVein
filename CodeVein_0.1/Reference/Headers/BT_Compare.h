#pragma  once

#include "BT_Decorator_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Compare final : public CBT_Decorator_Node
{
public:
	
	enum Mode { EQUAL, NOT_EQUAL, GREATER_THAN, SMALLER_THAN };
	enum Value { BOOL, FLOAT, INT};

	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, Value _eValue,  _tchar* _TargetKey_A, Mode _eMode, _tchar* _TargetKey_B)
			: eValue(_eValue), eMode(_eMode)
		{ strcpy_s<STR_128>(Target_NodeName, pNodeName);
		lstrcpy(TargetKey_A, _TargetKey_A);
		lstrcpy(TargetKey_B, _TargetKey_B);}

		char			Target_NodeName[STR_128] = { 0, };
		_tchar*			TargetKey_A = nullptr;
		_tchar*			TargetKey_B = nullptr;
		Mode			eMode = EQUAL;
		Value			eValue = BOOL;

	} INFO;

protected:
	explicit CBT_Compare();
	explicit CBT_Compare(const CBT_Compare& rhs);
	virtual ~CBT_Compare() = default;

public:
	HRESULT Set_Child(CBT_Node* pNode);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging);

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CBT_Node*		m_pChild = nullptr;
	_bool			m_bInProgress = false;

	Value			m_eValue = BOOL;
	_tchar			m_pTargetKey_A[STR_128] = { 0, };
	_tchar			m_pTargetKey_B[STR_128] = { 0, };
	Mode			m_eMode = EQUAL;

	/*
	0 : Key A
	1 : Key B
	*/
	_bool			m_bTargetBool[2] = { false, };
	_float			m_fTargetValue[2] = { 0.f, };
	_int			m_iTargetValue[2] = { 0, };

public:
	static CBT_Compare* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
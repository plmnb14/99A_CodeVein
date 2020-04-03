#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)
/*
업데이트시마다 계속 사이각을 체크해서 돌린다.
*/
class CTransform;
class ENGINE_DLL CBT_ChaseDir final : public CBT_Task_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* _pNodeName, CTransform* pThis_Transform, _tchar* _pTarget_Key, _double _dRunTime, _double _dTimeOffset)
			: pTransform(pThis_Transform), dRunTime(_dRunTime), dTimeOffset(_dTimeOffset)
		{ strcpy_s<256>(Target_NodeName, _pNodeName); 
		lstrcpy(Target_Key, _pTarget_Key); }

		CTransform*	pTransform = nullptr;
		char		Target_NodeName[256] = { 0, };
		_tchar		Target_Key[256] = { 0, };
		_double		dRunTime = 0;
		_double		dTimeOffset = 0;
	} INFO;

protected:
	explicit CBT_ChaseDir();
	explicit CBT_ChaseDir(const CBT_ChaseDir& rhs);
	virtual ~CBT_ChaseDir() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	HRESULT Look_At_Target(_double TimeDelta, const _v3& vTarget_Pos);
	
private:
	CTransform*	m_pTransform = nullptr;
	_tchar		m_Target_Key[256] = { 0, };

	_double		m_dCurTime = 0;
	_double		m_dRunTime = 0;
	_double		m_dTimeOffset = 0;

	_double		m_dMaxTime = 0;

public:
	static CBT_ChaseDir* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
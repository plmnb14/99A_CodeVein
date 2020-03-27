#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBT_UpdateTransform final : public CBT_Service_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _Target_Key, CTransform* _pTarget_Transform, _double dWaitingTime, _double dOffset)
			: Target_Transform(_pTarget_Transform), Target_dWaitingTime(dWaitingTime), Target_dOffset(dOffset)
		{ strcpy_s<256>(Target_NodeName, pNodeName); 
		lstrcpy(Target_Key, _Target_Key);}

		char				Target_NodeName[256];
		_tchar				Target_Key[256];
		_double				Target_dWaitingTime;
		_double				Target_dOffset;
		CTransform*			Target_Transform;
	}INFO;

protected:
	explicit CBT_UpdateTransform();
	explicit CBT_UpdateTransform(const CBT_UpdateTransform& rhs);
	virtual ~CBT_UpdateTransform() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	// dMaxTime = m_dWaitingTime +- m_dOffset
	_double		m_dMaxTime = 0;

private:
	_double				m_dCurTime = 0;
	_double				m_dWaitingTime = 0;
	_double				m_dOffset = 0;

	_tchar				m_pTargetKey[256] = { 0, };

	CTransform*			m_pTarget_Transform = nullptr;

public:
	static CBT_UpdateTransform* Create_Prototype();
	virtual CBT_UpdateTransform* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
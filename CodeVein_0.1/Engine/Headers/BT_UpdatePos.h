#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBT_UpdatePos final : public CBT_Service_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _Target_Key, CTransform* _pTarget_Transform, _uint _Count_Of_Execution, _double dUpdateTime, _double dOffset, CBT_Service_Node::Mode _eMode)
			: Target_Transform(_pTarget_Transform), Target_dUpdateTime(dUpdateTime), Target_dOffset(dOffset), Count_Of_Execution(_Count_Of_Execution)
		{ strcpy_s<256>(Target_NodeName, pNodeName); 
		lstrcpy(Target_Key, _Target_Key);}

		char					Target_NodeName[256] = {0,};
		_tchar					Target_Key[256] = { 0, };
		_double					Target_dUpdateTime = 0;
		_double					Target_dOffset = 0;
		CTransform*				Target_Transform = nullptr;
		_uint					Count_Of_Execution = 0;
		CBT_Service_Node::Mode	eMode = CBT_Service_Node::Infinite;
	}INFO;

protected:
	explicit CBT_UpdatePos();
	explicit CBT_UpdatePos(const CBT_UpdatePos& rhs);
	virtual ~CBT_UpdatePos() = default;

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
	_double				m_dUpdateTime = 0;
	_double				m_dOffset = 0;

	_tchar				m_pTargetKey[256] = { 0, };

	CTransform*			m_pTarget_Transform = nullptr;

public:
	static CBT_UpdatePos* Create_Prototype();
	virtual CBT_UpdatePos* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
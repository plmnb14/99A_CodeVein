#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_UpdateGageRatio final : public CBT_Service_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _Key_Save_GageRatio, _tchar* _Target_Key_MAXGage, _tchar* _Target_Key_CurGage, _uint _Count_Of_Execution, _double dUpdateTime, _double dOffset, CBT_Service_Node::Mode _eMode)
			: Target_dUpdateTime(dUpdateTime), Target_dOffset(dOffset), Count_Of_Execution(_Count_Of_Execution)
		{ strcpy_s<STR_128>(Target_NodeName, pNodeName);
		lstrcpy(Target_Save_GageRatio, _Key_Save_GageRatio);
		lstrcpy(Target_KeyMaxGage, _Target_Key_MAXGage);
		lstrcpy(Target_KeyCurGage, _Target_Key_CurGage);}

		char					Target_NodeName[STR_128] = { 0, };
		_tchar					Target_Save_GageRatio[STR_128] = { 0, };
		_tchar					Target_KeyMaxGage[STR_128] = { 0, };
		_tchar					Target_KeyCurGage[STR_128] = { 0, };
		_double					Target_dUpdateTime = 0;
		_double					Target_dOffset = 0;
		_uint					Count_Of_Execution = 0;
		CBT_Service_Node::Mode	eMode = CBT_Service_Node::Infinite;
	}INFO;

protected:
	explicit CBT_UpdateGageRatio();
	explicit CBT_UpdateGageRatio(const CBT_UpdateGageRatio& rhs);
	virtual ~CBT_UpdateGageRatio() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	// dMaxTime = m_dWaitingTime +- m_dOffset
	_double		m_dMaxTime = 0;

private:
	_double				m_dCurTime = 0;
	_double				m_dUpdateTime = 0;
	_double				m_dOffset = 0;

	_tchar				m_pKey_Save_GageRatio[STR_128] = { 0, };
	_tchar				m_pTargetKey_CurGage[STR_128] = { 0, };
	_tchar				m_pTargetKey_MaxGage[STR_128] = { 0, };

public:
	static CBT_UpdateGageRatio* Create_Prototype();
	virtual CBT_UpdateGageRatio* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
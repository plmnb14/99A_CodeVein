#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)

class CGameObject;
class ENGINE_DLL CBT_StartDissolve final : public CBT_Service_Node
{
public:
	enum Mode { One, Particle};

	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, CGameObject* pThisObject, _float _fFxSpeed, _bool _bFadeIn, _double _Service_Start_Time, CBT_Service_Node::Mode _eMode)
			: Target_Object(pThisObject), fFxSpeed(_fFxSpeed), _bFadeIn(_bFadeIn), Service_Start_Time(_Service_Start_Time), eMode(_eMode)
		{ strcpy_s<256>(Target_NodeName, pNodeName);
		}

		char					Target_NodeName[256] = {0,};
		CGameObject*			Target_Object = nullptr;
		_float					fFxSpeed = 0.f;
		_bool					_bFadeIn = false;
		_double					Service_Start_Time = 0;
		CBT_Service_Node::Mode	eMode = CBT_Service_Node::Infinite;
	}INFO;

protected:
	explicit CBT_StartDissolve();
	explicit CBT_StartDissolve(const CBT_StartDissolve& rhs);
	virtual ~CBT_StartDissolve() = default;

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
	CGameObject*			m_pThisObject = nullptr;
	_float					m_fFxSpeed = 0.f;
	_bool					m_bFadeIn = false;

	_double					m_dService_StartTime = 0;
	_bool					m_bService_Start = false;

	_double					m_dCurTime = 0;

public:
	static CBT_StartDissolve* Create_Prototype();
	virtual CBT_StartDissolve* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
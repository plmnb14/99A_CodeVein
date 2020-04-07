#pragma once

#include "..\headers\BT_Task_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_SetValue final : public CBT_Task_Node
{
public:
	enum Mode { BOOL, FLOAT, INT, VEC3 };

	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _pTarget_Key, _bool _bValue, _float _fValue, _int _iValue, _v3 _vValue, Mode _eMode)
			: bValue(_bValue), fValue(_fValue), iValue(_iValue), vValue(_vValue), eMode(_eMode)
		{ strcpy_s<256>(Target_NodeName, pNodeName); 
		lstrcpy(Target_Key, _pTarget_Key);}

		char			Target_NodeName[256] = {0,};
		_tchar			Target_Key[256] = { 0, };
		_bool			bValue = false;
		_float			fValue = 0.f;
		_int			iValue = 0;
		_v3				vValue = _v3(0.f, 0.f, 0.f);
		Mode			eMode = BOOL;
	}INFO;

protected:
	explicit CBT_SetValue();
	explicit CBT_SetValue(const CBT_SetValue& rhs);
	virtual ~CBT_SetValue() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	_tchar			m_pTarget_Key[256] = { 0, };
	_bool			m_bValue = false;
	_float			m_fValue = 0.f;
	_int			m_iValue = 0;
	_v3				m_vValue = _v3(0.f, 0.f, 0.f);

	Mode			m_eMode = BOOL;

public:
	static CBT_SetValue* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END
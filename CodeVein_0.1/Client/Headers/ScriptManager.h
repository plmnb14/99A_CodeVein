#pragma once

#define MaxTriggerCnt 32

#include "Base.h"
#include "Client_Defines.h"
#include "YetiTrap.h"

namespace Engine
{
	class CGameObject;
}

BEGIN(Client)

class CScriptManager : public CBase
{
	DECLARE_SINGLETON(CScriptManager)

private:
	typedef enum Script_Info
	{
		Script_Base , Script_Stage01, Script_Stage02, Script_Stage03, Script_Stage04, Script_Stage_End,
	}SCRIPT_SCENE;

private:
	explicit CScriptManager();
	virtual ~CScriptManager() = default;

public:
	virtual void Set_StageIdx(_ulong _dwStageIdx) { m_eStageIdx = (SCRIPT_SCENE)_dwStageIdx; }

public:
	virtual void Ready_ScriptManager(_Device _pGraphicDev);
	virtual void Ready_Script_DynamicObject(_ulong _dwStageIdx);
	virtual void Reset_ScriptEvent(_ushort _sScriptStageIdx , _bool _bResetAll = false);

public:
	virtual void Reset_Script_DynmicObject();

public:
	virtual void Update_ScriptMgr(_double _dDeltaTime , _ulong _dwSubsetIdx , _ulong _dwCellIdx);

	//======================================================================================================================

private:
	vector<CGameObject*>	m_vecObjectPool;
	vector<CYetiTrap*>		m_vecTrapPool;

private:
	SCRIPT_SCENE	m_eStageIdx = Script_Stage_End;

private:
	_Device			m_pGraphicDev = nullptr;

private:
	_double			m_fDeltaTime = 0;

private:
	_ulong			m_dwSubsetIdx = 0;
	_ulong			m_dwCellIdx = 0;

private:
	_bool			m_bEventTrigger[Script_Stage_End][MaxTriggerCnt] = {};

private:
	_bool			m_bTimer = false;
	_float			m_fTimer = 0.f;

private:
	virtual void St01_Sub0();
	virtual void St01_Sub1();
	virtual void St01_Sub2();
	virtual void St01_Sub3();
	virtual void St01_Sub4();
	virtual void St01_Sub5();
	virtual void St01_Sub6();

private:
	virtual void St02_Sub0();
	virtual void St02_Sub1();
	virtual void St02_Sub2();
	virtual void St02_Sub3();
	virtual void St02_Sub4();

private:
	virtual void St03_Sub0();
	virtual void St03_Sub1();
	virtual void St03_Sub2();
	virtual void St03_Sub3();
	virtual void St03_Sub4();

private:
	virtual void St04_Sub0();
	virtual void St04_Sub1();
	virtual void St04_Sub2();

private:
	virtual void Stage_Base_Scripts();
	virtual void Stage_01_Scripts();
	virtual void Stage_02_Scripts();
	virtual void Stage_03_Scripts();
	virtual void Stage_04_Scripts();

private:
	virtual void Ready_Base_DynamicObject();
	virtual void Ready_Stage01_DynamicObject();
	virtual void Ready_Stage02_DynamicObject();
	virtual void Ready_Stage03_DynamicObject();
	virtual void Ready_Stage04_DynamicObject();

private:
	virtual void Update_ScriptEvent();

public:
	virtual void Free();

public:
	void Check_YetiTrap();
};

END


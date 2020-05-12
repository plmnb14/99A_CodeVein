#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CStageAgent : public CBase
{
	DECLARE_SINGLETON(CStageAgent)

private:
	explicit CStageAgent();
	virtual ~CStageAgent() = default;

public:
	virtual void Update_StageAgent(_Device _pGraphicDev);

public:
	virtual void Reserve_ChangeStage();

public:
	virtual void Free();

private:
	virtual void Change_Stage(_Device _pGraphicDev);


private:
	_bool m_bReserveChangeStage = false;
	_short m_sOldStageIdx = 999;

};

END


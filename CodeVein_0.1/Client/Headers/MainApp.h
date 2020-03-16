#pragma once

// 내 게임을 진행하기위해ㅓㅅ.

#include "Client_Defines.h"
#include "Base.h"
#include "Management.h"

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Ready_MainApp();

public:
	_int	Update_MainApp(_double TimeDelta);
	void	LateUpdate_MainApp(_double TimeDelta);
	HRESULT Render_MainApp();

private:
	CRenderer*				m_pRenderer = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphic_Dev = nullptr;

private:
	HRESULT Ready_Default_Setting(CGraphic_Device::WINMODE eMode, _ushort nWinCX, _ushort nWinCY);
	HRESULT Ready_Component_Prototype();
	HRESULT Ready_Start_Scene(SCENEID eScene);

public:
	static CMainApp* Create();
	virtual void Free(); // 멤버변수의 정리.
};

END
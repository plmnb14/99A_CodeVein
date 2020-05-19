#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CMassageUI final : public CUI
{
private:
	explicit CMassageUI(_Device Graphic_Device);
	explicit CMassageUI(const CMassageUI& rhs);
	virtual ~CMassageUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint TextureIndex);
	void	SetUp_State(_double TimeDelta);

public:
	_bool	Get_Check_Play_BossnameUI() { return m_bCheck_Play_BossnameUI; }

public:
	void	Set_Check_Play_BossnameUI(_bool Check_Bossname) { m_bCheck_Play_BossnameUI = Check_Bossname; }
	void	Set_BossName(BOSS_NAME eBossName) { m_eBossName = eBossName; }

private:
	_bool		m_bCheck_MassageStart = false;
	_bool		m_bCheck_Play_BossnameUI = false;
	_bool		m_bPullAlpha = false;
	_bool		m_bReset = false;

	_bool		m_bTestbool = false;

	_bool		m_bCheck_Alpha0 = false;
	_bool		m_bCheck_Alpha1 = false;
	_bool		m_bCheck_Alpha2 = false;
	_bool		m_bCheck_Alpha3 = false;

	_float		m_fAlpha = 0.f;

	_float		m_fAlpha0 = 0.f;
	_float		m_fAlpha1 = 0.f;
	_float		m_fAlpha2 = 0.f;
	_float		m_fAlpha3 = 0.f;

	_float		m_fTimer = 0.f;

	_float		m_fNow_BarSize = 0.f;
	_float		m_fBarPercentage = 0.f;
	_float		m_fTextPercentage = 0.f;
	BOSS_NAME	m_eBossName = BossName_End;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CMassageUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
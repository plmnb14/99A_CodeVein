#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CGet_ItemUI final : public CUI
{
private:
	explicit CGet_ItemUI(_Device Graphic_Device);
	explicit CGet_ItemUI(const CGet_ItemUI& rhs);
	virtual ~CGet_ItemUI() = default;

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
	_bool	Get_Click_Check() { return m_bCheck_Click; }
	_uint	Get_Cound_Pickuptiem() { return m_iCount_PickUpitem; }
	_float	Get_EndTimer() { return m_fEndTimer; }

	void	Set_EndTimer(_double TimeDelta) { m_fEndTimer += (_float)TimeDelta; }

private:
	_bool		m_bCheck_Click = false;
	_bool		m_bSparkle_Box = false;
	_bool		m_bShow_GetItemName = false;
	_bool		m_bShow_Ask_Pickup = false;

	_float		m_fTimer = 0.f;
	_float		m_fEndTimer = 0.f;

	_float		m_fPercentage = 0.f;
	_float		m_fSparkleBox = 1.f;

	_float		m_fNowItemBar_Size = 0.f;

	_uint		m_iUINumber = 0;
	_uint		m_iCount_PickUpitem = 0;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;



public:
	static CGet_ItemUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
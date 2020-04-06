#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CDamegeNumUI final : public CUI
{
private:
	explicit CDamegeNumUI(_Device Graphic_Device);
	explicit CDamegeNumUI(const CDamegeNumUI& rhs);
	virtual ~CDamegeNumUI() = default;

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


private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CDamegeNumUI*	Create(_Device pGraphic_Device, CGameObject* MonsterTarget);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
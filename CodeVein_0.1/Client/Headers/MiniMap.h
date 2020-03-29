#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMiniMap final : public CUI
{
private:
	explicit CMiniMap(_Device pDevice);
	explicit CMiniMap(const CMiniMap& rhs);
	virtual ~CMiniMap() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CNavMesh*				m_pNavMesh = nullptr;

public:
	static CMiniMap*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
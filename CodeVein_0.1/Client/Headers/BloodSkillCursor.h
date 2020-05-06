#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodSkillCursor final : public CUI
{
private:
	explicit CBloodSkillCursor(_Device pDevice);
	explicit CBloodSkillCursor(const CBloodSkillCursor& rhs);
	virtual ~CBloodSkillCursor() = default;

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

private:
	_float m_fSpeed = 0.f;

public:
	static CBloodSkillCursor*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END

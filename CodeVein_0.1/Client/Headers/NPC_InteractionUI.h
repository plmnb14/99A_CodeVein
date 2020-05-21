#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CNPC_InteractionUI final : public CUI
{
private:
	explicit CNPC_InteractionUI(_Device pDevice);
	explicit CNPC_InteractionUI(const CNPC_InteractionUI& rhs);
	virtual ~CNPC_InteractionUI() = default;

public:
	_bool	Get_ReactConversation() { return m_bReact; }
public:
	void	Set_Alpha(_float fAlpha);
	void	Set_Active(_bool bIsActive);
	void	Set_Interaction(_bool bInteraction) {	m_bInteraction = bInteraction;	}
	void	Set_ReactConverSation(_bool bReact) {  m_bReact = bReact;}

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	Change_Texture(const _tchar* _Name);
	void	SetUp_Default();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CBuffer_RcTex*		m_pBufferCom = nullptr;

private:
	_bool				m_bLateInit = false;
	_bool				m_bInteraction = false;
	_bool				m_bReact = false;
	_int				m_iTexIdx = 0;
	_float				m_fAlpha = 0.f;

public:
	static CNPC_InteractionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END

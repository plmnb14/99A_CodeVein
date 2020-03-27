#pragma once

#include "UI.h"
#include "Client_Defines.h"




BEGIN(Client)
class CInven_Icon;
class CInventory final : public CUI
{
private:
	explicit CInventory(_Device pDevice);
	explicit CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	void Set_Move() { m_bIsMove = true; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Move_Inven(_double TimeDelta);
	void					Click_Icon();

private:
	HRESULT					SetUp_Icon(); // 인벤토리 생성 시, 버튼 생성

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	vector<CInven_Icon*>	m_vecIcon;
	_bool					m_bIsMove = false;
	_v2						m_vOldPos;

public:
	static CInventory*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
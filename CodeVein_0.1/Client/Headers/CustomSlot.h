#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CCustomSlot final : public CUI
{
public:
	enum SLOT_TYPE
	{
		TYPE_HAIR,
		TYPE_FACE,
		TYPE_EYE,
		TYPE_MASK,
		TYPE_INNER,
		TYPE_END
	};

private:
	explicit CCustomSlot(_Device pDevice);
	explicit CCustomSlot(const CCustomSlot& rhs);
	virtual ~CCustomSlot() = default;

public:
	bool Get_Select() { return m_bIsSelect; }

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_Type(SLOT_TYPE eType) { m_eType = eType; }
	void Set_SlotIdx(_int iIdx) { m_iSlotIdx = iIdx; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool Pt_InRect();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_int iIdx, _bool bElem = false);
	void	SetUp_Default();

	void	Check_LateInit();
	void	Check_SlotTexture();
	void	Change_Texture(const _tchar * _Name);


private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pElemTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	SLOT_TYPE				m_eType = TYPE_END;
	_int					m_iSlotIdx = 0;
	_bool					m_bIsSelect = false;
	_bool					m_bIsCollMouse = false;
	_bool					m_bLateInit = false;

public:
	static CCustomSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
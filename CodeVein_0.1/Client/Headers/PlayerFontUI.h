#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CFontNumUI;
class CPlayerFontUI final : public CUI
{
private:
	explicit CPlayerFontUI(_Device pDevice);
	explicit CPlayerFontUI(const CPlayerFontUI& rhs);
	virtual ~CPlayerFontUI() = default;

public:
	_ulong Get_Number() { return m_iNumber; }

public:
	void Set_Number(_ulong iNum) { m_iNumber = iNum; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	HRESULT Add_Component();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	_ulong		Calc_Digits(_ulong dwNumber);
	void		MyNumberFont();

private:
	vector<CFontNumUI*> m_vecFontNum;
	_ulong m_iNumber = 0; // ¼ýÀÚ
	_ulong m_iDigits = 0; // ÀÚ¸´¼ö


public:
	static CPlayerFontUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void Free();
};

END
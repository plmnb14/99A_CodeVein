#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CFontNumUI;
class CFontNumManager final : public CUI
{
private:
	explicit CFontNumManager(_Device pDevice);
	explicit CFontNumManager(const CFontNumManager& rhs);
	virtual ~CFontNumManager() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);

private:
	HRESULT Add_Component();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	//_ulong Calc_Digits();
	_ulong Calc_Digits(_ulong dwNumber);
	//void Calc_NumberFont();
	void Calc_NumberFont(_ulong dwFontNum, _float fPosX, _float fPosY, _float fSizeX, _float fSizeY, _float fViewZ);

private:
	CFontNumUI* Make_FontNum();

//private:
//	vector<CFontNumUI*> m_vecFontNum;
//	_ulong m_iNumber = 0; // ����
//	_ulong m_iDigits = 0; // �ڸ���
	

public:
	static CFontNumManager*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void Free();
};

END
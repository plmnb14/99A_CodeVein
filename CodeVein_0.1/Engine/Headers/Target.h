#pragma once

// ·»´õÅ¸°Ù ÇÏ³ª¸¦ ´ëÇ¥ÇÏ´Â °´Ã¼.

#include "Base.h"
#include "Buffer_ViewPort.h"

BEGIN(Engine)

class CTarget final : public CBase
{
private:
	explicit CTarget(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CTarget() = default;
public:
	LPDIRECT3DTEXTURE9 Get_Texture() const {
		return m_pTarget_Texture; }
public:
	HRESULT Ready_Render_Target(_uint iSizeX, _uint iSizeY, D3DFORMAT eFormat, D3DXCOLOR ClearColor);
	HRESULT SetUp_OnGraphicDev(_uint iIndex);
	HRESULT Release_OnGraphicDev(_uint iIndex);
	HRESULT Clear_Target();
#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Buffer(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer();
#endif
private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	LPDIRECT3DTEXTURE9		m_pTarget_Texture = nullptr;
	LPDIRECT3DSURFACE9		m_pTarget_Surface = nullptr; // ·»´õÅ¸°ÙÀ» ´ëÇ¥ÇÏ´Â °´Ã¼.
	LPDIRECT3DSURFACE9		m_pOld_Surface = nullptr;
	D3DXCOLOR				m_ClearColor;

#ifdef _DEBUG
private:
	CBuffer_ViewPort*		m_pViewPortBuffer = nullptr;
#endif
public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat, D3DXCOLOR ClearColor);
	virtual void Free();
};

END
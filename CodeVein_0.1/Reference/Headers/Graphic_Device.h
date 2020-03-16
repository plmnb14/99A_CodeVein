#ifndef GraphicDev_h__
#define GraphicDev_h__

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)
public:
	enum WINMODE { WINMODE_FULL, WINMODE_WIN };
private:
	explicit CGraphic_Device(void);
	virtual ~CGraphic_Device(void) = default;
public:
	LPDIRECT3DDEVICE9 Get_Graphic_Device(void) { 
		return m_pGraphicDev;  }
public:
	HRESULT	Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, LPDIRECT3DDEVICE9* ppGraphicDev = nullptr);

public:
	LPD3DXSPRITE		Get_Sprite();
	LPD3DXFONT			Get_Font();

public:
	void Render_Begin();
	void Render_End(HWND _hWnd);
	void Render_Sprite_Begin();
	void Render_Sprite_End();

private:
	LPDIRECT3D9				m_pSDK = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	LPD3DXSPRITE			m_pSprite = nullptr;
	LPD3DXFONT				m_pFont = nullptr;
public:
	virtual void Free(void);
};

END
#endif // GraphicDev_h__

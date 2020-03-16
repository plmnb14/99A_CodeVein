#pragma once

#include "Component.h"

BEGIN(Engine)

class CShader;
class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TEXTURETYPE {TYPE_GENERAL, TYPE_CUBE, TYPE_END};
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;
public:
	HRESULT SetUp_OnGraphicDev(_uint iStage, _uint iIndex = 0);
	HRESULT SetUp_OnShader(D3DXHANDLE hHandle, CShader* pShader, _uint iIndex = 0);
public:
	virtual HRESULT Ready_Component_Prototype(TEXTURETYPE eType, const _tchar* pFileName, const _uint& iCnt);
	virtual HRESULT Ready_Component(void* pArg);		
private:
	vector<IDirect3DBaseTexture9*>			m_vecTexture;
	typedef vector<IDirect3DBaseTexture9*>	VECTEXTURE;
private:
	_tchar									m_szFileName[MAX_PATH] = L"";
public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURETYPE eType, const _tchar* pFileName, const _uint& iCnt = 1);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
protected:
	virtual void Free();
};

END
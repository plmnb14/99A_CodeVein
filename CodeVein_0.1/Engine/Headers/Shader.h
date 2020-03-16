#pragma once

// 1. 외부에서 작성된 셰이더파일을 사용할 수 있게끔 준비해준다.
// 2. 셰이더로 그리기위한 기능.

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;
public:
	HRESULT Set_Bool(D3DXHANDLE Handle, _bool Bool);
	HRESULT Set_Value(D3DXHANDLE Handle, const void* pData, _uint iByte);
	HRESULT Set_Texture(D3DXHANDLE Handle, LPDIRECT3DBASETEXTURE9 pTexture);
	HRESULT Commit_Changes();
public:
	virtual HRESULT Ready_Component_Prototype(const _tchar* pFilePath);
	virtual HRESULT Ready_Component(void* pArg);
public:
	HRESULT Begin_Shader();
	HRESULT Begin_Pass(_uint iPassIdx);
	HRESULT End_Pass();
	HRESULT End_Shader();	
private:	
	LPD3DXEFFECT		m_pEffect = nullptr;	
	LPD3DXBUFFER		m_pErrorBuffer = nullptr;	
public:
	static CShader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
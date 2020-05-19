#pragma once

#include "Target.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager() = default;
public:
	LPDIRECT3DTEXTURE9 Get_Texture(const _tchar* pTargetTag);
public:
	HRESULT Add_Render_Target(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTargetTag, _uint iWidth, _uint iHeight, D3DFORMAT eFormat, D3DXCOLOR ClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(const _tchar* pMRTTag);
	HRESULT End_MRT(const _tchar* pMRTTag);
	HRESULT Begin_Render_Target(const _tchar* pTargetTag);
	HRESULT End_Render_Target(const _tchar* pTargetTag);
	HRESULT New_Stencil(const _tchar* pTargetTag);
	HRESULT Origin_Stencil(const _tchar* pTargetTag);

//#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Buffer(const _tchar* pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(const _tchar* pMRTTag);
	HRESULT Render_Debug_Buffer_Single(const _tchar* pTargetTag);
//#endif

private: // 렌더타겟을들을 산발적으로 추가.
	map<const _tchar*, CTarget*>			m_Targets;
	typedef map<const _tchar*, CTarget*>	TARGETS;


private: // 동시에 장치에 셋팅해야하는 렌더타겟을들을 묶어놓은 형태. 
	map<const _tchar*, list<CTarget*>>				m_MRTs;
	typedef map<const _tchar*, list<CTarget*>>		MRTS;

private:
	CTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<CTarget*>* Find_MRT(const _tchar* pMRTTag);
public:
	virtual void Free();
};

END
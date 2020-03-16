#pragma once

// 여러 형태의 정점 배열들을 가지는 클래스들의 부모.
// 정점버퍼를 가지는 객체들의 부모다.

#include "Component.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB = nullptr;
	_uint							m_iStride = 0; // 정점 하나의 크기.
	_uint							m_iNumVertices; // 정점들의 갯수.
	_ulong							m_dwFVF = 0; // 정점 멤버를 구성하는 fvf열거체.
	_v3*							m_pPosition = nullptr;
protected:
	LPDIRECT3DINDEXBUFFER9			m_pIB = nullptr;	
	_uint							m_iPolygonSize = 0;
	_uint							m_iNumPolygons = 0;
	D3DFORMAT						m_eFormat = D3DFORMAT(0);


public:
	virtual CComponent* Clone_Component(void* pArg = nullptr) = 0;
protected:
	virtual void Free();
};

END
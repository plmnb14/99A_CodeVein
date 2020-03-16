#pragma once

// ���� ������ ���� �迭���� ������ Ŭ�������� �θ�.
// �������۸� ������ ��ü���� �θ��.

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
	_uint							m_iStride = 0; // ���� �ϳ��� ũ��.
	_uint							m_iNumVertices; // �������� ����.
	_ulong							m_dwFVF = 0; // ���� ����� �����ϴ� fvf����ü.
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
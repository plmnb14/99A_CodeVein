#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CFlag final : public CGameObject
{
public:
	typedef struct tagFlagInfo
	{
		tagFlagInfo() {}
		tagFlagInfo(_uint _iNum_VTX_X, _uint _iNum_VTX_Y, _float _fSizeX, _float _fSizeY)
			: iNum_VTX_X(_iNum_VTX_X), iNum_VTX_Y(_iNum_VTX_Y), fSizeX(_fSizeX), fSizeY(_fSizeY)
		{}

		_uint iNum_VTX_X = 0;
		_uint iNum_VTX_Y = 0;
		_float fSizeX = 0;
		_float fSizeY = 0;
	}INFO;

private:
	struct tagFlag
	{
		_v3 Position;
		_v2 UV;
	};

protected:
	explicit CFlag(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFlag(const CFlag& rhs);
	virtual ~CFlag() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	void Set_Wind(const _v3 &vDir, const _float fStrength, const _v3& vRange);

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Flag(_uint iNum_VTX_X, _uint iNum_VTX_Y, _float fSizeX, _float fSizeY);

	physx::PxClothMeshDesc	CreateMeshGrid(physx::PxVec3 dirU, physx::PxVec3 dirV, physx::PxU32 numU, physx::PxU32 numV,
		vector<physx::PxVec4>& vertices, vector<physx::PxU32>& indices, vector<physx::PxVec2>& texcoords);

private:
	CTransform*						m_pTransformCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;

private:
	physx::PxCloth*					m_Cloth = nullptr;
	physx::PxReal					mTime = 0.f;

	// 바람
	physx::PxVec3					m_WindDir = { 0.f, 0.f, 0.f };
	physx::PxVec3					m_WindRange = { 0.f, 0.f, 0.f };
	physx::PxReal					m_WindStrength = 0.f;


	// 정점 버퍼
	_int							m_TriCount = 0;
	vector<physx::PxVec2>			m_UVs;

	IDirect3DVertexBuffer9*			m_pVB = nullptr;
	IDirect3DIndexBuffer9*			m_pIB = nullptr;

public:
	static CFlag* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
#include "stdafx.h"
#include "..\Headers\Flag.h"
#include "PxPhysicsAPI.h"

using namespace physx;

CFlag::CFlag(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFlag::CFlag(const CFlag & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlag::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFlag::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	INFO eTemp;

	if (nullptr != pArg)
		eTemp = *(INFO*)pArg;
	else
		eTemp = INFO(5, 5, 2.f, 1.f);

	Ready_Flag(eTemp.iNum_VTX_X, eTemp.iNum_VTX_Y, eTemp.fSizeX, eTemp.fSizeY);

	this->Set_Wind(_v3(1.f, 0.1f, 0), 40.0f, _v3(0.0f, 10.0f, 10.0f));

	m_pTransformCom->Set_Pos(V3_NULL);
	m_pTransformCom->Set_Scale(V3_ONE);

	return NOERROR;
}

_int CFlag::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	// 위치 전달.
	_v3 vMyPos = m_pTransformCom->Get_Pos();
	m_Cloth->setGlobalPose(PxTransform(vMyPos.x, vMyPos.y, vMyPos.z));

	PX_ASSERT(m_Cloth);

	mTime += _float(TimeDelta * CALC::Random_Num_Double(0.0f, 1.0f));

	_float st = 1.0f + (float)sin(mTime);
	_float windStrength = _float(CALC::Random_Num_Double(1.0f, st) * m_WindStrength);
	_float windRangeStrength = _float(CALC::Random_Num_Double(0.0f, 2.0f));

	PxVec3 offset(PxReal(CALC::Random_Num_Double(-1, 1)), PxReal(CALC::Random_Num_Double(-1, 1)), PxReal(CALC::Random_Num_Double(-1, 1)));
	_float ct = 1.0f + (float)cos(mTime + 0.1);
	offset *= ct;
	PxVec3 windAcceleration = windStrength * m_WindDir + windRangeStrength * m_WindRange.multiply(offset);

	m_Cloth->setExternalAcceleration(windAcceleration);


	m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this);

	return NOERROR;
}

_int CFlag::Late_Update_GameObject(_double TimeDelta)
{
	// 점정 위치 업데이트

	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	// 천 주소
	PxClothFabric* pFabric = m_Cloth->getFabric();

	// 깃발 파티클 데이터
	PxClothParticleData* pData = m_Cloth->lockParticleData();

	tagFlag* pVtx = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVtx, 0);

	_ulong		dwIndex = 0;

	for (_ulong i = 0; i < pFabric->getNbParticles(); ++i)
	{
		_v3 temp = *(_v3*)(pData->particles + i);

		pVtx[i].Position = temp;
		pVtx[i].UV = _v2(m_UVs[i].x, 1.f - m_UVs[i].y);
	}

	m_pVB->Unlock();

	return NOERROR;
}

HRESULT CFlag::Render_GameObject()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(14);

	m_pGraphic_Dev->SetStreamSource(0, m_pVB, 0, sizeof(tagFlag));
	m_pGraphic_Dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	m_pGraphic_Dev->SetIndices(m_pIB);
	m_pGraphic_Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_Cloth->getFabric()->getNbParticles(), 0, m_TriCount);

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return NOERROR;
}

void CFlag::Set_Wind(const _v3 & vDir, const _float fStrength, const _v3 & vRange)
{
	m_WindStrength = fStrength;
	m_WindDir = physx::PxVec3(vDir.x, vDir.y, vDir.z);
	m_WindRange = physx::PxVec3(vRange.x, vRange.y, vRange.z);
}

HRESULT CFlag::Add_Component(void * pArg)
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Default", L"Com_Shader", (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_HPBar", L"Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFlag::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	//m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0);


	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;


	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CFlag::Ready_Flag(_uint iNum_VTX_X, _uint iNum_VTX_Y, _float fSizeX, _float fSizeY)
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();


	PxTransform clothPose(PxVec3(0.f, 0.f, 0.f), PxQuat(PxIdentity));

	PxReal halfSizeX = 2.0f * 0.5f;
	PxReal halfSizeY = 1.0f * 0.5f;
	PxReal offset = 2.0f * 0.1f;
	PxReal halfHeight = 5.0f * 0.5f;

	PxReal clothHeight = 5.f - halfSizeY;

	// create the cloth flag mesh
	vector<PxVec4> vertices;
	vector<PxU32> primitives;
	PxClothMeshDesc meshDesc = CreateMeshGrid(
		PxVec3(2.0f, 0, 0), PxVec3(0, -1.0f, 0), iNum_VTX_X, iNum_VTX_Y, vertices, primitives, m_UVs);

	// attach two corners on the left
	for (PxU32 i = 0; i < meshDesc.points.count; i++)
	{
		PxReal u = m_UVs[i].x, v = m_UVs[i].y;
		bool constrain = ((u < 0.01) && (v < 0.01)) || ((u < 0.01) && (v > 0.99));
		vertices[i].w = constrain ? 0.0f : 1.0f;
	}

	// create cloth fabric
	PxClothFabric* clothFabric = PxClothFabricCreate(physics, meshDesc, PxVec3(0, -1, 0));
	PX_ASSERT(meshDesc.points.stride == sizeof(PxVec4));

	// create the cloth actor
	const PxClothParticle* particles = (const PxClothParticle*)meshDesc.points.data;
	PxCloth* cloth = physics.createCloth(clothPose, *clothFabric, particles, PxClothFlags());
	PX_ASSERT(cloth);

	// add this cloth into the scene
	scene.addActor(*cloth);

	// set solver settings
	cloth->setSolverFrequency(120);
	cloth->setDampingCoefficient(PxVec3(0.0f));

	cloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.1f));
	cloth->setTetherConfig(PxClothTetherConfig(1.0f, 1.0f));

	m_Cloth = cloth;

	return NOERROR;
}

physx::PxClothMeshDesc CFlag::CreateMeshGrid(physx::PxVec3 dirU, physx::PxVec3 dirV, physx::PxU32 numU, physx::PxU32 numV, vector<physx::PxVec4>& vertices, vector<physx::PxU32>& indices, vector<physx::PxVec2>& texcoords)
{
	_int numVertices = numU * numV;
	_int numQuads = (numU - 1) * (numV - 1);

	m_TriCount = numQuads * 2;

	vertices.resize(numVertices);
	indices.resize(numQuads * 4);
	texcoords.resize(numVertices);

	// fill in point data
	PxReal scaleU = 1 / PxReal(numU - 1);
	PxReal scaleV = 1 / PxReal(numV - 1);

	PxVec4* posIt = &(*vertices.begin());
	PxVec2* texIt = &(*texcoords.begin());
	for (PxU32 i = 0; i < numV; i++)
	{
		PxReal texV = i * scaleV;
		PxVec3 posV = (texV - 0.5f) * dirV;
		for (PxU32 j = 0; j < numU; j++)
		{
			PxReal texU = j * scaleU;
			PxVec3 posU = (texU - 0.5f) * dirU;
			*posIt++ = PxVec4(posU + posV, 1.0f);
			*texIt++ = PxVec2(texU, 1.0f - texV);
		}
	}

	// fill in quad index data
	PxU32 *idxIt = &(*indices.begin());
	for (PxU32 i = 0; i < numV - 1; i++)
	{
		for (PxU32 j = 0; j < numU - 1; j++)
		{
			PxU32 i0 = i * numU + j;
			*idxIt++ = i0;
			*idxIt++ = i0 + 1;
			*idxIt++ = i0 + numU + 1;
			*idxIt++ = i0 + numU;
		}
	}

	PxClothMeshDesc meshDesc;

	// convert vertex array to PxBoundedData (desc.points)
	meshDesc.points.data = &(*vertices.begin());
	meshDesc.points.count = static_cast<PxU32>(numVertices);
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &vertices.begin()->w;
	meshDesc.invMasses.count = static_cast<PxU32>(numVertices);
	meshDesc.invMasses.stride = sizeof(PxVec4);

	// convert index array to PxBoundedData (desc.quads)
	meshDesc.quads.data = &(*indices.begin());
	meshDesc.quads.count = static_cast<PxU32>(numQuads);
	meshDesc.quads.stride = sizeof(PxU32) * 4; // <- stride per quad

	// 정점 정보 저장소 만듦.
	m_pGraphic_Dev->CreateVertexBuffer(sizeof(tagFlag) * numVertices, 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, nullptr);
	m_pGraphic_Dev->CreateIndexBuffer(sizeof(PxU32) * 3 * m_TriCount, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB, nullptr);
	
	// 정점좌표는 Render에서 저장할 것임,
	// 정점 순서 저장.
	POLYGON32*		pIndex = nullptr;
	_ulong TriCount = 0;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong	dwIndex = 0;
	_ulong	dwTriIdx = 0;

	for (PxU32 i = 0; i < numV - 1; i++)
	{
		for (PxU32 j = 0; j < numU - 1; j++)
		{
			dwIndex = i * numU + j;

			pIndex[dwTriIdx]._0 = dwIndex + numU;
			pIndex[dwTriIdx]._1 = dwIndex + numU + 1;
			pIndex[dwTriIdx]._2 = dwIndex + 1;
			++dwTriIdx;

			pIndex[dwTriIdx]._0 = dwIndex + numU;
			pIndex[dwTriIdx]._1 = dwIndex + 1;
			pIndex[dwTriIdx]._2 = dwIndex;
			++dwTriIdx;
		}
	}

	m_pIB->Unlock();

	return meshDesc;
}

CFlag * CFlag::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFlag* pInstance = new CFlag(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFlag");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlag::Clone_GameObject(void * pArg)
{
	CFlag* pInstance = new CFlag(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CGunGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlag::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	CGameObject::Free();
}

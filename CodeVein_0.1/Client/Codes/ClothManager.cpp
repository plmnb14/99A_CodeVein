#include "stdafx.h"
#include "ClothManager.h"

IMPLEMENT_SINGLETON(CClothManager)

using namespace physx;

CClothManager::CClothManager()
{
}

HRESULT CClothManager::Ready_ClothManager()
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	for (_int i = 0; i < Static_End; ++i)
	{
		// 메쉬 이름 정함.
		_tchar name[STR_128] = { 0, };

		switch (i)
		{
		case Cloth_Static::Hair01:
			lstrcpy(name, L"Mesh_Hair_Long");
			break;
		default:
			MSG_BOX("CClothManager : Failed To Ready_ClothManager");
			return E_FAIL;
		}

		// 컴포넌트 잠깐 가져올 것임.
		CMesh_Static* pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, name));
	
		if (nullptr == pMesh_Static)
		{
			MSG_BOX("CClothManager : Failed To Get_Cloth_StaticCom");
			return E_FAIL;
		}

		// create the cloth cape mesh from file
		vector<PxVec4> vertices;
		vector<PxU16> primitives;
		PxClothMeshDesc meshDesc = CreateMesh(pMesh_Static->Get_pMesh(), 1.f, PxQuat(PxIdentity), PxVec3(0, 0, 0), vertices, primitives);

		if (!meshDesc.isValid())
			MSG_BOX(" PxCloth : Failed to Craete Mesh");


		switch (i)
		{
		case Cloth_Static::Hair01:
			Init_HairLong(meshDesc, vertices);
			break;
		}

		// 잠깐 가져온 것 제거
		Safe_Release(pMesh_Static);
	}

	for (_int i = 0; i < Dynamic_End; ++i)
	{
		// 메쉬 이름 정함.
		_tchar name[STR_128] = { 0, };

	}

	return S_OK;
}

HRESULT CClothManager::Update_Cloth_Static(Cloth_Static eTag)
{
	return S_OK;
}

HRESULT CClothManager::Update_Cloth_Dynamic(Cloth_Dynamic eTag)
{
	return S_OK;
}

physx::PxCloth * CClothManager::Get_Cloth_Static(Cloth_Static eStatic)
{
	if (eStatic > CClothManager::Static_End)
		return nullptr;

	return m_pCloth_Static[eStatic];
}

physx::PxCloth * CClothManager::Get_Cloth_Dynamic(Cloth_Dynamic eDynamic)
{
	if (eDynamic > CClothManager::Dynamic_End)
		return nullptr;

	return m_pCloth_Dynamic[eDynamic];
}

physx::PxClothMeshDesc CClothManager::CreateMesh(LPD3DXMESH pMesh, physx::PxReal scale, physx::PxQuat rot, physx::PxVec3 offset, vector<physx::PxVec4>& vertices, vector<physx::PxU16>& indices)
{
	if (nullptr == pMesh)
	{
		MSG_BOX("CCloth : n_pMesh is Nullptr");
		return PxClothMeshDesc();
	}

	_ulong numVertices = pMesh->GetNumVertices();
	_ulong numTriangles = pMesh->GetNumFaces();

	vertices.resize(numVertices);
	indices.resize(numTriangles * 3);

	vector<PxVec3> verticesTemp;
	verticesTemp.resize(numVertices);

	////////////////////////////////////////////////////
	// 버텍스 포지션 정보를 vec3 에서 vec4로 바꿔서 저장
	//IDirect3DVertexBuffer9* pVB = nullptr;
	_byte*		pVertices = nullptr;

	//m_pMesh->GetVertexBuffer(&pVB);
	//pVB->Lock(0, 0, (void**)&pVertices, 0);
	pMesh->LockVertexBuffer(0, (void**)&pVertices);

	_ulong dwStride = pMesh->GetNumBytesPerVertex();

	for (_ulong i = 0; i < numVertices; ++i)
	{
		PxVec3 vTemp = *(PxVec3*)(pVertices + (i * dwStride));
		verticesTemp[i] = vTemp;
	}

	PxVec3 *vSrc = (PxVec3*)&(*verticesTemp.begin());
	PxVec4 *vDest = &(*vertices.begin());
	for (_ulong i = 0; i < numVertices; i++, vDest++, vSrc++)
	{
		*vDest = PxVec4(scale * rot.rotate(*vSrc) + offset, 0.f);
	}

	//pVB->Unlock();
	pMesh->UnlockVertexBuffer();

	/////////////////////////////////////////////////////
	// 인덱스 정보 저장
	_byte *	pIndex = nullptr;

	pMesh->LockIndexBuffer(0, (void**)&pIndex);
	//pMesh->GetIndexBuffer(&m_pIB);
	//m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong i = 0; i < numTriangles * 3; ++i)
	{
		indices[i] = *((PxU16*)pIndex + i);
	}

	pMesh->UnlockIndexBuffer();


	PxClothMeshDesc meshDesc;

	// convert vertex array to PxBoundedData (desc.points)
	meshDesc.points.data = &(*vertices.begin());
	meshDesc.points.count = static_cast<PxU32>(numVertices);
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &vertices.begin()->w;
	meshDesc.invMasses.count = static_cast<PxU32>(numVertices);
	meshDesc.invMasses.stride = sizeof(PxVec4);

	// convert face index array to PxBoundedData (desc.triangles)
	meshDesc.triangles.data = &(*indices.begin());
	meshDesc.triangles.count = static_cast<PxU32>(numTriangles);
	meshDesc.triangles.stride = sizeof(PxU16) * 3; // <- stride per triangle
												   //meshDesc.triangles.stride = sizeof(PxU32) * 3; // <- stride per triangle

	meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;

	return meshDesc;
}

PxCloth* CClothManager::CreateQuadifier(PxClothMeshDesc& meshDesc)
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	PxClothMeshQuadifier quadifier(meshDesc);

	// Create Fabric
	PxClothFabric* clothFabric = PxClothFabricCreate(physics, quadifier.getDescriptor(), PxVec3(0, -1, 0));
	PX_ASSERT(quadifier.getDescriptor().points.stride == sizeof(PxVec4));

	// create the cloth
	const PxClothParticle* particles = (const PxClothParticle*)quadifier.getDescriptor().points.data;
	PxCloth* pOut = physics.createCloth(PxTransform(PxVec3(0.f, 0.f, 0.f), PxQuat(PxIdentity)), *clothFabric, particles, PxClothFlags());
	PX_ASSERT(pOut);

	return pOut;
}

void CClothManager::Init_HairLong(physx::PxClothMeshDesc& pClothMeshDesc, vector<PxVec4>& vertices)
{
	// Long Hair
	for (PxU32 i = 0; i < 4241; i++)
	{
		//if (mVertices[i].y < 1.5f)
		if (vertices[i].y < 1.4f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair01] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair01];

	scene.addActor(*pCloth);

	// set solver settings
	pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

													 // reduce impact of frame acceleration
													 // x, z: cloth swings out less when walking in a circle
													 // y: cloth responds less to jump acceleration
	pCloth->setLinearInertiaScale(PxVec3(0.2f, 0.3f, 0.2f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	pCloth->setAngularInertiaScale(PxVec3(0.2f));

	// reduce centrifugal force of rotating frame
	pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

void CClothManager::Init_HairPonyTail()
{
}

void CClothManager::Init_Muffler()
{
}

void CClothManager::Free()
{
	// 옷은 피직스가 내부에서 알아서 제거해줄 거임.
}

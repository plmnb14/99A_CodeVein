#include "stdafx.h"
#include "ClothManager.h"

IMPLEMENT_SINGLETON(CClothManager)

using namespace physx;

CClothManager::CClothManager()
{
}

HRESULT CClothManager::Ready_ClothManager()
{
	Ready_Cloth_Static();
	//Ready_Cloth_Dynamic();

	return S_OK;
}

HRESULT CClothManager::Update_Cloth_Static(Cloth_Static eTag)
{
	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	if (nullptr == m_pPlayerFrame[Neck])
		Ready_matPlayerBone();

	_v3 vPlayerPos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();

	PxTransform rootPose = PxTransform(PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z));;

	m_pCloth_Static[eTag]->setTargetPose(rootPose);



	switch (eTag)
	{
	case Cloth_Static::Hair01:
		Update_Hair_ColPos(m_pCloth_Static[Hair01]);
		break;
	case Cloth_Static::Hair02:
		Update_Hair_ColPos(m_pCloth_Static[Hair02]);
		break;
	case Cloth_Static::Hair03:
		Update_Hair_ColPos(m_pCloth_Static[Hair03]);
		break;
	case Cloth_Static::Hair04:
		Update_Hair_ColPos(m_pCloth_Static[Hair04]);
		break;
	case Cloth_Static::Hair05:
		Update_Hair_ColPos(m_pCloth_Static[Hair05]);
		break;
	case Cloth_Static::Hair06:
		Update_Hair_ColPos(m_pCloth_Static[Hair06]);
		break;
	case Cloth_Static::Hair07:
		Update_Hair_ColPos(m_pCloth_Static[Hair07]);
		break;

	default:
		break;
	}



	return S_OK;
}

HRESULT CClothManager::Update_Cloth_Dynamic(Cloth_Dynamic eTag)
{
	if (nullptr == m_pPlayerFrame[Neck])
		Ready_matPlayerBone();


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

HRESULT CClothManager::Ready_Cloth_Static()
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	for (_int i = 0; i < Static_End; ++i)
	{
		// ¸Þ½¬ ÀÌ¸§ Á¤ÇÔ.
		_tchar name[STR_128] = { 0, };

		switch (i)
		{
		case Cloth_Static::Hair01:
			lstrcpy(name, L"Mesh_Hair_01");
			break;

		case Cloth_Static::Hair02:
			lstrcpy(name, L"Mesh_Hair_02");
			break;

		case Cloth_Static::Hair03:
			lstrcpy(name, L"Mesh_Hair_03");
			break;

		case Cloth_Static::Hair04:
			lstrcpy(name, L"Mesh_Hair_04");
			break;

		case Cloth_Static::Hair05:
			lstrcpy(name, L"Mesh_Hair_05");
			break;

		case Cloth_Static::Hair06:
			lstrcpy(name, L"Mesh_Hair_06");
			break;

		case Cloth_Static::Hair07:
			lstrcpy(name, L"Mesh_Hair_07");
			break;

		default:
			MSG_BOX("CClothManager : Failed To Ready_ClothManager");
			return E_FAIL;
		}

		// ÄÄÆ÷³ÍÆ® Àá±ñ °¡Á®¿Ã °ÍÀÓ.
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
			Init_Hair01(meshDesc, vertices);
			break;

		case Cloth_Static::Hair02:
			Init_Hair02(meshDesc, vertices);
			break;

		case Cloth_Static::Hair03:
			Init_Hair03(meshDesc, vertices);
			break;

		case Cloth_Static::Hair04:
			Init_Hair04(meshDesc, vertices);
			break;

		case Cloth_Static::Hair05:
			Init_Hair05(meshDesc, vertices);
			break;

		case Cloth_Static::Hair06:
			Init_Hair06(meshDesc, vertices);
			break;

		case Cloth_Static::Hair07:
			Init_Hair07(meshDesc, vertices);
			break;

		}

		// Àá±ñ °¡Á®¿Â °Í Á¦°Å
		Safe_Release(pMesh_Static);
	}

	return S_OK;
}

HRESULT CClothManager::Ready_Cloth_Dynamic()
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	for (_int i = 0; i < Dynamic_End; ++i)
	{
		// ¸Þ½¬ ÀÌ¸§ Á¤ÇÔ.
		_tchar name[STR_128] = { 0, };

		PxScene& scene = *g_pPhysx->Get_Scene();
		PxPhysics& physics = *g_pPhysx->Get_Physics();

		for (_int i = 0; i < Static_End; ++i)
		{
			// ¸Þ½¬ ÀÌ¸§ Á¤ÇÔ.
			_tchar name[STR_128] = { 0, };

			switch (i)
			{
			case Cloth_Dynamic::Drape:
				//lstrcpy(name, L"Mesh_Hair_01");
				break;

			case Cloth_Dynamic::LongCoat:
				//lstrcpy(name, L"Mesh_Hair_02");
				break;

			case Cloth_Dynamic::Gauntlet:
				//lstrcpy(name, L"Mesh_Hair_03");
				break;

			case Cloth_Dynamic::Muffler:
				//lstrcpy(name, L"Mesh_Hair_04");
				break;

			default:
				MSG_BOX("CClothManager : Failed To Ready_ClothManager");
				return E_FAIL;
			}

			// ÄÄÆ÷³ÍÆ® Àá±ñ °¡Á®¿Ã °ÍÀÓ.
			CMesh_Dynamic* pMesh_Static = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, name));

			if (nullptr == pMesh_Static)
			{
				MSG_BOX("CClothManager : Failed To Get_Cloth_StaticCom");
				return E_FAIL;
			}

			// create the cloth cape mesh from file
			vector<PxVec4> vertices;
			vector<PxU16> primitives;
			PxClothMeshDesc meshDesc = CreateMesh(pMesh_Static->Get_MeshContainer()[0]->pOriginalMesh, 1.f, PxQuat(PxIdentity), PxVec3(0, 0, 0), vertices, primitives);

			if (!meshDesc.isValid())
				MSG_BOX(" PxCloth : Failed to Craete Mesh");


			switch (i)
			{
			case Cloth_Dynamic::Drape:
				Init_Drape(meshDesc, vertices);
				break;

			case Cloth_Dynamic::LongCoat:
				Init_LongCoat(meshDesc, vertices);
				break;

			case Cloth_Dynamic::Gauntlet:
				Init_Gauntlet(meshDesc, vertices);
				break;

			case Cloth_Dynamic::Muffler:
				Init_Muffler(meshDesc, vertices);
				break;

			}

			// Àá±ñ °¡Á®¿Â °Í Á¦°Å
			Safe_Release(pMesh_Static);
		}
	}

	return S_OK;
}

HRESULT CClothManager::Ready_matPlayerBone()
{
	CMesh_Dynamic* pMeshCom = TARGET_TO_D_MESH(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	if (nullptr == pMeshCom)
		return E_FAIL;

	// ¸ñ
	m_pPlayerFrame[Neck] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("Neck", 1);

	// ¾ûµ¢ÀÌ
	m_pPlayerFrame[Hips] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("Hips", 0);

	// ¿Þ¾î±ú
	m_pPlayerFrame[LeftArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftArm", 3);

	// ¿ÞÆÈ²ÞÄ¡
	m_pPlayerFrame[LeftForeArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftForeArm", 3);

	// ¿Þ¼Õ
	m_pPlayerFrame[LeftHand] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftHand", 3);

	// ¿À¸¥¾î±ú
	m_pPlayerFrame[RightArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightArm", 2);

	// ¿À¸¥ÆÈ²ÞÄ¡
	m_pPlayerFrame[RightForeArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightForeArm", 2);

	// ¿À¸¥¼Õ
	m_pPlayerFrame[RightHand] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightHand", 2);

	// ¿Þ°ñ¹Ý
	m_pPlayerFrame[LeftUpLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftUpLeg", 0);

	// ¿Þ¹«¸­
	m_pPlayerFrame[LeftLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftLeg", 0);

	// ¿Þ¹ß
	m_pPlayerFrame[LeftFoot] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftFoot", 0);

	// ¿À¸¥°ñ¹Ý
	m_pPlayerFrame[RightUpLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightUpLeg", 0);

	// ¿À¸¥¹«¸­
	m_pPlayerFrame[RightLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightLeg", 0);

	// ¿À¸¥¹ß
	m_pPlayerFrame[RightFoot] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightFoot", 0);

	return S_OK;
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
	// ¹öÅØ½º Æ÷Áö¼Ç Á¤º¸¸¦ vec3 ¿¡¼­ vec4·Î ¹Ù²ã¼­ ÀúÀå
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
	// ÀÎµ¦½º Á¤º¸ ÀúÀå
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

void CClothManager::Update_Hair_ColPos(physx::PxCloth* pCloth)
{
	// ÃÖÃÊ ¾÷µ¥ÀÌÆ® ½Ã ¿ø°ú Ä¸½¶ »ý¼º.
	if (0 == pCloth->getNbCollisionSpheres())
	{
		vector<PxClothCollisionSphere> vecSpheres;
		vecSpheres.resize(2);

		_v3 vNeck = *(_v3*)(&(m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3])) - _v3(0.f, 1.4992f, 0.f);// *pTransform->Get_WorldMat()).m[3]);
		vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vNeck), 0.07f));

		_v3 vHip = *(_v3*)(&(m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3])) - _v3(0.f, 1.4992f, 0.f);// * pTransform->Get_WorldMat()).m[3]);
		vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vNeck), 0.1f));

		pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

		pCloth->addCollisionCapsule(0, 1);
	}



	vector<PxClothCollisionSphere> vecSpheres;
	vecSpheres.resize(2);

	// Ãæµ¹±¸ : ¸ñ, ¾ûµ¢ÀÌ 
	pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

	vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3]) - PxVec3(0.f, 1.4992f, 0.f);
	vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]) - PxVec3(0.f, 1.4992f, 0.f);

	pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

}

void CClothManager::Init_Hair01(physx::PxClothMeshDesc& pClothMeshDesc, vector<PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 9821; i++)
	{
		if (vertices[i].y < 0.04f &&
			vertices[i].z < -0.04f)
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

void CClothManager::Init_Hair02(physx::PxClothMeshDesc& pClothMeshDesc, vector<PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 7057; i++)
	{
		if (vertices[i].y < 0.02f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair02] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair02];

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

void CClothManager::Init_Hair03(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 4241; i++)
	{
		if (vertices[i].y < 0.04f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair03] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair03];

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

void CClothManager::Init_Hair04(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 3464; i++)
	{
		if (vertices[i].y < 0.04f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair04] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair04];

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

void CClothManager::Init_Hair05(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 8203; i++)
	{
		if (vertices[i].y < 0.1f &&
			vertices[i].z < -0.07)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair05] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair05];

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

void CClothManager::Init_Hair06(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 6544; i++)
	{
		if (vertices[i].y < 0.02f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair06] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair06];

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

void CClothManager::Init_Hair07(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 6976; i++)
	{
		if (vertices[i].y < 0.03f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair07] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Static[Hair07];

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

void CClothManager::Init_Drape(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices)
{

}

void CClothManager::Init_LongCoat(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	/*
	Attribld		0
	VertexStart	0
	VertexCount	11855

	Attribld		1
	VertexStart	11855
	VertexCount	2454

	Attribld		2
	VertexStart	14309
	VertexCount	1384

	Attribld		3
	VertexStart	15693
	VertexCount	2454

	Attribld		4
	VertexStart	18147
	VertexCount	1384
	*/

	//for (PxU32 i = 0; i < 6976; i++)
	//{
	//	//if (mVertices[i].y < 1.5f)
	//	if (vertices[i].y < 0.05f)
	//		vertices[i].w = 0.1f;
	//}


	m_pCloth_Dynamic[LongCoat] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxCloth* pCloth = m_pCloth_Dynamic[LongCoat];

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

void CClothManager::Init_Gauntlet(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	/*
	Attribld		0
	VertexStart	0
	VertexCount	11533

	Attribld		1
	VertexStart	11533
	VertexCount	1404

	Attribld		2
	VertexStart	12937
	VertexCount	1404
	*/

	for (PxU32 i = 11533; i < 14341; i++)
	{
		if (vertices[i].y < 1.3f)
			vertices[i].w = 0.1f;
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

void CClothManager::Init_Muffler(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	/*
	Attribld		0
	VertexStart	0
	VertexCount	12570

	Attribld		1
	VertexStart	12570
	VertexCount	912

	Attribld		2
	VertexStart	13482
	VertexCount	912
	*/

	for (PxU32 i = 0; i < 12570; i++)
	{
		if (vertices[i].y < 1.25f)
			vertices[i].w = 0.8f;
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

void CClothManager::Free()
{
	// ¿ÊÀº ÇÇÁ÷½º°¡ ³»ºÎ¿¡¼­ ¾Ë¾Æ¼­ Á¦°ÅÇØÁÙ °ÅÀÓ.
}

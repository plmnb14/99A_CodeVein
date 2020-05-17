#include "stdafx.h"
#include "ClothManager.h"

IMPLEMENT_SINGLETON(CClothManager)

using namespace physx;

CClothManager::CClothManager()
{
}

HRESULT CClothManager::Ready_ClothManager()
{
	//Ready_Cloth_Dynamic();
	Ready_Cloth_Static();
	Ready_ColRadius();

	return S_OK;
}

HRESULT CClothManager::Update_Cloth_Static(Cloth_Static eTag, _bool bCanCol)
{
	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	if (nullptr == m_pPlayerFrame[Neck])
		Ready_matPlayerBone();

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	//스케일, 쿼터니안, 위치, 대상 
	D3DXQUATERNION quater;
	D3DXVECTOR3 vPlayerPos;
	D3DXMatrixDecompose(&D3DXVECTOR3(), &quater, &vPlayerPos, &TARGET_TO_TRANS(pPlayer)->Get_WorldMat());

	PxTransform rootPose = PxTransform(PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z), *(PxQuat*)&quater);

	m_pCloth_Static[eTag]->setTargetPose(rootPose);

	Set_Wind(m_pCloth_Static[eTag], g_vWindDir);

	// 스킬 쓸 때는 충돌구 잠시 제거
	if (bCanCol)
	{
		Clear_Collider(m_pCloth_Static[eTag]);
	}
	else
	{
		Update_CurRadius();

		Update_Hair_ColPos(m_pCloth_Static[eTag]);
	}


	return S_OK;
}

HRESULT CClothManager::Update_Cloth_Dynamic(Cloth_Dynamic eTag, _bool bCanCol)
{
	if (Cloth_Dynamic::None == eTag)
		return S_OK;

	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	if (nullptr == m_pPlayerFrame[Neck])
		Ready_matPlayerBone();

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	//스케일, 쿼터니안, 위치, 대상 
	D3DXQUATERNION quater;
	D3DXVECTOR3 vPlayerPos;
	D3DXMatrixDecompose(&D3DXVECTOR3(), &quater, &vPlayerPos, &TARGET_TO_TRANS(pPlayer)->Get_WorldMat());

	PxTransform rootPose = PxTransform(PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z), *(PxQuat*)&quater);

	m_pCloth_Dynamic[eTag]->setTargetPose(rootPose);

	Set_Wind(m_pCloth_Dynamic[eTag], g_vWindDir);

	// 스킬 쓸 때는 충돌구 잠시 제거
	if (bCanCol)
	{
		Clear_Collider(m_pCloth_Static[eTag]);
	}
	else
	{
		Update_CurRadius();

		Update_Cloth_ColPos(m_pCloth_Dynamic[eTag]);
	}


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

void CClothManager::Set_Sleep_AllStatic()
{
	for (_uint i = 0; i < Static_End; ++i)
	{
		if (m_pCloth_Static[i])
		{
			Clear_Collider(m_pCloth_Static[i]);

			m_pCloth_Static[i]->putToSleep();

		}
	}
}

void CClothManager::Set_Sleep_AllDynamic()
{
	for (_uint i = 0; i < Dynamic_End; ++i)
	{
		if (m_pCloth_Dynamic[i])
		{
			Clear_Collider(m_pCloth_Dynamic[i]);

			m_pCloth_Dynamic[i]->putToSleep();

		}
	}
}

void CClothManager::Clear_Collider(physx::PxCloth* pCloth)
{
	Reset_CurRadius();

	for (_uint i = 0; i < pCloth->getNbCollisionSpheres(); ++i)
		pCloth->removeCollisionSphere(i);

	for (_uint i = 0; i < pCloth->getNbCollisionCapsules(); ++i)
		pCloth->removeCollisionCapsule(i);
}

HRESULT CClothManager::Ready_Cloth_Static()
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	PxSceneWriteLock scopedLock(scene);

	for (_int i = 0; i < Static_End; ++i)
	{
		// 메쉬 이름 정함.
		_tchar name[STR_128] = { 0, };

		switch (i)
		{
		case Cloth_Static::Hair_01:
			lstrcpy(name, L"Mesh_Hair_01");
			break;

		case Cloth_Static::Hair_02:
			lstrcpy(name, L"Mesh_Hair_02");
			break;

		case Cloth_Static::Hair_03:
			lstrcpy(name, L"Mesh_Hair_03");
			break;

		case Cloth_Static::Hair_04:
			lstrcpy(name, L"Mesh_Hair_04");
			break;

		case Cloth_Static::Hair_05:
			lstrcpy(name, L"Mesh_Hair_05");
			break;

		case Cloth_Static::Hair_06:
			lstrcpy(name, L"Mesh_Hair_06");
			break;

		case Cloth_Static::Hair_07:
			lstrcpy(name, L"Mesh_Hair_07");
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
		case Cloth_Static::Hair_01:
			Init_Hair01(meshDesc, vertices, pMesh_Static);
			break;

		case Cloth_Static::Hair_02:
			Init_Hair02(meshDesc, vertices, pMesh_Static);
			break;

		case Cloth_Static::Hair_03:
			Init_Hair03(meshDesc, vertices, pMesh_Static);
			break;

		case Cloth_Static::Hair_04:
			Init_Hair04(meshDesc, vertices, pMesh_Static);
			break;

		case Cloth_Static::Hair_05:
			Init_Hair05(meshDesc, vertices, pMesh_Static);
			break;

		case Cloth_Static::Hair_06:
			Init_Hair06(meshDesc, vertices, pMesh_Static);
			break;

		case Cloth_Static::Hair_07:
			Init_Hair07(meshDesc, vertices, pMesh_Static);
			break;

		}

		// 잠깐 가져온 것 제거
		Safe_Release(pMesh_Static);

		m_bValid_CLoth_Static[i] = true;
	}

	return S_OK;
}

HRESULT CClothManager::Ready_Cloth_Dynamic()
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	PxSceneWriteLock scopedLock(scene);

	for (_int i = 0; i < Dynamic_End; ++i)
	{
		// 메쉬 이름 정함.
		_tchar name[STR_128] = { 0, };

		switch (i)
		{
		case Cloth_Dynamic::None:
			lstrcpy(name, L"Mesh_Player");
			continue;	// None은 Pass

		case Cloth_Dynamic::Drape_01:
			lstrcpy(name, L"Mesh_Drape_01");
			break;

		case Cloth_Dynamic::Gauntlet_01:
			lstrcpy(name, L"Mesh_Gauntlet_01");
			break;
		case Cloth_Dynamic::Gauntlet_03:
			lstrcpy(name, L"Mesh_Gauntlet_03");
			break;

		case Cloth_Dynamic::LongCoat_01:
			lstrcpy(name, L"Mesh_LongCoat_01");
			break;
		case Cloth_Dynamic::LongCoat_02:
			lstrcpy(name, L"Mesh_LongCoat_02");
			break;
		case Cloth_Dynamic::LongCoat_03:
			lstrcpy(name, L"Mesh_LongCoat_03");
			break;

		case Cloth_Dynamic::Muffler_01:
			lstrcpy(name, L"Mesh_Muffler_01");
			break;
		case Cloth_Dynamic::Muffler_02:
			lstrcpy(name, L"Mesh_Muffler_02");
			break;

		default:
			MSG_BOX("CClothManager : Failed To Ready_ClothManager");
			return E_FAIL;
		}

		// 컴포넌트 잠깐 가져올 것임.
		CMesh_Dynamic* pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, name));

		if (nullptr == pMesh_Dynamic)
		{
			MSG_BOX("CClothManager : Failed To Get_Cloth_StaticCom");
			return E_FAIL;
		}

		// create the cloth cape mesh from file
		vector<PxVec4> vertices;
		vector<PxU16> primitives;
		PxClothMeshDesc meshDesc = CreateMesh(pMesh_Dynamic->Get_MeshContainer()[0]->pOriginalMesh, 1.f, PxQuat(PxIdentity), PxVec3(0, 0, 0), vertices, primitives);

		if (!meshDesc.isValid())
			MSG_BOX(" PxCloth : Failed to Craete Mesh");


		switch (i)
		{
		case Cloth_Dynamic::Drape_01:
			Init_Drape_01(meshDesc, vertices);
			break;
		case Cloth_Dynamic::Gauntlet_01:
			Init_Gauntlet_01(meshDesc, vertices);
			break;
		case Cloth_Dynamic::Gauntlet_03:
			Init_Gauntlet_03(meshDesc, vertices);
			break;

		case Cloth_Dynamic::LongCoat_01:
			Init_LongCoat_01(meshDesc, vertices);
			break;
		case Cloth_Dynamic::LongCoat_02:
			Init_LongCoat_02(meshDesc, vertices);
			break;
		case Cloth_Dynamic::LongCoat_03:
			Init_LongCoat_03(meshDesc, vertices);
			break;

		case Cloth_Dynamic::Muffler_01:
			Init_Muffler_01(meshDesc, vertices);
			break;
		case Cloth_Dynamic::Muffler_02:
			Init_Muffler_02(meshDesc, vertices);
			break;

		default:
			break;
		}

		// 잠깐 가져온 것 제거
		Safe_Release(pMesh_Dynamic);

		m_bValid_CLoth_Dynamic[i] = true;
	}

	return S_OK;
}

HRESULT CClothManager::Ready_matPlayerBone()
{
	CMesh_Dynamic* pMeshCom = TARGET_TO_D_MESH(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	if (nullptr == pMeshCom)
		return E_FAIL;

	// 목
	m_pPlayerFrame[Neck] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("Neck", 1);

	// 엉덩이
	m_pPlayerFrame[Hips] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("Hips", 0);

	// 왼어깨
	m_pPlayerFrame[LeftArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftArm", 3);

	// 왼팔꿈치
	m_pPlayerFrame[LeftForeArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftForeArm", 3);

	// 왼손
	m_pPlayerFrame[LeftHand] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftHand", 3);

	// 오른어깨
	m_pPlayerFrame[RightArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightArm", 2);

	// 오른팔꿈치
	m_pPlayerFrame[RightForeArm] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightForeArm", 2);

	// 오른손
	m_pPlayerFrame[RightHand] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightHand", 2);

	// 왼골반
	m_pPlayerFrame[LeftUpLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftUpLeg", 0);

	// 왼무릎
	m_pPlayerFrame[LeftLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftLeg", 0);

	// 왼발
	m_pPlayerFrame[LeftFoot] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("LeftFoot", 0);

	// 오른골반
	m_pPlayerFrame[RightUpLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightUpLeg", 0);

	// 오른무릎
	m_pPlayerFrame[RightLeg] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightLeg", 0);

	// 오른발
	m_pPlayerFrame[RightFoot] = (D3DXFRAME_DERIVED*)pMeshCom->Get_BonInfo("RightFoot", 0);

	return S_OK;
}

HRESULT CClothManager::Ready_ColRadius()
{
	// 구 최대 반지름
	m_fMaxRadius[Neck] = 0.13f;
	m_fMaxRadius[Hips] = 0.13f;
	m_fMaxRadius[LeftArm] = 0.07f;
	m_fMaxRadius[LeftForeArm] = 0.07f;
	m_fMaxRadius[LeftHand] = 0.07f;
	m_fMaxRadius[RightArm] = 0.07f;
	m_fMaxRadius[RightForeArm] = 0.07f;
	m_fMaxRadius[RightHand] = 0.07f;
	m_fMaxRadius[LeftUpLeg] = 0.1f;
	m_fMaxRadius[LeftLeg] = 0.1f;
	m_fMaxRadius[LeftFoot] = 0.1f;
	m_fMaxRadius[RightUpLeg] = 0.1f;
	m_fMaxRadius[RightLeg] = 0.1f;
	m_fMaxRadius[RightFoot] = 0.1f;

	// 구 현재 반지름
	m_fCurRadius[Neck] = 0.13f;
	m_fCurRadius[Hips] = 0.13f;
	m_fCurRadius[LeftArm] = 0.07f;
	m_fCurRadius[LeftForeArm] = 0.07f;
	m_fCurRadius[LeftHand] = 0.07f;
	m_fCurRadius[RightArm] = 0.07f;
	m_fCurRadius[RightForeArm] = 0.07f;
	m_fCurRadius[RightHand] = 0.07f;
	m_fCurRadius[LeftUpLeg] = 0.1f;
	m_fCurRadius[LeftLeg] = 0.1f;
	m_fCurRadius[LeftFoot] = 0.1f;
	m_fCurRadius[RightUpLeg] = 0.1f;
	m_fCurRadius[RightLeg] = 0.1f;
	m_fCurRadius[RightFoot] = 0.1f;

	return S_OK;
}

void CClothManager::Set_Wind(physx::PxCloth * pCloth, _v3 vWindDir)
{
	D3DXVec3Normalize(&vWindDir, &vWindDir);

	PxReal strength = 5.0f;
	PxVec3 offset(PxReal(CALC::Random_Num_Double(-1, 1)) + vWindDir.x, PxReal(CALC::Random_Num_Double(-1, 1)) + vWindDir.y, PxReal(CALC::Random_Num_Double(-1, 1)) + vWindDir.z);
	PxVec3 windAcceleration = strength * offset;
	pCloth->setExternalAcceleration(windAcceleration);
}

void CClothManager::Set_Cloth_Default(physx::PxCloth * pCloth)
{
	// set solver settings
	pCloth->setSolverFrequency(120);

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

void CClothManager::Update_Hair_ColPos(physx::PxCloth* pCloth)
{
	_v3 vHairOffset = _v3(0.f, -1.4992f, 0.f);
	PxVec3 PxVHairOffset = PxVec3(0.f, -1.4992f, 0.f);

	// 긴머리만 충돌구 많이 생성
	if (pCloth == m_pCloth_Static[Hair_03])
	{
		// 최초 업데이트 시 구와 캡슐 생성.
		if (0 == pCloth->getNbCollisionSpheres())
		{
			vector<PxClothCollisionSphere> vecSpheres;
			vecSpheres.resize(10);

			_v3 vNeck = *(_v3*)(&(m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vNeck), m_fCurRadius[Neck]));

			_v3 vHip = *(_v3*)(&(m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vHip), m_fCurRadius[Hips]));

			_v3 vLeftArm = *(_v3*)(&(m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[2] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftArm), m_fCurRadius[LeftArm]));

			_v3 vLeftForeArm = *(_v3*)(&(m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[3] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftForeArm), m_fCurRadius[LeftForeArm]));

			_v3 vRightArm = *(_v3*)(&(m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[4] = (PxClothCollisionSphere(*(PxVec3*)(&vRightArm), m_fCurRadius[RightArm]));

			_v3 vRightForeArm = *(_v3*)(&(m_pPlayerFrame[RightForeArm]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[5] = (PxClothCollisionSphere(*(PxVec3*)(&vRightForeArm), m_fCurRadius[RightForeArm]));

			_v3 vLeftUpLeg = *(_v3*)(&(m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[6] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftUpLeg), m_fCurRadius[LeftUpLeg]));

			_v3 vLeftLeg = *(_v3*)(&(m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[7] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftLeg), m_fCurRadius[LeftLeg]));

			_v3 vRightUpLeg = *(_v3*)(&(m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[8] = (PxClothCollisionSphere(*(PxVec3*)(&vRightUpLeg), m_fCurRadius[RightUpLeg]));

			_v3 vRightLeg = *(_v3*)(&(m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[9] = (PxClothCollisionSphere(*(PxVec3*)(&vRightLeg), m_fCurRadius[RightLeg]));


			pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

			pCloth->addCollisionCapsule(0, 1);
			pCloth->addCollisionCapsule(2, 3);
			pCloth->addCollisionCapsule(4, 5);
			pCloth->addCollisionCapsule(6, 7);
			pCloth->addCollisionCapsule(8, 9);
		}

		vector<PxClothCollisionSphere> vecSpheres;
		vecSpheres.resize(10);

		// 충돌구 : 목, 엉덩이 
		pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

		vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[0].radius = m_fCurRadius[Neck];

		vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[1].radius = m_fCurRadius[Hips];

		vecSpheres[2].pos = *(PxVec3*)(&m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[2].radius = m_fCurRadius[LeftArm];

		vecSpheres[3].pos = *(PxVec3*)(&m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[3].radius = m_fCurRadius[LeftForeArm];

		vecSpheres[4].pos = *(PxVec3*)(&m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[4].radius = m_fCurRadius[RightArm];

		vecSpheres[5].pos = *(PxVec3*)(&m_pPlayerFrame[RightForeArm]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[5].radius = m_fCurRadius[RightForeArm];

		vecSpheres[6].pos = *(PxVec3*)(&m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[6].radius = m_fCurRadius[LeftUpLeg];

		vecSpheres[7].pos = *(PxVec3*)(&m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[7].radius = m_fCurRadius[LeftLeg];

		vecSpheres[8].pos = *(PxVec3*)(&m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[8].radius = m_fCurRadius[RightUpLeg];

		vecSpheres[9].pos = *(PxVec3*)(&m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[9].radius = m_fCurRadius[RightLeg];

		pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());
	}
	else
	{
		// 최초 업데이트 시 구와 캡슐 생성.
		if (0 == pCloth->getNbCollisionSpheres())
		{
			vector<PxClothCollisionSphere> vecSpheres;
			vecSpheres.resize(4);

			_v3 vNeck = *(_v3*)(&(m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3])) + vHairOffset;// *pTransform->Get_WorldMat()).m[3]);
			vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vNeck), m_fCurRadius[Neck]));

			_v3 vHip = *(_v3*)(&(m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3])) + vHairOffset;// * pTransform->Get_WorldMat()).m[3]);
			vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vHip), m_fCurRadius[Hips]));

			_v3 vLeftArm = *(_v3*)(&(m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[2] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftArm), m_fCurRadius[LeftArm]));

			_v3 vRightArm = *(_v3*)(&(m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3])) + vHairOffset;
			vecSpheres[3] = (PxClothCollisionSphere(*(PxVec3*)(&vRightArm), m_fCurRadius[RightArm]));

			pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

			pCloth->addCollisionCapsule(0, 1);
			pCloth->addCollisionCapsule(2, 3);
		}

		vector<PxClothCollisionSphere> vecSpheres;
		vecSpheres.resize(4);

		pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

		vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[0].radius = m_fCurRadius[Neck];

		vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[1].radius = m_fCurRadius[Hips];

		vecSpheres[2].pos = *(PxVec3*)(&m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[2].radius = m_fCurRadius[LeftArm];

		vecSpheres[3].pos = *(PxVec3*)(&m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3]) + PxVHairOffset;
		vecSpheres[3].radius = m_fCurRadius[RightArm];

		pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());
	}

}

void CClothManager::Update_Cloth_ColPos(physx::PxCloth * pCloth)
{
	// 드레이프 충돌 구
	if (pCloth == m_pCloth_Dynamic[Drape_01])
	{

	}

	// 건틀릿 충돌 구
	else if (pCloth == m_pCloth_Dynamic[Gauntlet_01] ||
		pCloth == m_pCloth_Dynamic[Gauntlet_03])
	{
		if (0 == pCloth->getNbCollisionSpheres())
		{
			vector<PxClothCollisionSphere> vecSpheres;
			vecSpheres.resize(3);

			_v3 vHip = *(_v3*)(&(m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]));
			vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vHip), m_fCurRadius[Hips]));

			_v3 vLeftForeArm = *(_v3*)(&(m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]));
			vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftForeArm), m_fCurRadius[LeftForeArm]));

			_v3 vLeftHand = *(_v3*)(&(m_pPlayerFrame[LeftHand]->CombinedTransformationMatrix.m[3]));
			vecSpheres[2] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftHand), m_fCurRadius[LeftHand]));

			pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

			pCloth->addCollisionCapsule(1, 2);
		}

		vector<PxClothCollisionSphere> vecSpheres;
		vecSpheres.resize(3);

		pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

		vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]);
		vecSpheres[0].radius = m_fCurRadius[Hips];

		vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]);
		vecSpheres[1].radius = m_fCurRadius[LeftForeArm];

		vecSpheres[2].pos = *(PxVec3*)(&m_pPlayerFrame[LeftHand]->CombinedTransformationMatrix.m[3]);
		vecSpheres[2].radius = m_fCurRadius[LeftHand];

		pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());
	}
	// 롱코트 충돌 구
	else if (pCloth == m_pCloth_Dynamic[LongCoat_01] ||
		pCloth == m_pCloth_Dynamic[LongCoat_03])
	{
		if (0 == pCloth->getNbCollisionSpheres())
		{
			vector<PxClothCollisionSphere> vecSpheres;
			vecSpheres.resize(6);

			_v3 vLeftUpLeg = *(_v3*)(&(m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftUpLeg), m_fCurRadius[LeftUpLeg]));

			_v3 vLeftLeg = *(_v3*)(&(m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftLeg), m_fCurRadius[LeftLeg]));

			_v3 vLeftFoot = *(_v3*)(&(m_pPlayerFrame[LeftFoot]->CombinedTransformationMatrix.m[3]));
			vecSpheres[2] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftFoot), m_fCurRadius[LeftFoot]));

			_v3 vRightUpLeg = *(_v3*)(&(m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[3] = (PxClothCollisionSphere(*(PxVec3*)(&vRightUpLeg), m_fCurRadius[RightUpLeg]));

			_v3 vRightLeg = *(_v3*)(&(m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[4] = (PxClothCollisionSphere(*(PxVec3*)(&vRightLeg), m_fCurRadius[RightLeg]));

			_v3 vRightFoot = *(_v3*)(&(m_pPlayerFrame[RightFoot]->CombinedTransformationMatrix.m[3]));
			vecSpheres[5] = (PxClothCollisionSphere(*(PxVec3*)(&vRightFoot), m_fCurRadius[RightFoot]));

			pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

			pCloth->addCollisionCapsule(0, 1);
			pCloth->addCollisionCapsule(1, 2);

			pCloth->addCollisionCapsule(3, 4);
			pCloth->addCollisionCapsule(4, 5);

		}

		vector<PxClothCollisionSphere> vecSpheres;
		vecSpheres.resize(6);

		pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

		vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[0].radius = m_fCurRadius[LeftUpLeg];

		vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[1].radius = m_fCurRadius[LeftLeg];

		vecSpheres[2].pos = *(PxVec3*)(&m_pPlayerFrame[LeftFoot]->CombinedTransformationMatrix.m[3]);
		vecSpheres[2].radius = m_fCurRadius[LeftFoot];

		vecSpheres[3].pos = *(PxVec3*)(&m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[3].radius = m_fCurRadius[RightUpLeg];

		vecSpheres[4].pos = *(PxVec3*)(&m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[4].radius = m_fCurRadius[RightLeg];

		vecSpheres[5].pos = *(PxVec3*)(&m_pPlayerFrame[RightFoot]->CombinedTransformationMatrix.m[3]);
		vecSpheres[5].radius = m_fCurRadius[RightFoot];

		pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());
	}
	// 머플러 충돌 구
	else
	{
		if (0 == pCloth->getNbCollisionSpheres())
		{
			vector<PxClothCollisionSphere> vecSpheres;
			vecSpheres.resize(11);

			_v3 vHip = *(_v3*)(&(m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]));
			vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vHip), m_fCurRadius[Hips]));

			_v3 vLeftForeArm = *(_v3*)(&(m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]));
			vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftForeArm), m_fCurRadius[LeftForeArm]));

			_v3 vLeftHand = *(_v3*)(&(m_pPlayerFrame[LeftHand]->CombinedTransformationMatrix.m[3]));
			vecSpheres[2] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftHand), m_fCurRadius[LeftHand]));

			_v3 vRightForeArm = *(_v3*)(&(m_pPlayerFrame[RightForeArm]->CombinedTransformationMatrix.m[3]));
			vecSpheres[3] = (PxClothCollisionSphere(*(PxVec3*)(&vRightForeArm), m_fCurRadius[RightForeArm]));

			_v3 vRightHand = *(_v3*)(&(m_pPlayerFrame[RightHand]->CombinedTransformationMatrix.m[3]));
			vecSpheres[4] = (PxClothCollisionSphere(*(PxVec3*)(&vRightHand), m_fCurRadius[RightHand]));

			_v3 vLeftUpLeg = *(_v3*)(&(m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[5] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftUpLeg), m_fCurRadius[LeftUpLeg]));

			_v3 vLeftLeg = *(_v3*)(&(m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[6] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftLeg), m_fCurRadius[LeftLeg]));

			_v3 vLeftFoot = *(_v3*)(&(m_pPlayerFrame[LeftFoot]->CombinedTransformationMatrix.m[3]));
			vecSpheres[7] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftFoot), m_fCurRadius[LeftFoot]));

			_v3 vRightUpLeg = *(_v3*)(&(m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[8] = (PxClothCollisionSphere(*(PxVec3*)(&vRightUpLeg), m_fCurRadius[RightUpLeg]));

			_v3 vRightLeg = *(_v3*)(&(m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]));
			vecSpheres[9] = (PxClothCollisionSphere(*(PxVec3*)(&vRightLeg), m_fCurRadius[RightLeg]));

			_v3 vRightFoot = *(_v3*)(&(m_pPlayerFrame[RightFoot]->CombinedTransformationMatrix.m[3]));
			vecSpheres[10] = (PxClothCollisionSphere(*(PxVec3*)(&vRightFoot), m_fCurRadius[RightFoot]));

			pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

			pCloth->addCollisionCapsule(1, 2);
			pCloth->addCollisionCapsule(3, 4);

			pCloth->addCollisionCapsule(5, 6);
			pCloth->addCollisionCapsule(6, 7);

			pCloth->addCollisionCapsule(8, 9);
			pCloth->addCollisionCapsule(9, 10);
		}

		vector<PxClothCollisionSphere> vecSpheres;
		vecSpheres.resize(11);

		pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

		vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]);
		vecSpheres[0].radius = m_fCurRadius[Hips];

		vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3]);
		vecSpheres[1].radius = m_fCurRadius[LeftArm];

		vecSpheres[2].pos = *(PxVec3*)(&m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]);
		vecSpheres[2].radius = m_fCurRadius[LeftForeArm];

		vecSpheres[3].pos = *(PxVec3*)(&m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3]);
		vecSpheres[3].radius = m_fCurRadius[RightArm];

		vecSpheres[4].pos = *(PxVec3*)(&m_pPlayerFrame[RightForeArm]->CombinedTransformationMatrix.m[3]);
		vecSpheres[4].radius = m_fCurRadius[RightForeArm];

		vecSpheres[5].pos = *(PxVec3*)(&m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[5].radius = m_fCurRadius[LeftUpLeg];

		vecSpheres[6].pos = *(PxVec3*)(&m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[6].radius = m_fCurRadius[LeftLeg];

		vecSpheres[7].pos = *(PxVec3*)(&m_pPlayerFrame[LeftFoot]->CombinedTransformationMatrix.m[3]);
		vecSpheres[7].radius = m_fCurRadius[LeftFoot];

		vecSpheres[8].pos = *(PxVec3*)(&m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[8].radius = m_fCurRadius[RightUpLeg];

		vecSpheres[9].pos = *(PxVec3*)(&m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]);
		vecSpheres[9].radius = m_fCurRadius[RightLeg];

		vecSpheres[10].pos = *(PxVec3*)(&m_pPlayerFrame[RightFoot]->CombinedTransformationMatrix.m[3]);
		vecSpheres[10].radius = m_fCurRadius[RightFoot];

		pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());
	}

	//// 최초 업데이트 시 원과 캡슐 생성.
	//if (0 == pCloth->getNbCollisionSpheres())
	//{
	//	vector<PxClothCollisionSphere> vecSpheres;
	//	vecSpheres.resize(14);

	//	_v3 vNeck = *(_v3*)(&(m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[0] = (PxClothCollisionSphere(*(PxVec3*)(&vNeck), m_fCurRadius[Neck]));

	//	_v3 vHip = *(_v3*)(&(m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[1] = (PxClothCollisionSphere(*(PxVec3*)(&vHip), m_fCurRadius[Hips]));

	//	_v3 vLeftArm = *(_v3*)(&(m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[2] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftArm), m_fCurRadius[LeftArm]));

	//	_v3 vLeftForeArm = *(_v3*)(&(m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[3] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftForeArm), m_fCurRadius[LeftForeArm]));

	//	_v3 vLeftHand = *(_v3*)(&(m_pPlayerFrame[LeftHand]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[4] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftHand), m_fCurRadius[LeftHand]));

	//	_v3 vRightArm = *(_v3*)(&(m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[5] = (PxClothCollisionSphere(*(PxVec3*)(&vRightArm), m_fCurRadius[RightArm]));

	//	_v3 vRightForeArm = *(_v3*)(&(m_pPlayerFrame[RightForeArm]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[6] = (PxClothCollisionSphere(*(PxVec3*)(&vRightForeArm), m_fCurRadius[RightForeArm]));

	//	_v3 vRightHand = *(_v3*)(&(m_pPlayerFrame[RightHand]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[7] = (PxClothCollisionSphere(*(PxVec3*)(&vRightHand), m_fCurRadius[RightHand]));

	//	_v3 vLeftUpLeg = *(_v3*)(&(m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[8] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftUpLeg), m_fCurRadius[LeftUpLeg]));

	//	_v3 vLeftLeg = *(_v3*)(&(m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[9] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftLeg), m_fCurRadius[LeftLeg]));

	//	_v3 vLeftFoot = *(_v3*)(&(m_pPlayerFrame[LeftFoot]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[10] = (PxClothCollisionSphere(*(PxVec3*)(&vLeftFoot), m_fCurRadius[LeftFoot]));

	//	_v3 vRightUpLeg = *(_v3*)(&(m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[11] = (PxClothCollisionSphere(*(PxVec3*)(&vRightUpLeg), m_fCurRadius[RightUpLeg]));

	//	_v3 vRightLeg = *(_v3*)(&(m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[12] = (PxClothCollisionSphere(*(PxVec3*)(&vRightLeg), m_fCurRadius[RightLeg]));

	//	_v3 vRightFoot = *(_v3*)(&(m_pPlayerFrame[RightFoot]->CombinedTransformationMatrix.m[3]));
	//	vecSpheres[13] = (PxClothCollisionSphere(*(PxVec3*)(&vRightFoot), m_fCurRadius[RightFoot]));

	//	pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());

	//	pCloth->addCollisionCapsule(0, 1);

	//	pCloth->addCollisionCapsule(2, 3);
	//	pCloth->addCollisionCapsule(3, 4);

	//	pCloth->addCollisionCapsule(5, 6);
	//	pCloth->addCollisionCapsule(6, 7);

	//	pCloth->addCollisionCapsule(8, 9);
	//	pCloth->addCollisionCapsule(9, 10);

	//	pCloth->addCollisionCapsule(11, 12);
	//	pCloth->addCollisionCapsule(12, 13);
	//}

	//vector<PxClothCollisionSphere> vecSpheres;
	//vecSpheres.resize(14);

	//pCloth->getCollisionData(&vecSpheres.front(), 0, 0, 0, 0);

	//vecSpheres[0].pos = *(PxVec3*)(&m_pPlayerFrame[Neck]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[0].radius = m_fCurRadius[Neck];

	//vecSpheres[1].pos = *(PxVec3*)(&m_pPlayerFrame[Hips]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[1].radius = m_fCurRadius[Hips];

	//vecSpheres[2].pos = *(PxVec3*)(&m_pPlayerFrame[LeftArm]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[2].radius = m_fCurRadius[LeftArm];

	//vecSpheres[3].pos = *(PxVec3*)(&m_pPlayerFrame[LeftForeArm]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[3].radius = m_fCurRadius[LeftForeArm];

	//vecSpheres[4].pos = *(PxVec3*)(&m_pPlayerFrame[LeftHand]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[4].radius = m_fCurRadius[LeftHand];

	//vecSpheres[5].pos = *(PxVec3*)(&m_pPlayerFrame[RightArm]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[5].radius = m_fCurRadius[RightArm];

	//vecSpheres[6].pos = *(PxVec3*)(&m_pPlayerFrame[RightForeArm]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[6].radius = m_fCurRadius[RightForeArm];

	//vecSpheres[7].pos = *(PxVec3*)(&m_pPlayerFrame[RightHand]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[7].radius = m_fCurRadius[RightHand];

	//vecSpheres[8].pos = *(PxVec3*)(&m_pPlayerFrame[LeftUpLeg]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[8].radius = m_fCurRadius[LeftUpLeg];

	//vecSpheres[9].pos = *(PxVec3*)(&m_pPlayerFrame[LeftLeg]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[9].radius = m_fCurRadius[LeftLeg];

	//vecSpheres[10].pos = *(PxVec3*)(&m_pPlayerFrame[LeftFoot]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[10].radius = m_fCurRadius[LeftFoot];

	//vecSpheres[11].pos = *(PxVec3*)(&m_pPlayerFrame[RightUpLeg]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[11].radius = m_fCurRadius[RightUpLeg];

	//vecSpheres[12].pos = *(PxVec3*)(&m_pPlayerFrame[RightLeg]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[12].radius = m_fCurRadius[RightLeg];

	//vecSpheres[13].pos = *(PxVec3*)(&m_pPlayerFrame[RightFoot]->CombinedTransformationMatrix.m[3]);
	//vecSpheres[13].radius = m_fCurRadius[RightFoot];

	//pCloth->setCollisionSpheres(&vecSpheres.front(), (PxU32)vecSpheres.size());


}

void CClothManager::Update_CurRadius()
{
	for (_uint i = 0; i < PlayerBone_End; ++i)
	{
		m_fCurRadius[i] += m_fMaxRadius[i] * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

		if (m_fCurRadius[i] > m_fMaxRadius[i])
			m_fCurRadius[i] = m_fMaxRadius[i];
	}
}

void CClothManager::Reset_CurRadius()
{
	for (_uint i = 0; i < PlayerBone_End; ++i)
		m_fCurRadius[i] = 0.f;
}

void CClothManager::Init_Hair01(physx::PxClothMeshDesc& pClothMeshDesc, vector<PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.04f &&
			vertices[i].z < -0.04f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Static[Hair_01] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_01]);

	Set_Cloth_Default(m_pCloth_Static[Hair_01]);

	m_pCloth_Static[Hair_01]->putToSleep();
}

void CClothManager::Init_Hair02(physx::PxClothMeshDesc& pClothMeshDesc, vector<PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.02f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Static[Hair_02] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_02]);

	Set_Cloth_Default(m_pCloth_Static[Hair_02]);

	m_pCloth_Static[Hair_02]->putToSleep();
}

void CClothManager::Init_Hair03(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.04f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair_03] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_03]);

	Set_Cloth_Default(m_pCloth_Static[Hair_03]);

	m_pCloth_Static[Hair_03]->putToSleep();
}

void CClothManager::Init_Hair04(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.04f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair_04] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_04]);

	Set_Cloth_Default(m_pCloth_Static[Hair_04]);

	m_pCloth_Static[Hair_04]->putToSleep();
}

void CClothManager::Init_Hair05(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.1f &&
			vertices[i].z < -0.07)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair_05] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_05]);

	Set_Cloth_Default(m_pCloth_Static[Hair_05]);

	m_pCloth_Static[Hair_05]->putToSleep();
}

void CClothManager::Init_Hair06(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.02f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair_06] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_06]);

	Set_Cloth_Default(m_pCloth_Static[Hair_06]);

	m_pCloth_Static[Hair_06]->putToSleep();
}

void CClothManager::Init_Hair07(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom)
{
	for (PxU32 i = 0; i < pStaticMeshCom->Get_pMesh()->GetNumVertices(); i++)
	{
		if (vertices[i].y < 0.03f)
			vertices[i].w = 0.2f;
	}


	m_pCloth_Static[Hair_07] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Static[Hair_07]);

	Set_Cloth_Default(m_pCloth_Static[Hair_07]);

	m_pCloth_Static[Hair_07]->putToSleep();
}

void CClothManager::Init_Drape_01(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 9226; i++)
	{
		if (vertices[i].y < 1.0f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[Drape_01] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[Drape_01]);

	Set_Cloth_Default(m_pCloth_Dynamic[Drape_01]);

	m_pCloth_Dynamic[Drape_01]->putToSleep();
}

void CClothManager::Init_Gauntlet_01(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 13432; i++)
	{
		if (vertices[i].y < 1.2f &&
			vertices[i].x < 0.f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[Gauntlet_01] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[Gauntlet_01]);

	Set_Cloth_Default(m_pCloth_Dynamic[Gauntlet_01]);

	m_pCloth_Dynamic[Gauntlet_01]->putToSleep();
}

void CClothManager::Init_Gauntlet_03(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 906; i++)
	{
		if (vertices[i].y < 1.0f)
			vertices[i].w = 0.2f;
	}
	for (PxU32 i = 15285; i < 15285 + 906; i++)
	{
		if (vertices[i].y < 1.0f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[Gauntlet_03] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[Gauntlet_03]);

	Set_Cloth_Default(m_pCloth_Dynamic[Gauntlet_03]);

	m_pCloth_Dynamic[Gauntlet_03]->putToSleep();
}

void CClothManager::Init_LongCoat_01(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 11855; i < 16267 + 1384; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[LongCoat_01] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[LongCoat_01]);

	Set_Cloth_Default(m_pCloth_Dynamic[LongCoat_01]);

	m_pCloth_Dynamic[LongCoat_01]->putToSleep();
}

void CClothManager::Init_LongCoat_02(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 316; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	for (PxU32 i = 14203; i < 14203 + 825; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	for (PxU32 i = 15028; i < 15028 + 316; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	for (PxU32 i = 15344; i < 15344 + 825; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[LongCoat_02] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[LongCoat_02]);

	Set_Cloth_Default(m_pCloth_Dynamic[LongCoat_02]);

	m_pCloth_Dynamic[LongCoat_02]->putToSleep();
}

void CClothManager::Init_LongCoat_03(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 939; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	for (PxU32 i = 939; i < 939 + 3459; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	for (PxU32 i = 17444; i < 17444 + 3459; i++)
	{
		if (vertices[i].y < 0.7f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[LongCoat_03] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[LongCoat_03]);

	Set_Cloth_Default(m_pCloth_Dynamic[LongCoat_03]);

	m_pCloth_Dynamic[LongCoat_03]->putToSleep();
}

void CClothManager::Init_Muffler_01(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 14402; i++)
	{
		if (vertices[i].y < 1.1f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[Muffler_01] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[Muffler_01]);

	Set_Cloth_Default(m_pCloth_Dynamic[Muffler_01]);

	m_pCloth_Dynamic[Muffler_01]->putToSleep();
}

void CClothManager::Init_Muffler_02(physx::PxClothMeshDesc & pClothMeshDesc, vector<physx::PxVec4>& vertices)
{
	for (PxU32 i = 0; i < 2616; i++)
	{
		if (vertices[i].y < 1.1f)
			vertices[i].w = 0.2f;
	}

	m_pCloth_Dynamic[Muffler_02] = CreateQuadifier(pClothMeshDesc);

	PxScene& scene = *g_pPhysx->Get_Scene();

	scene.addActor(*m_pCloth_Dynamic[Muffler_02]);

	Set_Cloth_Default(m_pCloth_Dynamic[Muffler_02]);

	m_pCloth_Dynamic[Muffler_02]->putToSleep();
}

void CClothManager::Free()
{
	// 옷은 피직스가 내부에서 알아서 제거해줄 거임.
}

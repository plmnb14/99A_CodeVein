#include "stdafx.h"
#include "ClothStatic.h"

using namespace physx;

CClothStatic::CClothStatic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCloth(pGraphic_Device)
{
}

CClothStatic::CClothStatic(const CClothStatic & rhs)
	: CCloth(rhs)
{
}

HRESULT CClothStatic::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CClothStatic::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	PxSceneWriteLock scopedLock(scene);

	// create the cloth cape mesh from file
	vector<PxVec4> vertices;
	vector<PxU16> primitives;
	PxClothMeshDesc meshDesc = CreateMesh(1.f, PxQuat(PxIdentity), PxVec3(0, 0, 0), vertices, primitives);

	// 무슨일을 하는지 의미를 모르겠음.
	//for (PxU32 i = 0; i < meshDesc.invMasses.count; i++)
	//{
	//	((float*)meshDesc.invMasses.data)[i * 4] = 0.5f;
	//}

	if (!meshDesc.isValid())
		MSG_BOX(" PxCloth : Failed to Craete Mesh");

	////////////////////////////////////////
	// 메쉬별 흔들릴 곳 정함.
	Set_Static_Vertex_W(pArg, vertices);
	/////////////////////////////////////////


	PxClothMeshQuadifier quadifier(meshDesc);

	// Create Fabric
	PxClothFabric* clothFabric = PxClothFabricCreate(physics, quadifier.getDescriptor(), PxVec3(0, -1, 0));
	PX_ASSERT(quadifier.getDescriptor().points.stride == sizeof(PxVec4));

	// create the cloth
	const PxClothParticle* particles = (const PxClothParticle*)quadifier.getDescriptor().points.data;
	m_pCloth = physics.createCloth(PxTransform(PxVec3(0.f, 0.f, 0.f), PxQuat(PxIdentity)), *clothFabric, particles, PxClothFlags());
	PX_ASSERT(m_pCloth);

	scene.addActor(*m_pCloth);

	CClothStatic::INFO Temp = *(CClothStatic::INFO*)pArg;

	switch (Temp.eName)
	{
	case CClothStatic::Hair_Long:
		Init_HairLong();
		break;
	case CClothStatic::Hair_Ponytail:
		Init_HairPonyTail();
		break;
	}



	m_pTransformCom->Set_Pos(_v3(0.f, 0.f, 0.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	return S_OK;
}

_int CClothStatic::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	Set_Wind();

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	_v3 vPlayerPos = TARGET_TO_TRANS(pPlayer)->Get_Pos();

	CMesh_Dynamic* pPlayerMesh = TARGET_TO_D_MESH(pPlayer);

	PxTransform rootPose = PxTransform(PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z));

	// physx 옷에 좌표 전달.
	m_pCloth->setTargetPose(rootPose);

	// d3d에서 쓸  physx에서 계산된 좌표.
	m_pTransformCom->Set_Pos(_v3(m_pCloth->getGlobalPose().p.x, m_pCloth->getGlobalPose().p.y, m_pCloth->getGlobalPose().p.z));

	return NO_EVENT;
}

_int CClothStatic::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	//=============================================================================================
	// 그림자랑 모션블러는 프리스텀 안에 없으면 안그림
	//=============================================================================================
	if (m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f))
	{
		if (!m_bDissolve)
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
				return E_FAIL;
		}

		else
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
				return E_FAIL;
		}

		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}
	//=============================================================================================

	return NO_EVENT;
}

HRESULT CClothStatic::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	if (m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f))
	{
		// 버텍스 교체
		PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

		PxClothFabric* pFabric = m_pCloth->getFabric();
		PxClothParticleData* pData = m_pCloth->lockParticleData();

		_ulong dwStride = m_pMesh->GetNumBytesPerVertex();

		_byte* pVertices = nullptr;

		m_pMesh->LockVertexBuffer(0, (void**)&pVertices);

		for (_ulong i = 0; i < _ulong(pFabric->getNbParticles()); ++i)
		{
			*(_v3*)(pVertices + (i * dwStride)) = *(_v3*)(pData->particles + i);
		}

		m_pMesh->UnlockVertexBuffer();
		/////////////////////////////////////////////////

		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		_uint iNumSubSet = (_uint)m_pMesh_Static->Get_NumMaterials();

		for (_uint i = 0; i < iNumSubSet; ++i)
		{
			m_iPass = m_pMesh_Static->Get_MaterialPass(i);

			if (m_bDissolve)
				m_iPass = 3;

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_StaticTexture_Auto(m_pMesh_Static, i);

			m_pShaderCom->Commit_Changes();

			m_pMesh_Static->Render_Mesh(i);

			m_pShaderCom->End_Pass();
		}

		m_pShaderCom->End_Shader();
	}

	return NOERROR;
}

HRESULT CClothStatic::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================


	return NOERROR;
}

void CClothStatic::Init_HairLong()
{
	// set solver settings
	m_pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

	// reduce impact of frame acceleration
	// x, z: cloth swings out less when walking in a circle
	// y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.2f, 0.3f, 0.2f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.2f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

void CClothStatic::Init_HairPonyTail()
{
	// set solver settings
	m_pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

	// reduce impact of frame acceleration
	// x, z: cloth swings out less when walking in a circle
	// y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.4f, 0.3f, 0.4f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.3f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

HRESULT CClothStatic::Add_Component(void * pArg)
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	_tchar name[STR_128] = { 0, };

	if (nullptr == pArg)
		lstrcpy(name, L"Mesh_Hair_Long");
	else
	{
		CClothStatic::INFO Temp = *(CClothStatic::INFO*)pArg;

		switch (Temp.eName)
		{
		case ClothStatic_Name::Hair_Long:
			lstrcpy(name, L"Mesh_Hair_Long");
			break;
		case ClothStatic_Name::Hair_Ponytail:
			lstrcpy(name, L"Mesh_Hair_Ponytail");
			break;
		}

	}

	// enum에 따른 메쉬교체 해야함.
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, name, L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	m_pMesh = m_pMesh_Static->Get_pMesh();

	// for.Com_Optimaization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CClothStatic::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 3.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 5.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 0.5f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fRimLightPower = 0.f;	// 림		: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).

	if (FAILED(m_pShaderCom->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================



	return S_OK;
}

CClothStatic * CClothStatic::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CClothStatic* pInstance = new CClothStatic(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CClothStatic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CClothStatic::Clone_GameObject(void * pArg)
{
	CClothStatic* pInstance = new CClothStatic(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CClothStatic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClothStatic::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pOptimizationCom);

	CCloth::Free();
}

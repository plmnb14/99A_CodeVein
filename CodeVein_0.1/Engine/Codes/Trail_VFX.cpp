#include "Trail_VFX.h"
#include "Management.h"

CTrail_VFX::CTrail_VFX(_Device pGraphicDev)
	:CGameObject(pGraphicDev)
	, m_dwInterval(0)
	, m_dwListCnt(0)
	, m_dwMaxCnt(0)
	, m_fUpdateTime_Cur(0.f)
	, m_fUpdateTime_Max(0.00001f)
	, m_bUpdateVtx(true)
{
}

CTrail_VFX::CTrail_VFX(const CTrail_VFX & rhs)
	: CGameObject(rhs)
	, m_dwInterval(rhs.m_dwInterval)
	, m_dwListCnt(rhs.m_dwListCnt)
	, m_dwMaxCnt(rhs.m_dwMaxCnt)
	, m_fUpdateTime_Cur(rhs.m_fUpdateTime_Cur)
	, m_fUpdateTime_Max(rhs.m_fUpdateTime_Max)
	, m_bUpdateVtx(rhs.m_bUpdateVtx)
{
}

CTrail_VFX::~CTrail_VFX()
{
}

HRESULT CTrail_VFX::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CTrail_VFX::Ready_GameObject(void * pArg)
{
	Add_Component();

	m_dwVtxFVF = VTXFVF_TEX;
	m_dwVtxSize = sizeof(VTXTEX);

	m_dwInterval = 5;
	m_dwListCnt = 5;

	m_dwMaxCnt = 20;

	return S_OK;
}

_int CTrail_VFX::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	_v3 TmpPos;

	memcpy(TmpPos, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));

	Compute_ViewZ(&TmpPos);

	CatmullRom_Pos();
	Ready_Vtx();

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(TimeDelta);

	switch (m_eType)
	{
	case Trail_Normal:
	{
		m_pRenderer->Add_RenderList(RENDER_ALPHA_TRAIL, this);
		//m_pRenderer->Add_RenderList(RENDER_ALPHA, this);

		break;
	}
	case Trail_Distortion:
	{
		m_pRenderer->Add_RenderList(RENDER_DISTORTION, this);
		break;
	}
	}

	return NO_EVENT;
}

_int CTrail_VFX::Late_Update_GameObject(_double TimeDelta)
{
	return NO_EVENT;
}

HRESULT CTrail_VFX::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CTrail_VFX::Render_GameObject()
{
	m_pGraphic_Dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Dev->SetFVF(m_dwVtxFVF);

	_int iPass = (m_eType == Trail_Normal ? 5 : 1);

	if (m_bUseMask)
		iPass = 6;

	CManagement* pManagement = CManagement::Get_Instance();

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);
	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat));
	m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat));

	Shader_Init(m_pShader, m_iTrailIdx);

	m_pShader->Begin_Shader();
	m_pShader->Begin_Pass(iPass);

	m_pGraphic_Dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_dwTricnt, m_pVtx, sizeof(VTXTEX));

	m_pShader->End_Pass();
	m_pShader->End_Shader();

	m_vecStart.clear();
	m_vecStart.shrink_to_fit();

	m_vecEnd.clear();
	m_vecStart.shrink_to_fit();

	pManagement = nullptr;

	return S_OK;
}

HRESULT CTrail_VFX::Render_GameObject_SetShader(CShader * pShader)
{
	//m_pGraphic_Dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//
	//m_pGraphic_Dev->SetFVF(m_dwVtxFVF);

	_int iPass = (m_eType == Trail_Normal ? 5 : 1);
	if (m_bUseMask) iPass = 6;

	Shader_Init(pShader, m_iTrailIdx);

	pShader->Begin_Pass(iPass);

	pShader->Commit_Changes();

	m_pGraphic_Dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_dwTricnt, m_pVtx, sizeof(VTXTEX));

	pShader->End_Pass();

	m_vecStart.clear();
	m_vecStart.shrink_to_fit();

	m_vecEnd.clear();
	m_vecStart.shrink_to_fit();

	return S_OK;
}

HRESULT CTrail_VFX::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	//m_pGraphic_Dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//
	//m_pGraphic_Dev->SetFVF(m_dwVtxFVF);

	_int iPass = (m_eType == Trail_Normal ? 5 : 1);

	if (m_bUseMask)
		iPass = 6;

	Shader_Init(pShader, m_iTrailIdx);

	pShader->Begin_Pass(iPass);

	pShader->Commit_Changes();

	m_pGraphic_Dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_dwTricnt, m_pVtx, sizeof(VTXTEX));

	pShader->End_Pass();

	m_vecStart.clear();
	m_vecStart.shrink_to_fit();

	m_vecEnd.clear();
	m_vecStart.shrink_to_fit();

	return S_OK;
}

void CTrail_VFX::Change_Tex(const _tchar * _TexName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_eTexName, _TexName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_eTexName, _TexName);

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Mesh");

	// 둘 다 해제
	Safe_Release(m_pTexture);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pTexture = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_eTexName));
	Safe_AddRef(iter->second);
}

void CTrail_VFX::Ready_Info(_v3 _vBeginPos, _v3 _vEndPos)
{
	m_bDrawTrail = true;

	//저장 갯수보다 많으면 먼저 생성된것부터 삭제
	while (m_listTrail.size() > 10)
	{
		m_listTrail.pop_front();
	}

	TRAIL_INFO tmpInfo;

	tmpInfo.m_vBeginPos = _vBeginPos;
	tmpInfo.m_vEndPos = _vEndPos;

	m_listTrail.push_back(tmpInfo);
}

void CTrail_VFX::CatmullRom_Pos()
{
	// 그리는 최소 갯수보다 적으면 리턴
	if (m_listTrail.size() < 4)
		return;

	m_vecStart.reserve(15 * 10);
	m_vecEnd.reserve(15 * 10);

	_int	iCnt = 0;

	auto& iter_Pos = m_listTrail.begin();
	auto& iter_End = m_listTrail.end();

	for (auto& iter = m_listTrail.begin(); iter_Pos != iter_End; ++iter)
	{
		_v3 vBeginPos0 = iter_Pos->m_vBeginPos;
		_v3 vEndPos0 = iter_Pos->m_vEndPos;
		++iter_Pos;
		if (iter_Pos == iter_End)
			break;

		_v3 vBeginPos1 = iter_Pos->m_vBeginPos;
		_v3 vEndPos1 = iter_Pos->m_vEndPos;
		++iter_Pos;
		if (iter_Pos == iter_End)
			break;

		_v3 vBeginPos2 = iter_Pos->m_vBeginPos;
		_v3 vEndPos2 = iter_Pos->m_vEndPos;
		++iter_Pos;
		if (iter_Pos == iter_End)
			break;

		_v3 vBeginPos3 = iter_Pos->m_vBeginPos;
		_v3 vEndPos3 = iter_Pos->m_vEndPos;
		iter_Pos = iter;

		LOOP(10)
		{
			_v3	vBegin, vEnd;

			D3DXVec3CatmullRom(&vBegin, &vBeginPos0, &vBeginPos1, &vBeginPos2, &vBeginPos3, 0.1f * i);
			m_vecStart.push_back(vBegin);

			D3DXVec3CatmullRom(&vEnd, &vEndPos0, &vEndPos1, &vEndPos2, &vEndPos3, 0.1f * i);
			m_vecEnd.push_back(vEnd);
		};
	}

	Ready_Vtx();
}

void CTrail_VFX::Ready_Vtx()
{
	if (m_listTrail.size() < 4)
		return;

	m_dwVtxCnt = 40;

	for (size_t i = 0; i < m_vecStart.size(); ++i)
	{
		//if (300 >= i * 2 ||
		//	300 >= i * 2 + 1)
		//	break;

		//m_pVtx[i * 2].vPosition = m_vecStart[i];
		//m_pVtx[i * 2].vTexUV.x = 0.f + ((_float(i)) / (m_vecStart.size() - 1));
		//m_pVtx[i * 2].vTexUV.y = 1.f;

		//m_pVtx[i * 2 + 1].vPosition = m_vecEnd[i];
		//m_pVtx[i * 2 + 1].vTexUV.x = 0.f + ((_float(i)) / (m_vecStart.size() - 1));
		//m_pVtx[i * 2 + 1].vTexUV.y = 0.f;

		m_pVtx[i * 2].vPosition = m_vecStart[i];
		m_pVtx[i * 2].vTexUV.x = 1.f - ((_float(i)) / (m_vecStart.size() - 1));
		m_pVtx[i * 2].vTexUV.y = 0.f;

		m_pVtx[i * 2 + 1].vPosition = m_vecEnd[i];
		m_pVtx[i * 2 + 1].vTexUV.x = 1.f - ((_float(i)) / (m_vecStart.size() - 1));
		m_pVtx[i * 2 + 1].vTexUV.y = 1.f;
	}

	m_dwTricnt = _ulong((m_vecStart.size() + m_vecEnd.size()) - 2);
}

void CTrail_VFX::Set_ParentTransform(const _mat * _pWorldMat)
{
	m_matParent = *_pWorldMat;

	m_pTransform->Calc_ParentMat(&(*_pWorldMat));
}

void CTrail_VFX::Reset_Info()
{
	m_bIsDead = false;

	m_dwVtxFVF = VTXFVF_TEX;
	m_dwVtxSize = sizeof(VTXTEX);

	m_dwInterval = 5;
	m_dwListCnt = 5;
	m_dwMaxCnt = 20;

	m_iTrailIdx		= 0;
	m_iTrailMaskIdx = 0;
	m_dwVtxCnt		= 0;
	m_dwVtxFVF		= 0;
	m_dwVtxSize		= 0;
	m_dwTricnt		= 0;
	m_bUpdateVtx	= false;
	m_bDrawTrail	= true;
	m_bUseMask		= false;
	m_fUpdateTime_Cur = 0.f;
	m_fUpdateTime_Max = 0.f;

	//Clear_TrailInfo();
}

HRESULT CTrail_VFX::Shader_Init(CShader* pShader, const _uint & iIndex)
{
	if (nullptr == pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat));
	//pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat));
	//pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat));

	_float	fAlpha = 1.f;
	_float	fDistortion = 0.01f;
	pShader->Set_Value("g_fAlpha", &fAlpha, sizeof(_float));
	pShader->Set_Value("g_fDistortion", &fDistortion, sizeof(_float));

	_v4		vDefaultOption = { 0.f, 0.f, 0.f, 1.f };
	pShader->Set_Value("g_vColor", &vDefaultOption, sizeof(_float));

	_bool	bDefaultOption = false;
	pShader->Set_Bool("g_bUseRGBA", bDefaultOption);
	pShader->Set_Bool("g_bUseColorTex", bDefaultOption);
	pShader->Set_Bool("g_bUseMaskTex", bDefaultOption);
	pShader->Set_Bool("g_bReverseColor", bDefaultOption);
	pShader->Set_Bool("g_bDissolve", bDefaultOption);

	m_pTexture->SetUp_OnShader("g_DiffuseTexture", pShader, iIndex);
	m_pMaskTexture->SetUp_OnShader("g_GradientTexture", pShader, m_iTrailMaskIdx);
	pShader->Set_Texture("g_DepthTexture", pManagement->Get_Target_Texture(L"Target_Depth"));

	//m_pTexture->SetUp_OnShader("g_DiffuseTexture_2", pShader, iIndex);
	//m_pTexture->SetUp_OnShader("g_DistortionTexture", pShader, iIndex);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CTrail_VFX::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Effect", L"Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// For.Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_SwordTrail", L"Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.MaskTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_SwordTrail", L"MaskTexture", (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	m_pTransform->Set_Pos({ 0, 0, 0 });
	m_pTransform->Set_Scale({ 1, 1, 1 });

	return S_OK;
}

CTrail_VFX * CTrail_VFX::Create(_Device pGraphicDev)
{
	CTrail_VFX* pInstance = new CTrail_VFX(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrail_VFX::Clone_GameObject(void * pArg)
{
	CTrail_VFX* pInstance = new CTrail_VFX(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_VFX::Free()
{
	CGameObject::Free();

	m_vecStart.shrink_to_fit();
	m_vecStart.clear();

	m_vecEnd.shrink_to_fit();
	m_vecEnd.clear();

	m_listTrail.clear();

	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
}

void CTrail_VFX::Clear_TrailInfo()
{
	//ENGINE::Safe_Delete_Array(m_pVtx);

	ZeroMemory(&m_pVtx, sizeof(VTXTEX) * 300);

	m_vecStart.shrink_to_fit();
	m_vecStart.clear();

	m_vecEnd.shrink_to_fit();
	m_vecEnd.clear();

	m_listTrail.clear();
}

void CTrail_VFX::Clear_TrailInfo_From_Front()
{
}
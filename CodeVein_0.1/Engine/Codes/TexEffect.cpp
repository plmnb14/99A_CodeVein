#include "..\Headers\TexEffect.h"

CTexEffect::CTexEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{

}

CTexEffect::CTexEffect(const CTexEffect& rhs)
	: CEffect(rhs)
{
	CEffect::m_pInfo = rhs.m_pInfo;

	m_bClone = true;
}

INSTANCEDATA CTexEffect::Get_InstanceData()
{
	INSTANCEDATA tData;

	memcpy(&tData.fRight, &m_pTransformCom->Get_WorldMat()._11, sizeof(_float) * 4);
	memcpy(&tData.fUp, &m_pTransformCom->Get_WorldMat()._21, sizeof(_float) * 4);
	memcpy(&tData.fLook, &m_pTransformCom->Get_WorldMat()._31, sizeof(_float) * 4);
	memcpy(&tData.fPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_float) * 4);
	memcpy(&tData.fColor, &m_vColor, sizeof(_float) * 4);
	tData.fDissolve = m_fDissolve;
	tData.fDistortion = m_pInfo->fDistortionPower;
	tData.fAlpha = m_fAlpha;
	tData.bDissolve = m_pInfo->bDissolve;
	tData.bReverseColor = m_pInfo->bRevColor;
	tData.bUseColorTex = m_pInfo->bUseColorTex;
	tData.bUseMaskTex = (m_pInfo->fMaskIndex != -1.f);
	tData.bUseRGBA = m_pInfo->bUseRGBA;

	return tData;
}

HRESULT CTexEffect::SetUp_ConstantTable_Instance(CShader* pShader)
{
	_float fMaskIndex = 0.f;
	if ((m_pInfo->fMaskIndex != -1.f))
		fMaskIndex = m_pInfo->fMaskIndex;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", pShader, _uint(m_fFrame))))
		return E_FAIL;
	if (FAILED(m_pGradientTextureCom->SetUp_OnShader("g_GradientTexture", pShader, _uint(fMaskIndex))))
		return E_FAIL;
	if (FAILED(m_pColorTextureCom->SetUp_OnShader("g_ColorTexture", pShader, _uint(m_pInfo->fColorIndex))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexEffect::Ready_GameObject_Prototype()
{
	// 생성 시, 오래 걸릴 수 있는 작업들을 수행한다.

	if (!m_pInfo)
	{
		m_pInfo = new EFFECT_INFO;
		ZeroMemory(m_pInfo, sizeof(EFFECT_INFO));
	}
	return NOERROR;
}

HRESULT CTexEffect::Ready_GameObject(void* pArg)
{
	// 복제해서 생성 된 후, 추가적으로 필요한 데이터들을 셋팅하낟.
	if (FAILED(Add_Component()))
		return E_FAIL;

	// Set Default Info
	m_pTransformCom->Set_Pos(V3_NULL);
	m_pTransformCom->Set_Scale(V3_ONE);
	m_pTransformCom->Set_Angle(V3_NULL);

	if (pArg)
		m_pDesc = (EFFECT_DESC*)pArg;
	else
	{
		m_pDesc = new EFFECT_DESC;
		m_pDesc->vWorldPos = { 0.f , 0.f , 0.f };
		m_pDesc->pTargetTrans = nullptr;
	}

	m_pManagement = CManagement::Get_Instance();
	if (nullptr == m_pManagement)
		return E_FAIL;

	//Safe_AddRef(m_pManagement);

	return NOERROR;
}

HRESULT CTexEffect::LateInit_GameObject()
{
	Setup_Info();
	Change_EffectTexture(m_pInfo->szName);
	Change_GradientTexture(m_pInfo->szGradientName);
	Change_ColorTexture(m_pInfo->szColorName);

	// Check Instance Target
	if (!lstrcmp(L"QueensKnight_Teleport_Particle", m_szParticleName) ||
		!lstrcmp(L"QueensKnight_SwordCrash_Particle", m_szParticleName) ||
		!lstrcmp(L"QueensKnight_JumpDown_Particle_Red", m_szParticleName) ||
		!lstrcmp(L"QueensKnight_ShieldAttack_Particle", m_szParticleName) ||
		!lstrcmp(L"Boss_Dead_Particle", m_szParticleName) ||
		!lstrcmp(L"ButterFly_PointParticle", m_szParticleName) ||
		!lstrcmp(L"ButterFly_PointParticle_Plum", m_szParticleName) ||
		!lstrcmp(L"ButterFly_VenomShot_PointParticle", m_szParticleName) ||
		!lstrcmp(L"Hit_Slash_Particle_0", m_szParticleName) ||
		!lstrcmp(L"Hit_Slash_Particle_1", m_szParticleName) ||
		!lstrcmp(L"Hit_Slash_Particle_2", m_szParticleName) ||
		!lstrcmp(L"Hit_Slash_Particle_3", m_szParticleName) ||
		!lstrcmp(L"MistletoeParticle", m_szParticleName) ||
		!lstrcmp(L"MistletoeParticle_Sub", m_szParticleName) ||
		!lstrcmp(L"Player_Buff_Particle", m_szParticleName) ||
		!lstrcmp(L"Player_ChargeSpark_Particle", m_szParticleName) ||
		!lstrcmp(L"Player_Heal_Particle", m_szParticleName) ||
		!lstrcmp(L"Player_Skill_Particle_Explosion", m_szParticleName) ||
		!lstrcmp(L"Player_Skill_RedParticle_Explosion", m_szParticleName) ||
		!lstrcmp(L"Player_Skill_RedParticle_Upper", m_szParticleName) ||
		!lstrcmp(L"Player_SpaceBar_StepParticle", m_szParticleName) ||
		//!lstrcmp(L"SpawnParticle", m_szParticleName) ||
		//!lstrcmp(L"SpawnParticle_Sub", m_szParticleName) ||
		!lstrcmp(L"SpawnParticle_ForBoss", m_szParticleName) ||
		!lstrcmp(L"ItemObject", m_szParticleName) ||
		!lstrcmp(L"ItemObject_Red", m_szParticleName) ||
		!lstrcmp(L"ItemObject_Blue", m_szParticleName) ||
		!lstrcmp(L"ItemObject_Purple", m_szParticleName) ||
		!lstrcmp(L"ItemObject_Yellow", m_szParticleName) ||
		!lstrcmp(L"ItemObject_Green", m_szParticleName)
		//!lstrcmp(L"IceGirl_PointParticle_Blue", m_szParticleName) ||
		//!lstrcmp(L"IceGirl_PointParticle_Green", m_szParticleName) ||
		//!lstrcmp(L"IceGirl_FlashParticle_Blue", m_szParticleName) ||
		//!lstrcmp(L"IceGirl_FlashParticle_Green", m_szParticleName)
		//!lstrcmp(L"MapMist", m_szParticleName)
		)
		m_bInstanceTarget = true;

	return S_OK;
}

_int CTexEffect::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CGameObject::LateInit_GameObject();
	
	if (m_fCreateDelay > 0.f)
	{
		Check_CreateDelay(TimeDelta);
		return S_OK;
	}

	CGameObject::Update_GameObject(TimeDelta);

	m_fLinearMovePercent += _float(TimeDelta) * 0.2f;

	Check_Frame(TimeDelta);
	Check_LifeTime(TimeDelta);

	Check_Move(TimeDelta);
	Setup_Billboard();
	Check_Alpha(TimeDelta);
	Check_Color(TimeDelta);

	// 어쩔수 없이 Update에서 호출
	if (m_bIsDead || m_fCreateDelay > 0.f)
		return S_OK;

	RENDERID eGroup = RENDERID::RENDER_EFFECT;
	if (m_iPass == 0)
		eGroup = RENDERID::RENDER_EFFECT;
	else
		eGroup = RENDERID::RENDER_DISTORTION;

	if(m_bInstanceTarget)
		eGroup = RENDERID::RENDER_INSTANCE;
	
	if (FAILED(m_pRendererCom->Add_RenderList(eGroup, this)))
		return E_FAIL;

	return S_OK;
}

_int CTexEffect::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (m_bIsDead || m_fCreateDelay > 0.f)
		return S_OK;
		
	return S_OK;
}


HRESULT CTexEffect::Render_GameObject()
{
	Render_GameObject_HWInstance(); // 텍스쳐 이펙트만 인스턴싱

	//if (FAILED(SetUp_ConstantTable()))
	//	return E_FAIL;
	//
	//m_pShaderCom->Begin_Shader();
	//m_pShaderCom->Begin_Pass(4);
	//
	//VTXCUBE_COL VtxCubeCol[8];
	//VtxCubeCol[0].vPosition = _v3(-0.5f, 0.5f, -0.5f);
	//VtxCubeCol[0].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[1].vPosition = _v3(0.5f, 0.5f, -0.5f);
	//VtxCubeCol[1].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[2].vPosition = _v3(0.5f, -0.5f, -0.5f);
	//VtxCubeCol[2].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[3].vPosition = _v3(-0.5f, -0.5f, -0.5f);
	//VtxCubeCol[3].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[4].vPosition = _v3(-0.5f, 0.5f, 0.5f);
	//VtxCubeCol[4].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[5].vPosition = _v3(0.5f, 0.5f, 0.5f);
	//VtxCubeCol[5].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[6].vPosition = _v3(0.5f, -0.5f, 0.5f);
	//VtxCubeCol[6].dwColor = COLOR_GREEN(1.f);
	//
	//VtxCubeCol[7].vPosition = _v3(-0.5f, -0.5f, 0.5f);
	//VtxCubeCol[7].dwColor = COLOR_GREEN(1.f);
	//
	//POLYGON32 wIdx[12] = {};
	//
	//wIdx[0]._0 = 1;
	//wIdx[0]._1 = 5;
	//wIdx[0]._2 = 6;
	//
	//wIdx[1]._0 = 1;
	//wIdx[1]._1 = 6;
	//wIdx[1]._2 = 2;
	//
	//wIdx[2]._0 = 4;
	//wIdx[2]._1 = 0;
	//wIdx[2]._2 = 3;
	//
	//wIdx[3]._0 = 4;
	//wIdx[3]._1 = 3;
	//wIdx[3]._2 = 7;
	//
	//wIdx[4]._0 = 4;
	//wIdx[4]._1 = 5;
	//wIdx[4]._2 = 1;
	//
	//wIdx[5]._0 = 4;
	//wIdx[5]._1 = 1;
	//wIdx[5]._2 = 0;
	//
	//wIdx[6]._0 = 3;
	//wIdx[6]._1 = 2;
	//wIdx[6]._2 = 6;
	//
	//wIdx[7]._0 = 3;
	//wIdx[7]._1 = 6;
	//wIdx[7]._2 = 7;
	//
	//wIdx[8]._0 = 5;
	//wIdx[8]._1 = 4;
	//wIdx[8]._2 = 7;
	//
	//wIdx[9]._0 = 5;
	//wIdx[9]._1 = 7;
	//wIdx[9]._2 = 6;
	//
	//wIdx[10]._0 = 0;
	//wIdx[10]._1 = 1;
	//wIdx[10]._2 = 2;
	//
	//wIdx[11]._0 = 0;
	//wIdx[11]._1 = 2;
	//wIdx[11]._2 = 3;
	//
	//m_pGraphic_Dev->SetFVF(VTXFVF_COL);
	//m_pGraphic_Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, wIdx, D3DFMT_INDEX32, VtxCubeCol, sizeof(VTX_COL));
	//
	//m_pShaderCom->End_Pass();
	//m_pShaderCom->End_Shader();
	return NOERROR;
}

HRESULT CTexEffect::Render_GameObject_SetShader(CShader* pShader)
{
	if (nullptr == pShader ||
		nullptr == m_pBufferCom)
		return E_FAIL;
		
	pShader->Begin_Pass(m_iPass);
	
	// Set Texture
	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;
	
	pShader->Commit_Changes();
	
	m_pBufferCom->Render_VIBuffer();
	pShader->End_Pass();
	
	return S_OK;
}

HRESULT CTexEffect::Render_GameObject_HWInstance()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;
	
	//m_pBufferCom->Render_Before_Instancing(m_pTransformCom->Get_WorldMat());
	
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(m_iPass);
	
	// Set Texture
	if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
		return E_FAIL;
	
	// Begin Pass 사이에 SetTexture 할 경우 바로 적용시키기 위해
	m_pShaderCom->Commit_Changes();
	
	m_pBufferCom->Render_DrawPrimitive_Instancing();
	
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();
	
	m_pBufferCom->Render_After_Instancing();

	return NOERROR;
}

void CTexEffect::Setup_Info()
{
	m_fLifeTime = m_pInfo->fLifeTime;
	m_vColor = m_pInfo->vStartColor;
	m_fAlpha = m_pInfo->fMaxAlpha;
	m_fMoveSpeed = m_pInfo->fMoveSpeed;
	m_fRotSpeed = m_pInfo->fRotSpeed;
	m_fAlphaSpeed = m_pInfo->fAlphaSpeed;
	if (m_bDelay_New)
		m_fCreateDelay = m_fDelay_New;
	else
		m_fCreateDelay = m_pInfo->fCreateDelay;

	m_fFrame = 0.f;
	m_fLinearMoveSpeed = 0.f;
	m_fLinearMovePercent = 0.f;
	m_vFollowPos = { 0.f, 0.f, 0.f };
	m_fAccel = 0.f;
	m_fDissolve = 0.f;

	m_bFadeOutStart = false;

	if (m_pInfo->bDistortion)
		m_iPass = 1;
	else
		m_iPass = 0;

	if(m_pInfo->fDistortionPower <= 0.f || m_pInfo->fDistortionPower > 2.f)
		m_pInfo->fDistortionPower = 0.09f;

	if (m_pInfo->bFadeIn)
		m_fAlpha = 0.f;

	if (m_pInfo->bStaticFrame)
		m_fFrame = m_pInfo->fMaxFrame;

	if (m_pInfo->bRandScale)
	{
		// rand 설정하면 x 값기준으로 동일하게 조정
		_float fScale = Engine::CCalculater::Random_Num(_int(m_pInfo->vStartScale.x * 30), _int(m_pInfo->vStartScale.x * 100)) * 0.01f;
		_v3 vSize = _v3(fScale, fScale, fScale);

		m_vLerpScale = vSize;
		m_pTransformCom->Set_Scale(vSize);
	}
	else
	{
		m_vLerpScale = m_pInfo->vStartScale;
		m_pTransformCom->Set_Scale(m_pInfo->vStartScale);
	}

	if (m_pInfo->bRandomMove)
	{
		_v3 vRandDir = _v3(1.f, 1.f, 1.f);
		vRandDir = Engine::CCalculater::Random_Dir(m_pInfo->vRandDirectionRange.x, m_pInfo->vRandDirectionRange.y, m_pInfo->vRandDirectionRange.z);
		D3DXVec3Normalize(&vRandDir, &vRandDir);
		m_vDir = vRandDir;
	}

	if (m_pInfo->fMoveSpeed_Max > 0.f)
	{
		m_fMoveSpeed = Engine::CCalculater::Random_Num(0, _int(m_pInfo->fMoveSpeed_Max * 100)) * 0.01f;
		m_fMoveSpeed += _int(m_pInfo->fMoveSpeed_Min);
	}

	if (m_pInfo->fRotSpeed_Max > 0.f)
	{
		m_fRotSpeed = Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRotSpeed_Max * 100)) * 0.01f;
		m_fRotSpeed += _int(m_pInfo->fRotSpeed_Min);
	}

	if (m_pInfo->fAlphaSpeed_Max > 0.f)
	{
		m_fAlphaSpeed = Engine::CCalculater::Random_Num(_int(m_pInfo->fAlphaSpeed_Min * 100), _int(m_pInfo->fAlphaSpeed_Max * 100)) * 0.01f;
		//m_fAlphaSpeed += _int(m_pInfo->fAlphaSpeed_Min);
	}

	if (m_pInfo->fCreateDelay_Max > 0.f)
	{
		m_fCreateDelay += Engine::CCalculater::Random_Num(0, _int(m_pInfo->fCreateDelay_Max * 100)) * 0.01f;
		m_fCreateDelay += _int(m_pInfo->fCreateDelay_Min);
	}

	if (m_pInfo->bRandStartPos)
	{
		_v3 vPos = _v3(m_pInfo->fRandStartPosRange_Min[AXIS_X], m_pInfo->fRandStartPosRange_Min[AXIS_Y], m_pInfo->fRandStartPosRange_Min[AXIS_Z]);

		vPos += _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_X] * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_Y] * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_Z] * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f));

		if (m_pDesc->pTargetTrans || m_bAutoFindPos)
			vPos += m_pDesc->pTargetTrans->Get_Pos();

		if (m_pTargetMatrix)
			vPos += _v3(m_pTargetMatrix->_41, m_pTargetMatrix->_42, m_pTargetMatrix->_43);

		vPos += m_pDesc->vWorldPos;
		m_pTransformCom->Set_Pos(vPos);
		m_vLerpPos = (vPos);
	}
	else
	{
		_v3 vPos = m_pInfo->vStartPos + m_pDesc->vWorldPos;
		if (m_pDesc->pTargetTrans)
			vPos += m_pDesc->pTargetTrans->Get_Pos();

		if (m_pTargetMatrix)
			vPos += _v3(m_pTargetMatrix->_41, m_pTargetMatrix->_42, m_pTargetMatrix->_43);

		m_pTransformCom->Set_Pos(vPos);
		m_vLerpPos = (vPos);
	}

	if (m_pInfo->bRotMove)
	{
		if (m_pInfo->bRandomRot)
		{
			_v3 vPos = _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.x * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.y * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.z * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f));

			m_vRot = vPos;
		}
	}
	else
	{
		if (m_pInfo->bRandomRot)
		{
			_v3 vPos = _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.x * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.y * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.z * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f));

			m_pTransformCom->Set_Angle(_v3(D3DXToRadian(vPos)));
		}
		else
		{
			m_pTransformCom->Set_Angle(_v3(D3DXToRadian(m_pInfo->vRotDirection.x), D3DXToRadian(m_pInfo->vRotDirection.y), D3DXToRadian(m_pInfo->vRotDirection.z)));
		}
	}
}

void CTexEffect::Setup_Billboard()
{
	_mat matBill, matView, matWorld;

	matWorld = m_pTransformCom->Get_WorldMat();
	matView = m_pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixIdentity(&matBill);

	if (m_pInfo->bBillBoard)
	{
		matBill = matView;
		memset(&matBill._41, 0, sizeof(_v3));
		D3DXMatrixInverse(&matBill, NULL, &matBill);

		m_pTransformCom->Set_WorldMat((matBill * matWorld));
	}
	else if (m_pInfo->bOnlyYRot)
	{
		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		D3DXMatrixInverse(&matBill, NULL, &matBill);

		m_pTransformCom->Set_WorldMat((matBill * matWorld));
	}
	//else
	//	m_pTransformCom->Update_Component();

	Compute_ViewZ(&m_pTransformCom->Get_Pos());
}

void CTexEffect::Check_Frame(_double TimeDelta)
{
	if (m_pInfo->bStaticFrame)
		return;

	m_fFrame += m_pInfo->fMaxFrame * m_pInfo->fAnimSpeed * _float(TimeDelta);
	if (m_pInfo->fMaxFrame != 0.f && m_pInfo->fMaxFrame <= m_fFrame)
	{
		if (m_bLoop)
		{
			m_fFrame = 0.f;
			m_fLifeTime = m_pInfo->fLifeTime;
			m_fAlpha = m_pInfo->fMaxAlpha;
			return;
		}
		m_bIsDead = true;
	}
}

void CTexEffect::Check_Move(_double TimeDelta)
{
	if (m_pInfo->bSlowly)
	{
		m_fMoveSpeed -= m_fMoveSpeed * _float(TimeDelta);
		if (m_fMoveSpeed <= 0.f)
			m_fMoveSpeed = 0.f;
	}

	_v3 vTargetPos = V3_NULL;
	if (m_pDesc->pTargetTrans) vTargetPos = m_pDesc->pTargetTrans->Get_Pos();
	if (m_pTargetMatrix) vTargetPos += _v3(m_pTargetMatrix->_41, m_pTargetMatrix->_42, m_pTargetMatrix->_43);

	if (m_pInfo->bDirMove)
	{
		if (m_pInfo->bLinearMove)
		{
			// Speed로 바꾸기
			D3DXVec3Lerp(&m_vLerpPos, &m_vLerpPos, &m_pInfo->vMoveDirection, m_fLinearMovePercent);

			m_pTransformCom->Set_Pos(m_vLerpPos);
		}
		else
		{
			_v3 vMove = V3_NULL;
			if (m_vMyDir != V3_NULL)
				vMove = m_vMyDir * m_fMoveSpeed * _float(TimeDelta);
			else
				vMove = m_pInfo->vMoveDirection * m_fMoveSpeed * _float(TimeDelta);
			
 			if ((m_pDesc->pTargetTrans && !m_bAutoFindPos)
				|| m_pTargetMatrix)
			{
				_v3 vPos = vTargetPos + m_pDesc->vWorldPos;
				m_vFollowPos += vMove;
				vPos += m_vFollowPos;
				m_pTransformCom->Set_Pos(vPos);
			}
			else if (m_bFinishPos)
			{
				//D3DXVec3Lerp(&m_vLerpPos, &m_vLerpPos, &m_vFinishPos, m_fLinearMovePercent);

				_v3 vDir = m_vFinishPos - m_pTransformCom->Get_Pos();
				vMove = vDir * m_fMoveSpeed * _float(TimeDelta);

				//m_pTransformCom->Set_Pos(m_vLerpPos + vMove);
				m_pTransformCom->Add_Pos(vMove);
			}
			else
			{
				m_pTransformCom->Add_Pos(vMove);
			}
		}

	}

	if (m_pInfo->bRandomMove)
	{
		_v3 vMove = m_vDir * m_fMoveSpeed * _float(TimeDelta);
		if ((m_pDesc->pTargetTrans && !m_bAutoFindPos)
			|| m_pTargetMatrix)
		{
			_v3 vPos = vTargetPos + m_pDesc->vWorldPos;
			m_vFollowPos += vMove;
			vPos += m_vFollowPos;
			m_pTransformCom->Set_Pos(vPos);
		}
		else
		{
			m_pTransformCom->Add_Pos(vMove);
		}
	}

	if (m_pInfo->bGravity)
	{
		m_fAccel += _float(TimeDelta);
		_float fY = (-GRAVITY * m_fAccel * m_fAccel * 0.5f) *  _float(TimeDelta);
		_v3 vPos = m_pTransformCom->Get_Pos();
		vPos.y += fY;
		m_pTransformCom->Set_Pos(vPos);
	}



	if (m_pInfo->bRotMove)
	{
		if (m_pInfo->bRandomRot)
		{
			m_pTransformCom->Add_Angle(AXIS_X, ((m_vRot.x) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Y, ((m_vRot.y) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Z, ((m_vRot.z) * _float(TimeDelta) * m_fRotSpeed));
		}
		else
		{
			m_pTransformCom->Add_Angle(AXIS_X, ((m_pInfo->vRotDirection.x) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Y, ((m_pInfo->vRotDirection.y) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Z, ((m_pInfo->vRotDirection.z) * _float(TimeDelta) * m_fRotSpeed));
		}
	}

	if (m_pInfo->bMoveWithRot)
	{
		_mat matRotX, matRotY, matRotZ;
		//_v3 vDirX = m_pTransformCom->Get_Axis(AXIS_X), vDirY = m_pTransformCom->Get_Axis(AXIS_Y), vDirZ = m_pTransformCom->Get_Axis(AXIS_Z);
		_v3 vAngle = m_pTransformCom->Get_Angle();
		_v3 vDir = vAngle;
		D3DXMatrixIdentity(&matRotX);
		D3DXMatrixIdentity(&matRotY);
		D3DXMatrixIdentity(&matRotZ);

		D3DXMatrixRotationX(&matRotX, D3DXToRadian(vAngle.x));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(vAngle.y));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(vAngle.z));
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotX);
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotZ);

		//vDirZ = vDirX + vDirY + vDirZ;
		D3DXVec3Normalize(&vDir, &vDir);

		m_pTransformCom->Add_Pos(m_fMoveSpeed * _float(TimeDelta), vDir);
	}

	if(!m_pInfo->bRotMove && !m_pInfo->bMoveWithRot
		&& m_pInfo->vRotDirection == V3_NULL)
		m_pTransformCom->Set_Angle(m_vAngle);

	m_pTransformCom->Update_Component();

	if (m_pInfo->bScaleMove)
	{
		D3DXVec3Lerp(&m_vLerpScale, &m_vLerpScale, &m_pInfo->vMoveScale, m_fLinearMovePercent * m_pInfo->fMoveScaleSpeed);
		//m_vLerpScale -= m_pInfo->vStartScale * _float(TimeDelta);
		m_pTransformCom->Set_Scale(m_vLerpScale);
	}
}

void CTexEffect::Check_LifeTime(_double TimeDelta)
{
	m_fLifeTime -= _float(TimeDelta);

	if (0.f >= m_fLifeTime)
		m_bIsDead = true;
}

void CTexEffect::Check_Alpha(_double TimeDelta)
{
	if (!m_bFadeOutStart && m_pInfo->bFadeIn)
	{
		m_fAlpha += _float(TimeDelta) * m_fAlphaSpeed;
	}

	if (m_fAlpha >= m_pInfo->fMaxAlpha)
		m_bFadeOutStart = true;

	if (m_bFadeOutStart && m_pInfo->bFadeOut)
	{
		m_fAlpha -= _float(TimeDelta) * m_fAlphaSpeed;
	}

	if (m_fAlpha < 0.f)
		m_fAlpha = 0.f;
	if (m_pInfo->fMaxAlpha < m_fAlpha)
		m_fAlpha = m_pInfo->fMaxAlpha;

	//if(m_bDissolveToggle)
	//	m_fDissolve -= _float(TimeDelta) * m_fAlphaSpeed;
	//else
		m_fDissolve += _float(TimeDelta) * m_fAlphaSpeed;

	//if (m_fDissolve < 0.f)
	//	m_bDissolveToggle = !m_bDissolveToggle;
	if (m_fDissolve > 1.f)
	{
		m_fDissolve = 1.f;
		m_bDissolveToggle = !m_bDissolveToggle;
	}
}

void CTexEffect::Check_Color(_double TimeDelta)
{
	if (m_pInfo->bColorMove)
	{
		D3DXVec4Lerp(&m_vColor, &m_vColor, &m_pInfo->vEndColor, _float(TimeDelta) * m_pInfo->fColorSpeed);
	}
}

void CTexEffect::Check_CreateDelay(_double TimeDelta)
{
	m_fCreateDelay -= _float(TimeDelta);

	if (m_fCreateDelay < 0.f)
	{
		m_fCreateDelay = 0.f;
	}
}

HRESULT CTexEffect::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_LinePoint", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_GradientTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Gradient", L"Com_GradientTexture", (CComponent**)&m_pGradientTextureCom)))
		return E_FAIL;

	// For.Com_ColorTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Colors", L"Com_ColorTexture", (CComponent**)&m_pColorTextureCom)))
		return E_FAIL;

	//// For.Com_Shader
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTexEffect::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_mat matWorld = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &matWorld, sizeof(_mat))))
		return E_FAIL;
	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	if (FAILED(pShader->Set_Value("g_matInvWorld", &matWorld, sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	if (FAILED(pShader->Set_Value("g_matProjInv", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matViewInv", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_fDistortion", &m_pInfo->fDistortionPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_vColor", &m_vColor, sizeof(_v4))))
		return E_FAIL;

	if (FAILED(pShader->Set_Bool("g_bUseColorTex", m_pInfo->bUseColorTex)))
		return E_FAIL;
	if (FAILED(pShader->Set_Bool("g_bReverseColor", m_pInfo->bRevColor)))
		return E_FAIL;
	if (FAILED(pShader->Set_Bool("g_bUseRGBA", m_pInfo->bUseRGBA)))
		return E_FAIL;

	if (FAILED(pShader->Set_Bool("g_bDissolve", m_pInfo->bDissolve)))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fDissolve", &m_fDissolve, sizeof(_float))))
		return E_FAIL;

	_float fMaskIndex = 0.f;
	if (FAILED(pShader->Set_Bool("g_bUseMaskTex", (m_pInfo->fMaskIndex != -1.f))))
		return E_FAIL;

	if ((m_pInfo->fMaskIndex != -1.f))
		fMaskIndex = m_pInfo->fMaskIndex;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", pShader, _uint(m_fFrame))))
		return E_FAIL;
	if (FAILED(m_pGradientTextureCom->SetUp_OnShader("g_GradientTexture", pShader, _uint(fMaskIndex))))
		return E_FAIL;
	if (FAILED(m_pColorTextureCom->SetUp_OnShader("g_ColorTexture", pShader, _uint(m_pInfo->fColorIndex))))
		return E_FAIL;

	pShader->Set_Texture("g_DepthTexture", pManagement->Get_Target_Texture(L"Target_Depth"));

	Safe_Release(pManagement);

	return NOERROR;
}

void CTexEffect::Change_EffectTexture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CTexEffect::Change_GradientTexture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_GradientTexture");

	// 둘 다 해제
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pGradientTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CTexEffect::Change_ColorTexture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_ColorTexture");

	Safe_Release(m_pColorTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pColorTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

CTexEffect* CTexEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTexEffect*	pInstance = new CTexEffect(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CTexEffect* CTexEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo)
{
	CTexEffect*	pInstance = new CTexEffect(pGraphic_Device);

	pInstance->m_pInfo = pInfo;

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTexEffect::Clone_GameObject(void* pArg)
{
	CTexEffect*	pInstance = new CTexEffect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexEffect::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	//Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColorTextureCom);
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(m_pRendererCom);

	//Safe_Release(m_pManagement);

	CEffect::Free();
}
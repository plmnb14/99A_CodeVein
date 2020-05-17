#include "..\Headers\OrthoEffect.h"

COrthoEffect::COrthoEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{

}

COrthoEffect::COrthoEffect(const COrthoEffect& rhs)
	: CEffect(rhs)
{
	CEffect::m_pInfo = rhs.m_pInfo;

	m_bClone = true;
}

INSTANCEDATA COrthoEffect::Get_InstanceData()
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

void COrthoEffect::Set_UV_Speed(_float fX, _float fY)
{
	m_fUV_Speed_X = fX;
	m_fUV_Speed_Y = fY;
}

void COrthoEffect::Set_Mask(const _tchar* _Name, _int _iMaskIdx)
{
	Change_MaskTexture(_Name);

	m_iMaskIdx = _iMaskIdx;
}

void COrthoEffect::Set_UI_Layer()
{
	m_bUILayer = true;
}

void COrthoEffect::Set_ViewZ(_float fViewZ)
{
	m_fViewZ = fViewZ;
}

HRESULT COrthoEffect::SetUp_ConstantTable_Instance(CShader* pShader)
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

HRESULT COrthoEffect::Ready_GameObject_Prototype()
{
	// 생성 시, 오래 걸릴 수 있는 작업들을 수행한다.

	if (!m_pInfo)
	{
		m_pInfo = new EFFECT_INFO;
		ZeroMemory(m_pInfo, sizeof(EFFECT_INFO));
	}
	return NOERROR;
}

HRESULT COrthoEffect::Ready_GameObject(void* pArg)
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

	m_iLayer = 9;

	return NOERROR;
}

HRESULT COrthoEffect::LateInit_GameObject()
{
	Setup_Info();
	Change_EffectTexture(m_pInfo->szName);
	Change_GradientTexture(m_pInfo->szGradientName);
	Change_ColorTexture(m_pInfo->szColorName);
	Change_MaskTexture(L"DefaultTex_Ortho_Title");

	return S_OK;
}

_int COrthoEffect::Update_GameObject(_double TimeDelta)
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
	m_fUV_Value_X += _float(TimeDelta) * m_fUV_Speed_X;
	m_fUV_Value_Y += _float(TimeDelta) * m_fUV_Speed_Y;

	Check_Frame(TimeDelta);
	Check_LifeTime(TimeDelta);
	Check_Move(TimeDelta);
	Check_Alpha(TimeDelta);
	Check_Color(TimeDelta);

	Setup_Ortho();

	// 어쩔수 없이 Update에서 호출
	if (m_bIsDead || m_fCreateDelay > 0.f)
		return S_OK;

	RENDERID eGroup = RENDERID::RENDER_ORTHO;
	if (m_bUILayer)
		eGroup = RENDERID::RENDER_UI;

	if (FAILED(m_pRendererCom->Add_RenderList(eGroup, this)))
		return E_FAIL;

	return S_OK;
}

_int COrthoEffect::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (m_bIsDead || m_fCreateDelay > 0.f)
		return S_OK;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = WINCX * m_vLerpScale.x;
	m_matWorld._22 = WINCY * m_vLerpScale.y;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_pTransformCom->Get_Pos().x * WINCX;// -WINCX * 0.5f;
	m_matWorld._42 = -m_pTransformCom->Get_Pos().y * WINCY;// +WINCY * 0.5f;
		
	return S_OK;
}


HRESULT COrthoEffect::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	m_matOldView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	m_matOldProj = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	CManagement::Get_Instance()->Set_Transform(D3DTS_WORLD, m_matWorld);
	CManagement::Get_Instance()->Set_Transform(D3DTS_VIEW, m_matView);
	CManagement::Get_Instance()->Set_Transform(D3DTS_PROJECTION, m_matProj);

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(m_iPass);

	// Set Texture
	if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
		return E_FAIL;

	m_pShaderCom->Commit_Changes();

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	CManagement::Get_Instance()->Set_Transform(D3DTS_VIEW, m_matOldView);
	CManagement::Get_Instance()->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return S_OK;
}

HRESULT COrthoEffect::Render_GameObject_SetShader(CShader* pShader)
{
	if (nullptr == pShader ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	m_matOldView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	m_matOldProj = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	CManagement::Get_Instance()->Set_Transform(D3DTS_WORLD, m_matWorld);
	CManagement::Get_Instance()->Set_Transform(D3DTS_VIEW, m_matView);
	CManagement::Get_Instance()->Set_Transform(D3DTS_PROJECTION, m_matProj);

	pShader->Begin_Pass(m_iPass);
	
	// Set Texture
	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;
	
	pShader->Commit_Changes();
	
	m_pBufferCom->Render_VIBuffer();
	pShader->End_Pass();

	CManagement::Get_Instance()->Set_Transform(D3DTS_VIEW, m_matOldView);
	CManagement::Get_Instance()->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return S_OK;
}

void COrthoEffect::Setup_Info()
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
	m_fCurveAccel = 2.f;
	m_fDissolve = 0.f;

	m_bFadeOutStart = false;

	if (m_pInfo->bDistortion)
		m_iPass = 1;
	else
		m_iPass = 9;

	if (m_bZwrite)
		m_iPass = 8;

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
		m_fMoveSpeed = Engine::CCalculater::Random_Num(_int(m_pInfo->fMoveSpeed_Min * 100), _int(m_pInfo->fMoveSpeed_Max * 100)) * 0.01f;
		//m_fMoveSpeed += _int(m_pInfo->fMoveSpeed_Min);
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

	if (m_bCurve)
	{
		_mat matRotX, matRotY, matRotZ;
		m_vCurveRotDir = _v3(1.f, 1.f, 1.f);
		D3DXMatrixRotationX(&matRotX, D3DXToRadian(CCalculater::Random_Num(0, 360)));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(CCalculater::Random_Num(0, 360)));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(CCalculater::Random_Num(0, 360)));
		D3DXVec3TransformNormal(&m_vCurveRotDir, &m_vCurveRotDir, &matRotX);
		D3DXVec3TransformNormal(&m_vCurveRotDir, &m_vCurveRotDir, &matRotY);
		D3DXVec3TransformNormal(&m_vCurveRotDir, &m_vCurveRotDir, &matRotZ);
		D3DXVec3Normalize(&m_vCurveRotDir, &m_vCurveRotDir);
	}
}

void COrthoEffect::Setup_Ortho()
{
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	//Compute_ViewZ(&m_pTransformCom->Get_Pos());
}

void COrthoEffect::Check_Frame(_double TimeDelta)
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

void COrthoEffect::Check_Move(_double TimeDelta)
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
			vMove = m_pInfo->vMoveDirection * m_fMoveSpeed * _float(TimeDelta);

			if (m_vMyDir != V3_NULL)
				vMove += m_vMyDir * m_fMoveSpeed * _float(TimeDelta);

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
				_v3 vDir = m_vFinishPos - m_pTransformCom->Get_Pos();
				D3DXVec3Normalize(&vDir, &vDir);
				vMove = (vDir * m_fMoveSpeed * 1.5f) * _float(TimeDelta);

				m_pTransformCom->Add_Pos(vMove);
			}
			else if (m_bCurve)
			{
				_v3 vDir = m_pCurveTargetTrans->Get_Pos() - m_pTransformCom->Get_Pos();
				if (D3DXVec3Length(&vDir) < 0.1f)
				{
					Set_Dead();
					return;
				}
				vMove = vDir * m_fMoveSpeed * _float(TimeDelta);

				m_fCurveAccel -= _float(TimeDelta);
				_float fCurveValue = (m_fCurvePower * m_fCurveAccel + GRAVITY * m_fCurveAccel * m_fCurveAccel * 0.5f) * _float(TimeDelta);

				_v3 vCurveDir = *D3DXVec3Cross(&vCurveDir, &m_vCurveRotDir, &vDir);

				vMove += fCurveValue * vCurveDir * m_fMoveSpeed * _float(TimeDelta);

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
		vPos.y -= fY;
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

	if (!m_pInfo->bRotMove && !m_pInfo->bMoveWithRot
		&& m_pInfo->vRotDirection == V3_NULL)
	{
		m_pTransformCom->Set_Angle(m_vAngle);
		m_pTransformCom->Update_Component();
	}

	if (m_pParentObject && !m_pParentObject->Get_Dead())
	{
		CTransform* pTargetTrans = TARGET_TO_TRANS(m_pParentObject);
		if (!pTargetTrans)
			return;

		_mat matParent = pTargetTrans->Get_WorldMat();
		_mat matWorld = m_pTransformCom->Get_WorldMat();
		m_pTransformCom->Set_WorldMat(matWorld * matParent);
	}

	if (m_pInfo->bScaleMove)
	{
		D3DXVec3Lerp(&m_vLerpScale, &m_vLerpScale, &m_pInfo->vMoveScale, m_fLinearMovePercent * m_pInfo->fMoveScaleSpeed);
		//m_vLerpScale -= m_pInfo->vStartScale * _float(TimeDelta);
		m_pTransformCom->Set_Scale(m_vLerpScale);
	}
}

void COrthoEffect::Check_LifeTime(_double TimeDelta)
{
	m_fLifeTime -= _float(TimeDelta);

	if (0.f >= m_fLifeTime)
		m_bIsDead = true;
}

void COrthoEffect::Check_Alpha(_double TimeDelta)
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

	m_fDissolve += _float(TimeDelta) * m_fAlphaSpeed;
		
	if (m_fDissolve > 1.f)
	{
		m_fDissolve = 1.f;
		m_bDissolveToggle = !m_bDissolveToggle;
	}
}

void COrthoEffect::Check_Color(_double TimeDelta)
{
	if (m_pInfo->bColorMove)
	{
		D3DXVec4Lerp(&m_vColor, &m_vColor, &m_pInfo->vEndColor, _float(TimeDelta) * m_pInfo->fColorSpeed);
	}
}

void COrthoEffect::Check_CreateDelay(_double TimeDelta)
{
	m_fCreateDelay -= _float(TimeDelta);

	if (m_fCreateDelay < 0.f)
	{
		m_fCreateDelay = 0.f;
	}
}

HRESULT COrthoEffect::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Ortho_Title", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_GradientTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Ortho_Title", L"Com_GradientTexture", (CComponent**)&m_pGradientTextureCom)))
		return E_FAIL;

	// For.Com_ColorTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Ortho_Title", L"Com_ColorTexture", (CComponent**)&m_pColorTextureCom)))
		return E_FAIL;

	// For.Com_MaskTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Ortho_Title", L"Com_MaskTexture", (CComponent**)&m_pMaskTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT COrthoEffect::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	//_mat matWorld = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_fUV_Value_X", &m_fUV_Value_X, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fUV_Value_Y", &m_fUV_Value_Y, sizeof(_float))))
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

	// 임시
	_bool bTitle = true;
	if (FAILED(pShader->Set_Bool("g_bTitle", bTitle)))
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
	if (FAILED(m_pMaskTextureCom->SetUp_OnShader("g_MaskTexture", pShader, _uint(m_iMaskIdx))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

void COrthoEffect::Change_EffectTexture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);

	if (!iter->second)
	{
		Change_EffectTexture(L"DefaultTex_Ortho_Title");
	}
}

void COrthoEffect::Change_GradientTexture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_GradientTexture");

	// 둘 다 해제
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pGradientTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);

	if (!iter->second)
	{
		Change_GradientTexture(L"DefaultTex_Ortho_Title");
	}
}

void COrthoEffect::Change_ColorTexture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_ColorTexture");

	Safe_Release(m_pColorTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pColorTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);

	if (!iter->second)
	{
		Change_ColorTexture(L"DefaultTex_Ortho_Title");
	}
}

void COrthoEffect::Change_MaskTexture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_MaskTexture");

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pMaskTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);

	if (!iter->second)
	{
		Change_MaskTexture(L"DefaultTex_Ortho_Title");
	}
}

COrthoEffect* COrthoEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	COrthoEffect*	pInstance = new COrthoEffect(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

COrthoEffect* COrthoEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo)
{
	COrthoEffect*	pInstance = new COrthoEffect(pGraphic_Device);

	pInstance->m_pInfo = pInfo;

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COrthoEffect::Clone_GameObject(void* pArg)
{
	COrthoEffect*	pInstance = new COrthoEffect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COrthoEffect::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColorTextureCom);
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	
	//Safe_Release(m_pManagement);

	CEffect::Free();
}
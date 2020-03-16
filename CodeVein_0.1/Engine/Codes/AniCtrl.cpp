#include "..\Headers\AniCtrl.h"

CAniCtrl::CAniCtrl(LPDIRECT3DDEVICE9 pGraphic_Device, LPD3DXANIMATIONCONTROLLER pAnimationCtrl)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pAnimationCtrl(pAnimationCtrl)
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pAnimationCtrl);
}

CAniCtrl::CAniCtrl(const CAniCtrl & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
	, m_pAnimationCtrl(nullptr)
{
	Safe_AddRef(m_pGraphic_Device);

	if (FAILED(rhs.m_pAnimationCtrl->CloneAnimationController(rhs.m_pAnimationCtrl->GetMaxNumAnimationOutputs(), rhs.m_pAnimationCtrl->GetMaxNumAnimationSets(), rhs.m_pAnimationCtrl->GetMaxNumTracks(), rhs.m_pAnimationCtrl->GetMaxNumEvents(), &m_pAnimationCtrl)))
		return;
}

HRESULT CAniCtrl::Ready_Animation()
{
	return NOERROR;
}

HRESULT CAniCtrl::SetUp_Animation(_uint iIndex)
{
	if (nullptr == m_pAnimationCtrl)
		return E_FAIL;

	if (m_iOldAniIndex == iIndex)
		return NOERROR;

	LPD3DXANIMATIONSET		pAS = nullptr;

	if (FAILED(m_pAnimationCtrl->GetAnimationSet(iIndex, &pAS)))
		return E_FAIL;

	m_Period = pAS->GetPeriod();

	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0;

	m_pAnimationCtrl->SetTrackAnimationSet(m_iNewTrack, pAS);

	m_pAnimationCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAnimationCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	m_pAnimationCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_TimeDeltaAcc + 0.2);
	m_pAnimationCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_TimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);
	m_pAnimationCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_TimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);

	m_pAnimationCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAnimationCtrl->KeyTrackSpeed(m_iNewTrack, 1.f, m_TimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);
	m_pAnimationCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_TimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);

	m_pAnimationCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_pAnimationCtrl->ResetTime();

	m_TimeDeltaAcc = 0.0;

	m_iOldAniIndex = iIndex;

	m_iCurrentTrack = m_iNewTrack;

	return NOERROR;
}

HRESULT CAniCtrl::Play_Animation(_double TimeDelta)
{
	if (nullptr == m_pAnimationCtrl)
		return E_FAIL;

	m_pAnimationCtrl->AdvanceTime(TimeDelta, nullptr);

	m_TimeDeltaAcc += TimeDelta;

	return NOERROR;
}

_bool CAniCtrl::Is_Finish_Animation(_float _fLerpValue)
{
	D3DXTRACK_DESC		Track_Desc;
	ZeroMemory(&Track_Desc, sizeof(D3DXTRACK_DESC));

	m_pAnimationCtrl->GetTrackDesc(m_iCurrentTrack, &Track_Desc);

	if (Track_Desc.Position >= m_Period * _fLerpValue)
		return true;

	else
		return false;

}

CAniCtrl * CAniCtrl::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LPD3DXANIMATIONCONTROLLER pAnimationCtrl)
{
	CAniCtrl*		pInstance = new CAniCtrl(pGraphic_Device, pAnimationCtrl);

	if (FAILED(pInstance->Ready_Animation()))
	{
		MSG_BOX("Failed To Create CAniCtrl Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAniCtrl * CAniCtrl::Clone_AniCtrl()
{
	return new CAniCtrl(*this);
}

void CAniCtrl::Free()
{
	Safe_Release(m_pAnimationCtrl);
	Safe_Release(m_pGraphic_Device);
}

#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

BEGIN(Client)

class CBackGround final : public CUI
{
protected:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype(); // ������ü ���� ��, ȣ��� �Լ�.
	virtual HRESULT Ready_GameObject(void* pArg); // �ΰ��� ��ü ���� ��, ȣ��� �Լ�.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	_float					m_fSpeed = 0.f;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphic_Device); // ������ü ����.
	virtual CGameObject* Clone_GameObject(void* pArg); // ���簴ü ����.
	virtual void Free();
};

END
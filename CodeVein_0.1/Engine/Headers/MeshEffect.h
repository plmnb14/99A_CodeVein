#pragma once

#include "GameObject.h"
#include "Management.h"
#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CMeshEffect final : public CEffect
{
public:

protected:
	explicit CMeshEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMeshEffect(const CMeshEffect& rhs);
	virtual ~CMeshEffect() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype(); // ������ü ���� ��, ȣ��� �Լ�.
	virtual HRESULT Ready_GameObject(void* pArg); // �ΰ��� ��ü ���� ��, ȣ��� �Լ�.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject();
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:

private:
	void Check_Move(_double TimeDelta);
	void Check_LifeTime(_double TimeDelta);
	void Check_Alpha(_double TimeDelta);
	void Check_Color(_double TimeDelta);
	void Check_CreateDelay(_double TimeDelta);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void Change_Texture(const _tchar* _Name);
	void Change_Mesh(const _tchar* _Name);

public:
	static CMeshEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device); // ������ü ����.
	static CMeshEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);

	virtual CGameObject* Clone_GameObject(void* pArg); // ���簴ü ����.
	virtual void Free();
};

END
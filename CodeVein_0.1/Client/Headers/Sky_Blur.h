#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CSky_Blur final : public CGameObject
{
protected:
	explicit CSky_Blur(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSky_Blur(const CSky_Blur& rhs);
	virtual ~CSky_Blur() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype(); // ������ü ���� ��, ȣ��� �Լ�.
	virtual HRESULT Ready_GameObject(void* pArg); // �ΰ��� ��ü ���� ��, ȣ��� �Լ�.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Check_Scene();
	void Change_Mesh(const _tchar * _MeshName);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;	
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	//CBuffer_CubeTex*		m_pBufferCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;

	SCENEID					m_eOldSceneID = SCENEID::SCENE_END;
	_tchar					m_szName[STR_128];
private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
public:
	static CSky_Blur* Create(LPDIRECT3DDEVICE9 pGraphic_Device); // ������ü ����.
	virtual CGameObject* Clone_GameObject(void* pArg); // ���簴ü ����.
	virtual void Free();
};

END
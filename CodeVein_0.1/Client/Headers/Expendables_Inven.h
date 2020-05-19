#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables_Slot.h"

BEGIN(Client)
class CExplainExpendUI;
class CExpendables_Inven final : public CUI
{
private:
	explicit CExpendables_Inven(_Device pDevice);
	explicit CExpendables_Inven(const CExpendables_Inven& rhs);
	virtual ~CExpendables_Inven() = default;

public:
	vector<CExpendables_Slot*>* Get_QuickSlot() { return &m_vecQuickSlot; }
	_uint Get_MaximumItemCnt() { return m_iMaximumCnt; }

public:
	void Set_MaximumItemCnt(_uint iMaximum) { m_iMaximumCnt = iMaximum; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Load_Expendables(CExpendables* pExpendables, _uint iIndex = 0); // ���Կ� ������ �ִ� �Լ�(�� ���Դ� 9���� �Ѿ�� ���� ���� �˻�)
	void					Click_Inven(); // Ȱ��ȭ�� ��쿡�� �����
	void					SetUp_Default();
	void					Add_Slot();
	void					Delete_QuickSlot(CExpendables_Slot* pSlot);
	void					Slot_Regist_Sound(_uint iIdx);
	void					Slot_UnRegist_Sound(_uint iIdx);
	void					SetUp_SlotPos();
	void					SetUp_SubUI_Active(_bool bIsActive);

public:
	void Add_Expendables(CExpendables::EXPEND_TYPE eType); // �ش� Ÿ���� �Һ� ������ ȹ��
	void Add_MultiExpendables(CExpendables::EXPEND_TYPE eType, _uint iCnt);
	void Sell_Expendables(_uint iDelete); // ���콺 Ŀ���� �浹�� �������� �μ���ŭ ����	
	void Use_Expendableas(CExpendables_Slot* pSlot); // ������ ��� ������ ���� ���
	
private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	vector<CExpendables_Slot*>	m_vecSlot;
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	vector<CExpendables*>	m_vecExpendables;
	vector<UI_DESC*>		m_vecUI_DESC;
	_uint					m_iMaximumCnt = 3;
	CExplainExpendUI*		m_pExplainUI = nullptr;
	_bool					m_bIsSubActive = false;

public:
	static CExpendables_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END


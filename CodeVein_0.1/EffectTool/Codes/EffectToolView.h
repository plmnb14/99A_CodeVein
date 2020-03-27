
// EffectToolView.h : CEffectToolView Ŭ������ �������̽�
//

#pragma once

namespace Engine
{
	class CGameObject;
	class CRenderer;
}

class CTerrain_Guide;
class CMainFrame;
class CParticleTab;
class CEffectToolDoc;
class CEffectToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CEffectToolView();
	DECLARE_DYNCREATE(CEffectToolView)

// Ư���Դϴ�.
public:
	CEffectToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate();

// �����Դϴ�.
public:
	virtual ~CEffectToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
//===========================================================
public:
	void Update();
	void Render();


private:
	void Ready_Default();
	void Ready_Camera();
	void Ready_Layer();
	void Ready_Resource();
	void Release();


private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	Engine::CGameObject*		m_pTerrainGuide = nullptr;
	CRenderer*					m_pRenderer = nullptr;

private:
	CMainFrame*					m_pMainFrm;
	CParticleTab*				m_pParticleTab;

};

#ifndef _DEBUG  // EffectToolView.cpp�� ����� ����
inline CEffectToolDoc* CEffectToolView::GetDocument() const
   { return reinterpret_cast<CEffectToolDoc*>(m_pDocument); }
#endif


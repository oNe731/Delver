#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CSceneManager)

CSceneManager::CSceneManager() : m_pScene(nullptr)
{
}

CSceneManager::~CSceneManager()
{
	Free();
}

CLayer * CSceneManager::Get_Layer(LAYERTAG eLayerTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Layer(eLayerTag);
}

HRESULT CSceneManager::Set_Scene(CScene * pScene)
{
	Safe_Release(m_pScene);

	Renderer()->Clear_RenderGroup();
	
	m_pScene = pScene;

	return S_OK;
}

_int CSceneManager::Update_Scene(const _float & fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	return m_pScene->Update_Scene(fTimeDelta);
}

void CSceneManager::LateUpdate_Scene()
{
	if (nullptr == m_pScene)
		return;

	m_pScene->LateUpdate_Scene();
}

void CSceneManager::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Renderer()->Render_GameObject(pGraphicDev);

	if (m_pScene)
		m_pScene->Render_Scene();
}

void CSceneManager::Free()
{
	Safe_Release(m_pScene);
}
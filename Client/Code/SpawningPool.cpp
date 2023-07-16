#include "..\Header\SpawningPool.h"
#include "Export_Function.h"
#include "DungeonWarrior.h"
#include "DungeonSpider.h"
#include "Wizard.h"
#include "Bat.h"
#include "Alien.h"
#include "Slime.h"
#include "Skeleton.h"

#include "Terrain.h"

CSpawningPool::CSpawningPool(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CSpawningPool::CSpawningPool(const CSpawningPool& rhs)
	: CGameObject(rhs)
{
}

CSpawningPool::~CSpawningPool()
{
}

HRESULT CSpawningPool::Ready_Object()
{
    m_eObjectTag = OBJECTTAG::SPAWNINGPOOL;
    m_eMonsterTag = MONSTERTAG::MONSTER_END;
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    void* pVertices = nullptr;
    m_pBuffer->m_pVB->Lock(0, m_pBuffer->m_dwVtxCnt * m_pBuffer->m_dwVtxSize, &pVertices, 0);
    for (UINT i = 0; i < m_pBuffer->m_dwVtxCnt; ++i)
    {
        m_vecSpawnerVertex.push_back((((VTXCOL*)pVertices) + i)->vPosition);
    }
    m_pBuffer->m_pVB->Unlock();

    void* pIndices = nullptr;
    m_pBuffer->m_pIB->Lock(0, m_pBuffer->m_dwTriCnt * m_pBuffer->m_dwIdxSize, &pIndices, 0);
    for (UINT i = 0; i < m_pBuffer->m_dwTriCnt; ++i)
    {
        m_vecSpawnerIndex.push_back(*(((INDEX32*)pIndices) + i));
    }
    m_pBuffer->m_pIB->Unlock();

    m_pTransform->Scale(_vec3(m_fSpawnRadius, 8.f, m_fSpawnRadius));
    m_pTransform->Translate(_vec3(0.f, 8.f, 0.f));

    return S_OK;
}

_int CSpawningPool::Update_Object(const _float& fTimeDelta)
{
    if (SCENETAG::EDITOR == SceneManager()->Get_Scene()->Get_SceneTag())
    {
        Engine::Renderer()->Add_RenderGroup(RENDER_NONALPHA, this);
        return 0;
    }

    if (SceneManager()->Get_GameStop()) { return 0; } // ! Esc �� MŰ ���� �� ������Ʈ ���߰� �ϴ� �뵵 �Դϴ�.

    _int iExit = __super::Update_Object(fTimeDelta);

    if (!m_MonsterList.empty())
    {
        for (auto& iter = m_MonsterList.begin(); iter != m_MonsterList.end();)
        {
            if ((*iter)->IsDead())
                iter = m_MonsterList.erase(iter);
            else
                ++iter;
        }
    }

    if (m_dqReserved.size() + m_MonsterList.size() < m_iPoolCapacity)
    {
        //++m_iReservedCount;
        m_dqReserved.push_back(fTimeDelta);
    }

    for(int i = 0; i < m_dqReserved.size(); ++i)
    {
        m_dqReserved[i] += fTimeDelta;
        if (m_dqReserved[i] > m_fSpawnTime)
        {
            m_dqReserved.pop_front();
            ReserveSpawn();
        }
    }

    return iExit;
    //return 0;
}

HRESULT CSpawningPool::InitSpawnerBox()
{   

    return S_OK;
}

void CSpawningPool::ReserveSpawn()
{
    mt19937 engine((_uint)time(NULL));           // MT19937 ���� ����
    uniform_int_distribution<__int64> distribution(-m_fSpawnRadius, m_fSpawnRadius); // ���� ����
    auto generator = bind(distribution, engine);

    //++m_iReservedCount;

    CMonster* pMonster = nullptr;
   
    switch (m_eMonsterTag)
    {
    case MONSTERTAG::SPIDER:
        pMonster = CDungeonSpider::Create(m_pGraphicDev);
        break;
    case MONSTERTAG::WARRIOR:
        pMonster = CDungeonWarrior::Create(m_pGraphicDev);
        break;
    case MONSTERTAG::BAT:
        pMonster = CBat::Create(m_pGraphicDev);
        break;
    case MONSTERTAG::WIZARD:
        pMonster = CWizard::Create(m_pGraphicDev);
        break;
    case MONSTERTAG::ALIEN:
        pMonster = CAlien::Create(m_pGraphicDev);
        break;
    case MONSTERTAG::SLIME:
        pMonster = CSlime::Create(m_pGraphicDev);
        break;
    case MONSTERTAG::SKELETON:
        pMonster = CSkeleton::Create(m_pGraphicDev);
        break;
    }

    NULL_CHECK_RETURN(pMonster);
    _float fX = generator();
    _float fZ = generator();
    pMonster->m_pTransform->Translate(m_pTransform->m_vInfo[INFO_POS] + _vec3(fX, 0.f, fZ));
    EventManager()->CreateObject(pMonster, LAYERTAG::GAMELOGIC);
    m_MonsterList.push_back(pMonster);

    CLayer* pLayer = SceneManager()->Get_Layer(LAYERTAG::GAMELOGIC);
    pMonster->Set_Terrain(dynamic_cast<CTerrain*>(pLayer->Get_ObjectList(OBJECTTAG::TERRAIN).front()));
}

void CSpawningPool::Set_SpawnRadius(_float _fRadius)
{
    m_fSpawnRadius = _fRadius; m_pTransform->Scale(_vec3(m_fSpawnRadius, 8.f, m_fSpawnRadius));
}

HRESULT CSpawningPool::Add_Component()
{
    CComponent* pComponent = nullptr;

    //if(SCENETAG::EDITOR == SceneManager()->Get_Scene()->Get_SceneTag())
    pComponent = m_pBuffer = dynamic_cast<CCubeColBf*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_CubeColBf"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

    pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

    return S_OK;
}

CSpawningPool* CSpawningPool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSpawningPool* pInstance = new CSpawningPool(pGraphicDev);

    if (FAILED(pInstance->Ready_Object()))
    {
        Safe_Release<CSpawningPool*>(pInstance);

        MSG_BOX("SpawningPool Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CSpawningPool::Free(void)
{
    __super::Free();
}

void CSpawningPool::Render_Object(void)
{
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    //m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
    m_pBuffer->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    //m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
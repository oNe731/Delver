#include "..\Header\PlayerState_Walk.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "Player.h"

CPlayerState_Walk::CPlayerState_Walk()
{
}

CPlayerState_Walk::CPlayerState_Walk(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CState(pGraphicDev)
{

}

CPlayerState_Walk::~CPlayerState_Walk()
{
}

HRESULT CPlayerState_Walk::Ready_State(CStateMachine* pOwner)
{
	m_pOwner = pOwner;

	return S_OK;
}

STATE CPlayerState_Walk::Update_State(const _float& fTimeDelta)
{
	STATE eState = Key_Input(fTimeDelta);



	return eState;
}

void CPlayerState_Walk::LateUpdate_State()
{
}

void CPlayerState_Walk::Render_State()
{
	//cout << "Walk" << endl;
}

STATE CPlayerState_Walk::Key_Input(const _float& fTimeDelta)
{
	_vec3 vLook = m_pOwner->Get_Transform()->m_vInfo[INFO_LOOK];
	_vec3 vRight = m_pOwner->Get_Transform()->m_vInfo[INFO_RIGHT];

	CPlayer& pPlayer = *dynamic_cast<CPlayer*>(SceneManager()->GetInstance()
		->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());
	STATE	_eState = STATE::IDLE;

	if (Engine::InputDev()->GetInstance()->Key_Pressing(DIK_W))
	{
		m_pOwner->Get_Transform()->Translate(10.f * fTimeDelta * vLook);
		_eState = STATE::ROMIMG;
	}
	if (Engine::InputDev()->GetInstance()->Key_Pressing(DIK_S))
	{
		m_pOwner->Get_Transform()->Translate(10.f * fTimeDelta * -vLook);
		_eState = STATE::ROMIMG;
	}
	if (Engine::InputDev()->GetInstance()->Key_Pressing(DIK_A))
	{
		m_pOwner->Get_Transform()->Translate(10.f * fTimeDelta * -vRight);
		_eState = STATE::ROMIMG;
	}
	if (Engine::InputDev()->GetInstance()->Key_Pressing(DIK_D))
	{
		m_pOwner->Get_Transform()->Translate(10.f * fTimeDelta * vRight);
		_eState = STATE::ROMIMG;
	}

	if (Engine::InputDev()->GetInstance()->Mouse_Pressing(DIM_LB))
	{
		m_pOwner->Get_Host()->Set_AttackTick(true);
		_eState = STATE::ATTACK;
	}


	return _eState;
}

CPlayerState_Walk* CPlayerState_Walk::Create(LPDIRECT3DDEVICE9 pGraphicDev, CStateMachine* pOwner)
{
	CPlayerState_Walk* pState = new CPlayerState_Walk(pGraphicDev);

	if (FAILED(pState->Ready_State(pOwner)))
	{
		Safe_Release<CPlayerState_Walk*>(pState);

		MSG_BOX("Create WalkState Failed");
		return nullptr;
	}

	return pState;
}

void CPlayerState_Walk::Free()
{
	__super::Free();
}

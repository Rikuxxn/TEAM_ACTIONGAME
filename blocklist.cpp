//=============================================================================
//
// ブロックリスト処理 [blocklist.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "blocklist.h"
#include "blockmanager.h"
#include "player.h"
#include "game.h"
#include "manager.h"
#include "particle.h"
#include <algorithm>

// 名前空間stdの使用
using namespace std;

//=============================================================================
// 木箱ブロックのコンストラクタ
//=============================================================================
CWoodBoxBlock::CWoodBoxBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;// リスポーン位置
}
//=============================================================================
// 木箱ブロックのデストラクタ
//=============================================================================
CWoodBoxBlock::~CWoodBoxBlock()
{
	// なし
}
//=============================================================================
// 木箱ブロックの初期化処理
//=============================================================================
HRESULT CWoodBoxBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置をリスポーン位置に設定
	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 木箱ブロックの更新処理
//=============================================================================
void CWoodBoxBlock::Update()
{
	CBlock::Update(); // 共通処理
}
//=============================================================================
// リスポーン処理
//=============================================================================
void CWoodBoxBlock::Respawn(D3DXVECTOR3 resPos)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// 水しぶきパーティクル生成
	CParticle::Create<CWaterParticle>(INIT_VEC3, GetPos(), D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, 10);
	CParticle::Create<CWaterParticle>(INIT_VEC3, GetPos(), D3DXCOLOR(0.3f, 0.5f, 1.0f, 0.5f), 50, 10);

	// リスポーン位置に設定
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// コライダーの更新
	UpdateCollider();

	// 動的に戻す
	SetEditMode(false);
}
//=============================================================================
// セット処理
//=============================================================================
void CWoodBoxBlock::Set(D3DXVECTOR3 pos)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// 鍵ブロックの位置を取得
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// 設置する目標位置
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// 向きをリセット

	Pos = targetPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// コライダーの更新
	UpdateCollider();
}


//=============================================================================
// 岩ブロックのコンストラクタ
//=============================================================================
CRockBlock::CRockBlock()
{
	// 値のクリア
	m_pathPoints = {};
	m_currentTargetIndex = 0;
	m_speed = 86500.0f;
	m_isBridgeSwitchOn = false;
	m_isHit = false;
	m_isPrevHit = false;
	m_isThrough = false;
	m_isPrevThrough = false;
	m_effectTimer = 0;
	m_playedRollSoundID = -1;
	m_dir = INIT_VEC3;
}
//=============================================================================
// 岩ブロックのデストラクタ
//=============================================================================
CRockBlock::~CRockBlock()
{
	// なし
}
//=============================================================================
// 岩ブロックの更新処理
//=============================================================================
void CRockBlock::Update(void)
{
	CBlock::Update();	// 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	if (GetPos().y < RESET_HEIGHT)
	{
		// リスポーン処理
		Respawn(D3DXVECTOR3(2815.5f, 670.0f, -1989.0f));
	}

	CManager::GetCamera()->SetShakeTargetPos(GetPos());// カメラ振動用の対象位置

	if (m_currentTargetIndex < 8)
	{
		// カメラシェイクON
		CManager::GetCamera()->SetCameraShake(true);
	}
	else
	{
		// カメラシェイクOFF
		CManager::GetCamera()->SetCameraShake(false);
	}

	//MoveToTarget();		// チェックポイントへ向けて移動

	//IsPlayerHit();		// プレイヤーとの接触判定
}
//=============================================================================
// リスポーン処理
//=============================================================================
void CRockBlock::Respawn(D3DXVECTOR3 resPos)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// リスポーン位置に設定
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0, 0, 0));

	// コライダーの更新
	UpdateCollider();

	// 現在のチェックポイントインデックスをリセットする
	m_currentTargetIndex = 0;

	// 動的に戻す
	SetEditMode(false);
}
//=============================================================================
// 通過ポイント追加処理
//=============================================================================
void CRockBlock::AddPathPoint(const D3DXVECTOR3& point)
{
	m_pathPoints.push_back(point);
}
//=============================================================================
// 目標に向かって移動する処理
//=============================================================================
void CRockBlock::MoveToTarget(void)
{
	if (m_pathPoints.empty() || m_currentTargetIndex >= (int)m_pathPoints.size())
	{
		return;
	}

	btRigidBody* pRigid = GetRigidBody();

	if (!pRigid)
	{
		return;
	}

	CModelEffect* pModelEffect = nullptr;

	if (m_currentTargetIndex >= 1)
	{
		m_effectTimer++;

		if (m_effectTimer >= ROLL_EFFECT_INTERVAL)
		{// エフェクト発生感覚
			m_effectTimer = 0;

			// ランダムな角度で横に広がる
			float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
			float speed = (rand() % 200) / 30.0f + 0.2f;

			// 移動量
			D3DXVECTOR3 move;
			move.x = cosf(angle) * speed;
			move.z = sinf(angle) * speed;
			move.y = (rand() % 300) / 50.0f + 0.05f; // 少しだけ上方向

			// 向き
			D3DXVECTOR3 rot;
			rot.x = ((rand() % 360) / 180.0f) * D3DX_PI;
			rot.y = ((rand() % 360) / 180.0f) * D3DX_PI;
			rot.z = ((rand() % 360) / 180.0f) * D3DX_PI;

			// サイズ
			D3DXVECTOR3 size;
			size.x = 0.5f + (rand() % 5) * 0.1f;
			size.y = 0.5f + (rand() % 5) * 0.1f;
			size.z = 0.5f + (rand() % 5) * 0.1f;

			// モデルエフェクトの生成
			pModelEffect = CModelEffect::Create("data/MODELS/effectModel_rock.x", D3DXVECTOR3(GetPos().x, GetPos().y - 200.0f, GetPos().z),
				rot, move, size, 180, -0.12f, 0.008f);
		}
	}

	D3DXVECTOR3 currentPos = GetPos();
	D3DXVECTOR3 targetPos = m_pathPoints[m_currentTargetIndex];

	// ターゲット方向ベクトル
	m_dir = targetPos - currentPos;
	float dist = D3DXVec3Length(&m_dir);

	if (dist < 100.0f)  // ある程度近づいたら次のポイントへ
	{
		m_currentTargetIndex++;
		return;
	}

	// 正規化
	D3DXVec3Normalize(&m_dir, &m_dir);

	btVector3 force(0.0f, 0.0f, 0.0f);

	if (m_currentTargetIndex >= 1)
	{
		m_isThrough = true;
	}
	else
	{
		m_isThrough = false;
	}

	bool isThroughNow = m_isThrough;

	if (isThroughNow && !m_isPrevThrough)
	{
		// ループ再生してIDを保存
		if (m_playedRollSoundID == -1)
		{
			m_playedRollSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_ROLL, currentPos, 850.0f, 1650.0f);
		}
	}

	m_isPrevThrough = isThroughNow;

	if (m_currentTargetIndex >= 6)
	{// インデックスが 6 を超えたら

		float fSpeedDown = 0.001f;

		// 減速させる
		force = btVector3(m_dir.x * (m_speed * fSpeedDown), 0.0f, m_dir.z * (m_speed * fSpeedDown));

		if (m_currentTargetIndex >= 8 && m_playedRollSoundID != -1)
		{
			// 転がる音の停止
			CManager::GetSound()->Stop(m_playedRollSoundID);
			m_playedRollSoundID = -1;
		}
	}
	else
	{// 通常
		// Z軸中心で転がす
		force = btVector3(m_dir.x * m_speed, 0.0f, m_dir.z * m_speed);
	}

	// 音源の位置更新（再生中のみ）
	if (m_playedRollSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedRollSoundID, currentPos);
	}

	// 適用中の速度に加える
	pRigid->applyCentralForce(force);
}
//=============================================================================
// プレイヤーとの接触判定処理
//=============================================================================
void CRockBlock::IsPlayerHit(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();

	D3DXVECTOR3 playerPos = pPlayer->GetPos();
	D3DXVECTOR3 rockPos = GetPos();

	float playerRadius = pPlayer->GetRadius();  // プレイヤーの当たり判定半径
	float rockRadius = 240.0f;					// 岩の半径

	float playerHeight = pPlayer->GetHeight();	// プレイヤーの高さ
	float rockHeight = 240.0f;					// 岩の高さ範囲

	// XZ距離チェック
	D3DXVECTOR2 diffXZ = D3DXVECTOR2(playerPos.x - rockPos.x, playerPos.z - rockPos.z);

	float distXZSq = D3DXVec2LengthSq(&diffXZ);
	float hitDistXZ = playerRadius + rockRadius;

	// Y差チェック
	float dy = fabsf(playerPos.y - rockPos.y);
	float hitHeight = (playerHeight * 0.5f) + rockHeight;

	bool isNowHit = m_isHit;

	if (isNowHit && !m_isPrevHit)
	{
		// プレイヤーヒットSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_HIT);
	}

	// プレイヤーが当たっていたかを記録
	m_isPrevHit = isNowHit;

	if (distXZSq < (hitDistXZ * hitDistXZ) && dy < hitHeight)
	{
		m_isHit = true;

		// プレイヤーのリスポーン
		pPlayer->RespawnToCheckpoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));
	}
	else
	{
		m_isHit = false;
	}
}


//=============================================================================
// シーソーブロックのコンストラクタ
//=============================================================================
CSeesawBlock::CSeesawBlock()
{
	// 値のクリア
	m_pHinge = nullptr;
}
//=============================================================================
// シーソーブロックのデストラクタ
//=============================================================================
CSeesawBlock::~CSeesawBlock()
{
	// なし
}
//=============================================================================
// シーソーブロックの初期化処理
//=============================================================================
HRESULT CSeesawBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	return S_OK;
}
//=============================================================================
// シーソーブロックの終了処理
//=============================================================================
void CSeesawBlock::Uninit(void)
{
	if (m_pHinge)
	{
		CManager::GetPhysicsWorld()->removeConstraint(m_pHinge);
		delete m_pHinge;
		m_pHinge = nullptr;
	}

	// ブロックの終了処理
	CBlock::Uninit();
}
//=============================================================================
// シーソーブロックのヒンジ削除処理
//=============================================================================
void CSeesawBlock::RemoveHinge(void)
{
	if (m_pHinge)
	{
		// ワールドから削除
		CManager::GetPhysicsWorld()->removeConstraint(m_pHinge);
		delete m_pHinge;
		m_pHinge = nullptr;
	}
}
//=============================================================================
// シーソーブロックの更新処理
//=============================================================================
void CSeesawBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();
}
//=============================================================================
// シーソーブロックのヒンジ設定処理
//=============================================================================
void CSeesawBlock::SetHinge(void)
{
	// リジッドボディの取得
	btRigidBody* pRigid = GetRigidBody();

	if (!pRigid)
	{// nullだったら
		return;
	}

	// rigidbody の transform を取得
	btTransform transform;
	if (pRigid->getMotionState())
	{
		pRigid->getMotionState()->getWorldTransform(transform);
	}
	else
	{
		transform = pRigid->getCenterOfMassTransform();
	}

	// Z軸をローカル軸としてヒンジに設定
	btVector3 localLongAxis(0, 0, 1);

	btVector3 pivot(0, 0, 0); // ヒンジのピボット（ブロック中心）

	// ヒンジ作成（ローカル空間軸）
	m_pHinge = new btHingeConstraint(*pRigid, pivot, localLongAxis, true);

	// 回転制限
	m_pHinge->setLimit(-SIMD_RADS_PER_DEG * 30.0f, SIMD_RADS_PER_DEG * 30.0f);

	// ワールドに追加
	CManager::GetPhysicsWorld()->addConstraint(m_pHinge, true);
}

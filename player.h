//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYER_H_// このマクロ定義がされていなかったら
#define _PLAYER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "debugproc3D.h"
#include "block.h"
#include "shadowS.h"
#include "effect.h"
#include "state.h"
#include "manager.h"

// 前方宣言
class CPlayer_StandState;
class CPlayer_MoveState;
class CPlayer_JumpState;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PARTS		(32)					// 最大パーツ数
#define PLAYER_SPEED	(200.0f)				// 移動スピード
#define SIDE_MOVE_SPEED (PLAYER_SPEED * 0.6f)	// 横移動は60%のスピード
#define MAX_JUMP_POWER	(330.3f)				// ジャンプ初速
#define MAX_GRAVITY		(-0.26f)				// 重力加速度
#define CAPSULE_RADIUS (18.5f)					// カプセルコライダーの半径
#define CAPSULE_HEIGHT (55.5f)					// カプセルコライダーの高さ

// 入力データ構造体
struct InputData
{
	D3DXVECTOR3 moveDir;      // 前後移動ベクトル
	bool jump;                // ジャンプ入力
};

//*****************************************************************************
// プレイヤークラス
//*****************************************************************************
class CPlayer : public CObject
{
public:
	CPlayer(int nPriority = 2);
	~CPlayer();

	// プレイヤーモーションの種類
	typedef enum
	{
		NEUTRAL = 0,		// 待機
		MOVE,				// 移動
		JUMP,				// ジャンプ
		MAX
	}PLAYER_MOTION;

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }
	void SetIsJumping(bool flag) { m_isJumping = flag; }
	void SetMotion(int type, int nBlendFrame);

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; };
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	btScalar GetRadius(void) const { return m_radius; }
	btScalar GetHeight(void) const { return m_height; }
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得
	bool GetPlayerUse(void) const { return m_playerUse; }
	bool GetOnGround(void) { return m_bOnGround; }
	bool GetIsMoving(void) const { return m_bIsMoving; }
	bool GetIsJumping(void) { return m_isJumping; }
	D3DXVECTOR3 GetForward(void) const;
	InputData GatherInput(void);
	D3DXVECTOR3 GetNearestRespawnPoint(void) const;
	CBlock* FindFrontBlockByRaycast(float rayLength);

	void ReleasePhysics(void);														// Physics破棄用
	void RespawnToCheckpoint(void);
	void RespawnToCheckpoint(D3DXVECTOR3 pos);
	void AddRespawnPoint(const D3DXVECTOR3& pos);
	CBlock* DetectBlockUnder(void);

	// ステート用にフラグ更新
	void UpdateMovementFlags(const D3DXVECTOR3& moveDir)
	{
		m_bIsMoving = (moveDir.x != 0.0f || moveDir.z != 0.0f);
	}

private:
	D3DXVECTOR3 m_pos;					// 位置
	D3DXVECTOR3 m_rot;					// 向き
	D3DXVECTOR3 m_rotDest;				// 向き
	D3DXVECTOR3 m_move;					// 移動量
	D3DXVECTOR3 m_targetMove;			// 目標速度
	D3DXVECTOR3 m_currentMove;			// 実際の移動速度
	D3DXVECTOR3 m_size;					// サイズ
	D3DXVECTOR3 m_colliderPos;			// カプセルコライダーの位置
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	CModel* m_apModel[MAX_PARTS];		// モデル(パーツ)へのポインタ
	CShadowS* m_pShadowS;				// ステンシルシャドウへのポインタ
	CMotion* m_pMotion;					// モーションへのポインタ
	int m_currentMotion;				// 現在のモーション
	btRigidBody* m_pRigidBody;			// 剛体へのポインタ
	btCollisionShape* m_pShape;			// 当たり判定の形へのポインタ
	CDebugProc3D* m_pDebug3D;			// 3Dデバッグ表示へのポインタ
	btScalar m_radius;					// カプセルコライダーの半径
	btScalar m_height;					// カプセルコライダーの高さ
	CBlock* m_pCarryingBlock;			// 運んでいるブロック
	int m_nNumModel;					// モデル(パーツ)の総数
	int m_jumpFrame;					// ジャンプしてから何フレーム経過したか
	bool m_playerUse;					// 使われているかどうか
	bool m_isJumping;					// ジャンプ中フラグ
	bool m_bIsMoving;					// 移動入力フラグ
	bool m_bOnGround;					// 接地フラグ
	std::vector<D3DXVECTOR3> m_ResPos;	// リスポーン地点
	int m_particleTimer;				// パーティクルタイマー
	static constexpr int DASH_PARTICLE_INTERVAL = 10; // パーティクル発生間隔（フレーム数）
	btTypedConstraint* m_pFloorConstraint;
	CBlock* m_pGroundBlock;      // 乗っている床

	// ステートを管理するクラスのインスタンス
	StateMachine<CPlayer> m_stateMachine;
};


//*****************************************************************************
// プレイヤーの待機状態
//*****************************************************************************
class CPlayer_StandState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// 待機モーション
		pPlayer->SetMotion(CPlayer::NEUTRAL, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力を取得
		InputData input = pPlayer->GatherInput();

		// ジャンプ入力があればジャンプステートに移行
		if (input.jump && pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			m_pMachine->ChangeState<CPlayer_JumpState>();
			return;
		}

		// 前方移動入力があれば通常移動ステートに移行
		if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
		{
			m_pMachine->ChangeState<CPlayer_MoveState>();
		}

		D3DXVECTOR3 move = pPlayer->GetMove();
		move *= 0.95f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

//*****************************************************************************
// プレイヤーの移動状態
//*****************************************************************************
class CPlayer_MoveState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// 移動モーション
		pPlayer->SetMotion(CPlayer::MOVE, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力取得
		InputData input = pPlayer->GatherInput();

		// フラグ更新
		pPlayer->UpdateMovementFlags(input.moveDir);

		// 目標速度計算
		D3DXVECTOR3 targetMove = input.moveDir;

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);
			targetMove *= PLAYER_SPEED;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// 現在速度との補間（イージング）
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// 補間後の速度をプレイヤーにセット
		pPlayer->SetMove(currentMove);

		// 物理エンジンにセット
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		// ジャンプ入力があればジャンプステートに切替
		if (input.jump && pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			// ジャンプ状態
			m_pMachine->ChangeState<CPlayer_JumpState>();
		}


		// 移動していなければ待機ステートに戻す
		if (!pPlayer->GetIsMoving() && !pPlayer->GetIsJumping())
		{
			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};


//*****************************************************************************
// プレイヤーのジャンプ状態
//*****************************************************************************
class CPlayer_JumpState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		pPlayer->SetIsJumping(true);

		// ジャンプモーション
		pPlayer->SetMotion(CPlayer::JUMP, 10);

		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();

		// 上方向の速度を直接ジャンプ力にする
		velocity.setY(MAX_JUMP_POWER);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		m_count = 0;
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		m_count++;

		// 入力取得
		InputData input = pPlayer->GatherInput();

		// フラグ更新
		pPlayer->UpdateMovementFlags(input.moveDir);

		// 目標速度計算
		D3DXVECTOR3 targetMove = input.moveDir;

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);
			targetMove *= PLAYER_SPEED;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// 現在速度との補間（イージング）
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// 補間後の速度をプレイヤーにセット
		pPlayer->SetMove(currentMove);

		// 物理エンジンにセット
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (m_count >= 55)
		{// 少し遅延してfalseにする
			pPlayer->SetIsJumping(false);
		}

		// ジャンプ中はステート切り替え処理だけスキップ
		if (pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
			{
				// 移動状態
				m_pMachine->ChangeState<CPlayer_MoveState>();
			}
			else
			{
				// 待機状態
				m_pMachine->ChangeState<CPlayer_StandState>();
			}
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:
	int m_count;
};

#endif

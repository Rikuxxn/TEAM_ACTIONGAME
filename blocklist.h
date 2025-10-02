//=============================================================================
//
// ブロックリスト処理 [blocklist.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"

//*****************************************************************************
// 木箱ブロッククラス
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	CWoodBoxBlock();
	~CWoodBoxBlock();

	HRESULT Init(void);
	void Update(void);
	void Respawn(D3DXVECTOR3 resPos) override;
	void Set(D3DXVECTOR3 pos);

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 4.0f; }  // 質量の取得

private:
	static constexpr float RESPAWN_HEIGHT = -810.0f;
	D3DXVECTOR3 m_ResPos;	// リスポーン位置
};


//*****************************************************************************
// 岩ブロッククラス
//*****************************************************************************
class CRockBlock : public CBlock
{
public:
	CRockBlock();
	~CRockBlock();

	void Update(void);
	void Respawn(D3DXVECTOR3 resPos) override;
	void AddPathPoint(const D3DXVECTOR3& point);// チェックポイント追加 (通常時用)
	void MoveToTarget(void);					// 転がし処理
	void IsPlayerHit(void);						// プレイヤーとの接触判定
	D3DXVECTOR3 GetVelocity(void) override { return m_dir; }

	// コライダー
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		float radius = std::max({ size.x, size.y, size.z }) * 0.5f;
		return new btSphereShape(radius);
	}
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 100.0f; }  // 質量の取得

private:
	static constexpr float RESET_HEIGHT = -480.0f;// リスポーンする高さ
	std::vector<D3DXVECTOR3> m_pathPoints;		// チェックポイントの配列 (代入用)
	int m_currentTargetIndex;					// 今の目標地点インデックス
	float m_speed;								// 力の強さ（速度の代わり）
	bool m_isBridgeSwitchOn;
	bool m_isHit;
	bool m_isPrevHit;
	bool m_isThrough;							// 通過したか
	bool m_isPrevThrough;						// 直前に通過したか
	int m_effectTimer;							// タイマー
	const int ROLL_EFFECT_INTERVAL = 5;			// エフェクト発生間隔（フレーム数）
	int m_playedRollSoundID;					// 再生中の音ID
	D3DXVECTOR3 m_dir;							// 岩の進行方向ベクトル
};

//*****************************************************************************
// シーソーブロッククラス
//*****************************************************************************
class CSeesawBlock : public CBlock
{
public:
	CSeesawBlock();
	~CSeesawBlock();

	HRESULT Init(void);
	void Uninit(void);
	void RemoveHinge(void);
	void Update(void);
	void OnPhysicsCreated(void) override
	{
		// ヒンジの設定処理
		SetHinge();
	}
	void OnPhysicsReleased(void) override
	{
		// ヒンジの破棄
		RemoveHinge();
	}

	void SetHinge(void);
	btHingeConstraint* GetHinge(void) { return m_pHinge; }

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetLinearFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetFriction(void) const { return 5.7f; }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	btScalar GetMass(void) const { return 10.0f; }// 質量の取得

private:
	btHingeConstraint* m_pHinge;
};

#endif

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
	btScalar GetMass(void) const { return 8.0f; }  // 質量の取得

private:
	static constexpr float RESPAWN_HEIGHT = -300.0f;
	D3DXVECTOR3 m_ResPos;	// リスポーン位置
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

//*****************************************************************************
// ギアブロッククラス
//*****************************************************************************
class CGearBlock : public CBlock
{
public:
	CGearBlock();
	~CGearBlock();

	HRESULT Init(void);
	void Update(void);

	// コライダー
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		// half extents（半径と高さの半分）
		btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.x * 0.5f);

		// Y軸方向の円柱
		return new btCylinderShape(halfExtents);
	}

	void SetRotDir(float dir) { m_rotDir = dir; } // 外から方向指定用
	void SetRotSpeed(float fSpeed) { m_rotSpeed = fSpeed; }
	void SetAmplitude(float fAmplitude) { m_MoveAmplitude = fAmplitude; }
	void SetPeriod(float fPeriod) { m_MovePeriod = fPeriod; }

	float GetRotDir(void) const { return m_rotDir; }
	float GetRotSpeed(void) const { return m_rotSpeed; }
	float GetAmplitude(void) const { return m_MoveAmplitude; }
	float GetPeriod(void) const { return m_MovePeriod; }

private:
	D3DXVECTOR3 m_initPos;	// 初期位置
	float m_rotDir;			// 回転方向（+1:正回転 / -1:逆回転）
	float m_rotSpeed;		// 回転スピード
	int m_nMoveCounter;		// 移動カウンター
	float m_MoveAmplitude;	// ±移動幅
	float m_MovePeriod;		// 周期フレーム
};

//*****************************************************************************
// ギア柱ブロッククラス
//*****************************************************************************
class CGearPillarBlock : public CBlock
{
public:
	CGearPillarBlock();
	~CGearPillarBlock();

	void Update(void);

	// コライダー
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		// half extents（半径と高さの半分）
		btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.x * 0.5f);

		// Y軸方向の円柱
		return new btCylinderShape(halfExtents);
	}

private:

};

//*****************************************************************************
// プレスブロッククラス
//*****************************************************************************
class CPressBlock : public CBlock
{
public:
	CPressBlock();
	~CPressBlock();

	// プレス機の状態
	typedef enum
	{
		IDLE = 0,	// 待機
		PRESSING,	// 押し出し
		RETRACTING,	// 戻る
		MAX
	}STATE;

	HRESULT Init(void);
	void Update(void);

private:
	static constexpr float PRESS_DISTANCE	= 180.0f;	// 押し出し距離
	static constexpr float PRESS_SPEED		= 6.0f;		// 移動スピード
	static constexpr float BACK_SPEED		= 3.0f;		// 戻りスピード
	static constexpr float WAIT_TIME		= 60.0f;	// 戻った後の待機時間（フレーム）

	D3DXVECTOR3 m_initPos;	// 初期位置
	float m_offSet;			// オフセットの値
	float m_waitTimer;		// 待機タイマー
	STATE m_state;	// 状態
};

//*****************************************************************************
// プロペラボディブロッククラス
//*****************************************************************************
class CPropellerBodyBlock : public CBlock
{
public:
	CPropellerBodyBlock();
	~CPropellerBodyBlock();

	void Update(void);

	// コライダー
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		// half extents（半径と高さの半分）
		btVector3 halfExtents(size.x * 0.5f, size.x * 0.5f, size.y * 0.2f);

		// Y軸方向の円柱
		return new btCylinderShapeZ(halfExtents);
	}

private:

};

//*****************************************************************************
// プロペラブロッククラス
//*****************************************************************************
class CPropellerWingBlock : public CBlock
{
public:
	CPropellerWingBlock();
	~CPropellerWingBlock();

	void Update(void);

private:

};

//*****************************************************************************
// ベルトコンベアブロッククラス
//*****************************************************************************
class CConveyerBlock : public CBlock
{
public:
	CConveyerBlock();
	~CConveyerBlock();

	void Update();

private:

};

//*****************************************************************************
// 動く床ブロッククラス
//*****************************************************************************
class CMoveBlock : public CBlock
{
public:
	CMoveBlock();
	~CMoveBlock();

	HRESULT Init(void);
	void Update(void);

private:
	D3DXVECTOR3 m_initPos;	// 初期位置
	int m_nMoveCounter;		// 移動カウンター
	float m_MoveAmplitude;	// ±移動幅
	float m_MovePeriod;		// 周期フレーム
};

#endif

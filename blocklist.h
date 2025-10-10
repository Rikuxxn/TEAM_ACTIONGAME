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

	HRESULT Init(void);
	void Update(void);

private:

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

#endif

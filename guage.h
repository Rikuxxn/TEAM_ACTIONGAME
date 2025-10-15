//=============================================================================
//
// ゲージ処理 [guage.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GUAGE_H_// このマクロ定義がされていなかったら
#define _GUAGE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object2D.h"

//*****************************************************************************
// ゲージクラス
//*****************************************************************************
class CGuage : public CObject2D
{
public:
	// 種類
	enum GUAGETYPE
	{
		TYPE_NONE = 0,	// 何もしていない状態
		TYPE_GUAGE,
		TYPE_FRAME,
		TYPE_MAX
	};

	CGuage();
	~CGuage();

	static CGuage* Create(D3DXVECTOR3 pos, GUAGETYPE type);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	GUAGETYPE m_type;							// ゲージの種類
	static constexpr float m_fWidth = 470.0f;	// 横幅
	static constexpr float m_fHeight = 65.0f;	// 縦幅
	static constexpr int m_nMax = 600;			// 最大値
	static int m_nIdxTexture;					// テクスチャインデックス
	int m_nHp;									// HP
	int m_nFrame;
};

#endif
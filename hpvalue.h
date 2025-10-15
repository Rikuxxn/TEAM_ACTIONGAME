//=============================================================================
//
// スコア処理 [hpvalue.h]
// Author : RIKU TANEKAWA → NAGAO YUSEI
//
//=============================================================================
#ifndef _HPVAL_H_// このマクロ定義がされていなかったら
#define _HPVAL_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "number.h"
#include "object.h"


//*****************************************************************************
// スコアクラス
//*****************************************************************************
class CHPValue : public CObject
{
public:
	CHPValue(int nPriority = 7);
	~CHPValue();

	static CHPValue* Create(float baseX, float baseY, float digitWidth, float digitHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	void SetHPValue(int nHPValue);
	static void AddHPValue(int nValue);
	int DigitNum(int nHPValue);

private:
	static const int MAX_DIGITS = 3;		// 桁数
	CNumber* m_apNumber[MAX_DIGITS];		// 各桁の数字表示用
	static int m_nHPValue;					// スコア
	static int m_nDig[MAX_DIGITS];
	float m_digitWidth;						// 数字1桁あたりの幅
	float m_digitHeight;					// 数字1桁あたりの高さ
	D3DXVECTOR3 m_basePos;					// 表示の開始位置
	int m_nIdxTexture;						// テクスチャインデックス

};

#endif
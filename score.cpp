//=============================================================================
//
// スコア処理 [score.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "score.h"
#include "number.h"
#include "renderer.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CScore::m_nScore = 0;
int CScore::m_nDig[MAX_DIGITS] = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CScore::CScore(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		m_apNumber[nCnt] = {};				
	}
	m_nScore	  = 0;						// スコア
	m_digitWidth  = 0.0f;					// 数字1桁あたりの幅
	m_digitHeight = 0.0f;					// 数字1桁あたりの高さ
	m_basePos     = INIT_VEC3;				// 表示の開始位置
	m_nIdxTexture = 0;						// テクスチャインデックス
}
//=============================================================================
// デストラクタ
//=============================================================================
CScore::~CScore()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CScore* CScore::Create(float baseX,float baseY,float digitWidth,float digitHeight)
{
	CScore* pScore;

	pScore = new CScore;

	pScore->m_basePos = D3DXVECTOR3(baseX, baseY, 0.0f);
	pScore->m_digitWidth = digitWidth;
	pScore->m_digitHeight = digitHeight;

	// 初期化処理
	pScore->Init();
	
	return pScore;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CScore::Init(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		float posX = m_basePos.x + nCnt * m_digitWidth;
		float posY = m_basePos.y;

		m_apNumber[nCnt] = CNumber::Create(posX, posY, m_digitWidth, m_digitHeight);

		if (!m_apNumber[nCnt])
		{
			return E_FAIL;
		}
	}

	// テクスチャ割り当て
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->RegisterDynamic("data/TEXTURE/num001.png");

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CScore::Uninit(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt] != nullptr)
		{
			// ナンバーの終了処理
			m_apNumber[nCnt]->Uninit();

			delete m_apNumber[nCnt];
			m_apNumber[nCnt] = nullptr;
		}
	}

	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CScore::Update(void)
{
	// HPの各桁の数字を抽出
	for (int nCount = 0; nCount < MAX_DIGITS; nCount++)
	{
		m_nDig[nCount] = NULL;
	}

	int nHP = m_nScore;
	for (int nCount = 0; nCount < MAX_DIGITS; nCount++)
	{
		int Idx = MAX_DIGITS - 1 - nCount;
		if (Idx >= 0 && Idx < MAX_DIGITS)
		{
			m_nDig[Idx] = nHP % 10;
			if (nHP != 0)
			{
				nHP /= 10;
			}
		}
	}

	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			// ナンバーの更新処理
			m_apNumber[nCnt]->Update();
		}
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CScore::Draw(void)
{
	int score = m_nScore;

	bool bDraw = false;

	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		int digit = m_nDig[nCnt];

		if (digit != 0 || nCnt == MAX_DIGITS - 1 || bDraw)
		{
			bDraw = true;

			if (m_apNumber[nCnt])
			{
				// 桁設定処理
				m_apNumber[nCnt]->SetDigit(digit);

				// テクスチャの取得
				CTexture* pTexture = CManager::GetTexture();

				// デバイスの取得
				CRenderer* renderer = CManager::GetRenderer();
				LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

				// テクスチャの設定
				pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

				m_apNumber[nCnt]->Draw();
			}
		}
	}

	//// スコアを各桁に分解（右から）
	//for (int nCnt = MAX_DIGITS - 1; nCnt >= 0; nCnt--)
	//{
	//	int digit = score % 10;
	//	score /= 10;

	//	if (m_apNumber[nCnt])
	//	{
	//		// 桁設定処理
	//		m_apNumber[nCnt]->SetDigit(digit);

	//		// テクスチャの取得
	//		CTexture* pTexture = CManager::GetTexture();

	//		// デバイスの取得
	//		CRenderer* renderer = CManager::GetRenderer();
	//		LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//		// テクスチャの設定
	//		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	//		m_apNumber[nCnt]->Draw();
	//	}
	//}
}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CScore::GetPos(void)
{
	return D3DXVECTOR3();
}
//=============================================================================
// スコア設定処理
//=============================================================================
void CScore::SetScore(int nScore)
{
	m_nScore = nScore;
}
//=============================================================================
// スコア加算処理
//=============================================================================
void CScore::AddScore(int nValue)
{
	m_nScore += nValue;

	if (m_nScore > 999)
	{
		// 8桁制限
		m_nScore = 999;
	}
}
//=============================================================================
// 桁分割処理
//=============================================================================
int CScore::DigitNum(int nScore)
{
	if (nScore == 0)
	{
		return 1;
	}

	int nCnt = 0;
	while (nScore > 0)
	{
		nScore /= 10;
		nCnt++;
	}

	return nCnt;
}
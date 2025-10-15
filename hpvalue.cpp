//=============================================================================
//
// スコア処理 [hpvalue.cpp]
// Author : RIKU TANEKAWA → NAGAO YUSEI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "hpvalue.h"
#include "number.h"
#include "renderer.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CHPValue::m_nHPValue = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CHPValue::CHPValue(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		m_apNumber[nCnt] = {};
	}
	m_nHPValue = 0;						// スコア
	m_digitWidth = 0.0f;					// 数字1桁あたりの幅
	m_digitHeight = 0.0f;					// 数字1桁あたりの高さ
	m_basePos = INIT_VEC3;				// 表示の開始位置
	m_nIdxTexture = 0;						// テクスチャインデックス
}
//=============================================================================
// デストラクタ
//=============================================================================
CHPValue::~CHPValue()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CHPValue* CHPValue::Create(float baseX, float baseY, float digitWidth, float digitHeight)
{
	CHPValue* pHPValue;

	pHPValue = new CHPValue;

	pHPValue->m_basePos = D3DXVECTOR3(baseX, baseY, 0.0f);
	pHPValue->m_digitWidth = digitWidth;
	pHPValue->m_digitHeight = digitHeight;

	// 初期化処理
	pHPValue->Init();

	return pHPValue;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CHPValue::Init(void)
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
void CHPValue::Uninit(void)
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
void CHPValue::Update(void)
{
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
void CHPValue::Draw(void)
{
	int HPValue = m_nHPValue;
	bool bDraw = false;

	// スコアを各桁に分解（右から）

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

	//for (int nCnt = MAX_DIGITS - 1; nCnt >= 0; nCnt--)
	//{
	//	int digit = HPValue % 10;
	//	HPValue /= 10;

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
D3DXVECTOR3 CHPValue::GetPos(void)
{
	return D3DXVECTOR3();
}
//=============================================================================
// スコア設定処理
//=============================================================================
void CHPValue::SetHPValue(int nHPValue)
{
	m_nHPValue = nHPValue;
}
//=============================================================================
// スコア加算処理
//=============================================================================
void CHPValue::AddHPValue(int nValue)
{
	m_nHPValue += nValue;

	if (m_nHPValue > 999)
	{
		// 3桁制限
		m_nHPValue = 999;
	}
}
//=============================================================================
// 桁分割処理
//=============================================================================
int CHPValue::DigitNum(int nHPValue)
{
	if (nHPValue == 0)
	{
		return 1;
	}

	int nCnt = 0;
	while (nHPValue > 0)
	{
		nHPValue /= 10;
		nCnt++;
	}

	return nCnt;
}
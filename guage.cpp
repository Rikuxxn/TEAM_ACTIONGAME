//=============================================================================
//
// ゲージ処理 [guage.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "guage.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CGuage::m_nIdxTexture = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CGuage::CGuage()
{
	// 値のクリア
	m_nIdxTexture = 0;
	m_type = TYPE_NONE;	// ゲージの種類
	m_nHp = NULL;
	m_nFrame = NULL;
}
//=============================================================================
// デストラクタ
//=============================================================================
CGuage::~CGuage()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CGuage* CGuage::Create(D3DXVECTOR3 pos, GUAGETYPE type)
{
	CGuage* pGuage = nullptr;

	pGuage = new CGuage;

	// 初期化処理
	pGuage->SetPos(pos);
	pGuage->SetSize(m_fWidth, m_fHeight);
	pGuage->SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
	pGuage->m_type = type;
	pGuage->Init();

	return pGuage;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGuage::Init(void)
{
	if (m_type == TYPE_FRAME)
	{
		// テクスチャ設定
		m_nIdxTexture = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/BatteryFrame.png");
	}

	m_nHp = m_nMax;

	// 2Dオブジェクトの初期化
	CObject2D::Init();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGuage::Uninit(void)
{
	// 2Dオブジェクトの終了処理
	CObject2D::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CGuage::Update(void)
{
	if (m_type == TYPE_GUAGE)
	{
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得
		float f, f2, f3, f4;
		f = (float)m_nHp / (float)m_nMax;
#if 0
		f2 = f / 0.1f;
		f3 = f2;
#endif
#if 1
		int n = f / 0.1f;
		f3 = (float)n + 1;
		//f3 = (float)n;
#endif
		f4 = f3 / 10;

#if _DEBUG
		if (pKeyboard->GetTrigger(DIK_1) == true)
		{
			m_nHp = m_nMax;
		}

		if (pKeyboard->GetTrigger(DIK_2) == true)
		{
			m_nHp -= 50;
		}

		if (pKeyboard->GetPress(DIK_3) == true)
		{
			if (m_nHp <= m_nMax)
			{
				m_nHp += 3;
			}
		}
#endif
		if (m_nHp > 0)
		{
			m_nHp--;
		}

		// 残りHPに応じて色設定
		if (m_nHp <= (int)(m_nMax * 0.3f))
		{// のこりHP30%以下
			SetCol(D3DCOLOR_RGBA(255, 0, 0, 255));
		}
		else if (m_nHp > (int)(m_nMax * 0.3f))
		{// のこりHP30%以上
			SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
		}

		// 2Dオブジェクトの更新処理
		CObject2D::UpdateUI(f4);
	}
	else if (m_type == TYPE_FRAME)
	{
		// 2Dオブジェクトの更新処理
		CObject2D::UpdateFrame();
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CGuage::Draw(void)
{
	if (m_type == TYPE_FRAME)
	{
		// テクスチャの取得
		CTexture* pTexture = CManager::GetTexture();

		// デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));
	}

	// 2Dオブジェクトの描画処理
	CObject2D::Draw();
}
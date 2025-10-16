//=============================================================================
//
// �u���b�N�}�l�[�W���[���� [blockmanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "blockmanager.h"
#include "json.hpp"
#include "FileDialogUtils.h"
#include "manager.h"
#include "imgui_internal.h"
#include "raycast.h"
#include "game.h"
#include "blocklist.h"
#include "guiInfo.h"

// JSON�̎g�p
using json = nlohmann::json;

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::vector<CBlock*> CBlockManager::m_blocks = {};	// �u���b�N�̏��
int CBlockManager::m_selectedIdx = 0;				// �I�𒆂̃C���f�b�N�X
CBlock* CBlockManager::m_draggingBlock = {};		// �h���b�O���̃u���b�N
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_FilePathMap; 
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_texFilePathMap;
CBlock* CBlockManager::m_selectedBlock = {};// �I�������u���b�N

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CBlockManager::CBlockManager()
{
	// �l�̃N���A
	m_selectedBlock				= nullptr;
	m_prevSelectedIdx			= -1;
	m_hasConsumedPayload		= false;
	m_pDebug3D					= nullptr;// 3D�f�o�b�O�\���ւ̃|�C���^
	m_autoUpdateColliderSize	= true;
	m_texIDs					= {};
	m_isDragging				= false;// �h���b�O�����ǂ���
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CBlockManager::~CBlockManager()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CBlock* CBlockManager::CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos)
{
	const char* path = CBlockManager::GetFilePathFromType(type);

	CBlock* newBlock = CBlock::Create(path, pos, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), type);

	if (newBlock)
	{
		m_blocks.push_back(newBlock);
	}

	return newBlock;
}
//=============================================================================
// ����������
//=============================================================================
void CBlockManager::Init(void)
{
	LoadConfig("data/block_payload.json");

	m_texIDs.resize(CBlock::TYPE_MAX);

	// GUI�e�N�X�`���̓o�^
	for (int typeInt = 0; typeInt < (int)CBlock::TYPE_MAX; typeInt++)
	{
		CBlock::TYPE type = static_cast<CBlock::TYPE>(typeInt);
		const char* texPath = CBlockManager::GetTexPathFromType(type);

		if (texPath)
		{
			m_texIDs[typeInt] = CManager::GetTexture()->RegisterDynamic(texPath);
		}
		else
		{
			m_texIDs[typeInt] = -1; // ��������
		}
	}

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();
}
//=============================================================================
// �I������
//=============================================================================
void CBlockManager::Uninit(void)
{
	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();
}
//=============================================================================
// �폜�\�񂪂���u���b�N�̍폜����
//=============================================================================
void CBlockManager::CleanupDeadBlocks(void)
{
	for (int i = (int)m_blocks.size() - 1; i >= 0; i--)
	{
		if (m_blocks[i]->IsDead())
		{
			// �u���b�N�̏I������
			m_blocks[i]->Uninit();
			m_blocks.erase(m_blocks.begin() + i);
		}
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CBlockManager::Update(void)
{
#ifdef _DEBUG

	// ���̍X�V
	UpdateInfo();

#endif
	// �u���b�N�폜����
	CleanupDeadBlocks();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CBlockManager::Draw(void)
{
#ifdef _DEBUG
	//// �I�𒆂̃u���b�N�����R���C�_�[�`��
	//CBlock* pSelectBlock = GetSelectedBlock();
	//if (pSelectBlock != nullptr)
	//{
	//	pSelectBlock->DrawCollider();
	//}
#endif
}
//=============================================================================
// ���̍X�V����
//=============================================================================
void CBlockManager::UpdateInfo(void)
{
	// GUI�X�^�C���̎擾
	ImGuiStyle& style = ImGui::GetStyle();

	style.GrabRounding		= 10.0f;		// �X���C�_�[�̂܂݂��ۂ�
	style.ScrollbarRounding = 10.0f;		// �X�N���[���o�[�̊p
	style.ChildRounding		= 10.0f;		// �q�E�B���h�E�̊p
	style.WindowRounding	= 10.0f;		// �E�B���h�E�S�̂̊p

	// �ꏊ
	CImGuiManager::Instance().SetPosImgui(ImVec2(1900.0f, 20.0f));

	// �T�C�Y
	CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 500.0f));

	// �ŏ���GUI
	CImGuiManager::Instance().StartImgui(u8"BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// �u���b�N���Ȃ��ꍇ
	if (m_blocks.empty())
	{
		ImGui::Text("No blocks placed yet.");
	}
	else
	{
		// �u���b�N�̑���
		ImGui::Text("Block Num %d", (int)m_blocks.size());

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// �C���f�b�N�X�I��
		ImGui::SliderInt("Block Index", &m_selectedIdx, 0, (int)m_blocks.size() - 1);

		// �͈͊O�΍�
		if (m_selectedIdx >= (int)m_blocks.size())
		{
			m_selectedIdx = (int)m_blocks.size() - 1;
		}

		// �O��I��ł��u���b�N������
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != m_selectedIdx)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// �Ώۃu���b�N�̎擾
		m_selectedBlock = m_blocks[m_selectedIdx];

		// �u���b�N���̒�������
		UpdateTransform(m_selectedBlock);

		if (GetUpdateCollider() == false)
		{
			// �R���C�_�[�̒�������
			UpdateCollider(m_selectedBlock);
		}
		else
		{
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�
			ImGui::Text("Auto Update Collider Size is Active");
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �u���b�N�^�C�v�ꗗ
	if (ImGui::TreeNode("Block Types"))
	{
		ImGui::BeginChild("BlockTypeList", ImVec2(0, 500), true); // �X�N���[���̈�

		const float imageSize = 100.0f; // �傫�߉摜
		int numTypes = (int)CBlock::TYPE_MAX;

		for (int i = 0; i < numTypes; i++)
		{
			if (i >= static_cast<int>(m_texIDs.size()))
			{
				continue;
			}

			int texIdx = m_texIDs[i];
			LPDIRECT3DTEXTURE9 tex = CManager::GetTexture()->GetAddress(texIdx);

			if (!tex)
			{
				continue; // nullptr �̓X�L�b�v
			}

			ImGui::PushID(i);
			ImGui::Image(reinterpret_cast<ImTextureID>(tex), ImVec2(imageSize, imageSize));

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				m_isDragging = true;// �h���b�O��
				CBlock::TYPE payloadType = static_cast<CBlock::TYPE>(i);
				ImGui::SetDragDropPayload("BLOCK_TYPE", &payloadType, sizeof(payloadType));
				ImGui::Text("Block Type %d", i);
				ImGui::Image(reinterpret_cast<ImTextureID>(tex), ImVec2(imageSize, imageSize));
				ImGui::EndDragDropSource();
			}

			// �}�E�X�̎擾
			CInputMouse* pMouse = CManager::GetInputMouse();

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_isDragging)
			{
				// ���݂�ImGui�̓�����ԁi�R���e�L�X�g�j�ւ̃|�C���^�[���擾
				ImGuiContext* ctx = ImGui::GetCurrentContext();

				if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
				{
					// �h���b�O��������
					m_isDragging = false;

					CBlock::TYPE draggedType = *(CBlock::TYPE*)ctx->DragDropPayload.Data;

					D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
					pos.y = 30.0f;

					// �u���b�N�̐���
					m_draggingBlock = CreateBlock(draggedType, pos);
				}
			}

			ImGui::PopID();

			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �摜�Ԃ̗]��
		}

		ImGui::EndChild();
		ImGui::TreePop();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	if (ImGui::Button("Save"))
	{
		// �_�C�A���O���J���ăt�@�C���ɕۑ�
		std::string path = OpenWindowsSaveFileDialog();

		if (!path.empty())
		{
			// �f�[�^�̕ۑ�
			CBlockManager::SaveToJson(path.c_str());
		}
	}

	ImGui::SameLine(0);

	if (ImGui::Button("Load"))
	{
		// �_�C�A���O���J���ăt�@�C�����J��
		std::string path = OpenWindowsOpenFileDialog();

		if (!path.empty())
		{
			// �f�[�^�̓ǂݍ���
			CBlockManager::LoadFromJson(path.c_str());
		}
	}

	ImGui::End();

	if (CManager::GetMode() == MODE_TITLE)
	{
		return;
	}

	// �}�E�X�I������
	PickBlockFromMouseClick();
}
//=============================================================================
// �u���b�N���̒�������
//=============================================================================
void CBlockManager::UpdateTransform(CBlock* selectedBlock)
{
	if (selectedBlock)
	{
		// �I�𒆂̃u���b�N�̐F��ς���
		selectedBlock->SetSelected(true);

		D3DXVECTOR3 pos = selectedBlock->GetPos();	// �I�𒆂̃u���b�N�̈ʒu�̎擾
		D3DXVECTOR3 rot = selectedBlock->GetRot();	// �I�𒆂̃u���b�N�̌����̎擾
		D3DXVECTOR3 size = selectedBlock->GetSize();// �I�𒆂̃u���b�N�̃T�C�Y�̎擾
		btScalar mass = selectedBlock->GetMass();	// �I�𒆂̃u���b�N�̎��ʂ̎擾

		// ���W�A�����p�x�Ɉꎞ�ϊ��i�ÓI�ϐ��ŕێ������쒆�̂ݍX�V�j
		static D3DXVECTOR3 degRot = D3DXToDegree(rot);
		static bool m_initializedDegRot = false;

		if (!m_initializedDegRot)
		{
			degRot = D3DXToDegree(rot);
			m_initializedDegRot = true;
		}

		bool isEditMode = selectedBlock->IsEditMode();

		ImGui::Checkbox("Kinematic", &isEditMode);

		if (isEditMode)
		{
			selectedBlock->SetEditMode(true);  // �`�F�b�N��Kinematic��
		}
		else
		{
			selectedBlock->SetEditMode(false); // �ʏ�ɖ߂�
		}

		//*********************************************************************
		// POS �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("POS"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_x", &pos.x, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_y", &pos.y, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_z", &pos.z, 1.0f, -9000.0f, 9000.0f, "%.1f");

		//*********************************************************************
		// ROT �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("ROT"); ImGui::SameLine(60);

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedX = ImGui::DragFloat("##Block_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedY = ImGui::DragFloat("##Block_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedZ = ImGui::DragFloat("##Block_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

		bool isRotChanged = changedX || changedY || changedZ;
		
		//*********************************************************************
		// SIZE �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("SIZE"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_x", &size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_y", &size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_z", &size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		// �`�F�b�N�{�b�N�X�F�g�嗦�ɉ����Ď����X�V���邩
		ImGui::Checkbox("Auto Update Collider Size", &m_autoUpdateColliderSize);

		// �O��̃T�C�Y��ێ�
		static D3DXVECTOR3 prevSize = selectedBlock->GetSize();

		// �T�C�Y�ύX�`�F�b�N
		bool isSizeChanged = (size != prevSize);

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		//*********************************************************************
		// �M�A�E�������̐ݒ�
		//*********************************************************************
		if (auto gear = dynamic_cast<CGearBlock*>(selectedBlock))
		{
			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			float rotateDir = gear->GetRotDir();

			// ���W�I�{�^���`���i�ǂ��炩��������I���j
			ImGui::Text("Rotate Direction:");

			ImGui::SameLine();

			if (ImGui::RadioButton("+", rotateDir > 0.0f))
			{
				rotateDir = 1.0f;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("-", rotateDir < 0.0f))
			{
				rotateDir = -1.0f;
			}

			gear->SetRotDir(rotateDir);

			// �X�s�[�h����
			float speed = gear->GetRotSpeed();
			ImGui::Text("Rot Speed:");
			ImGui::SameLine();
			ImGui::DragFloat("##GearRotateSpeed", &speed, 0.01f, 0.0f, 1.0f, "%.2f");

			// �ړ�������
			float Amplitude = gear->GetAmplitude();
			ImGui::Text("Amplitude:");
			ImGui::SameLine();
			ImGui::DragFloat("##GearAmplitude", &Amplitude, 1.0f, 0.0f, 300.0f, "%.1f");

			// ��������
			float Period = gear->GetPeriod();
			ImGui::Text("Period:");
			ImGui::SameLine();
			ImGui::DragFloat("##GearPeriod", &Period, 1.0f, 0.0f, 400.0f, "%.1f");

			// ��]�X�s�[�h�̐ݒ�
			gear->SetRotSpeed(speed);

			// �ړ����̐ݒ�
			gear->SetAmplitude(Amplitude);

			// �����̐ݒ�
			gear->SetPeriod(Period);
		}

		//*********************************************************************
		// ���� �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("MASS"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// �X���C�_�[�i�͈�: 0.0f �` 80.0f�j
		ImGui::SliderFloat("##MassSlider", &mass, 0.0f, 80.0f, "%.2f");


		// �p�x�����W�A���ɖ߂�
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		// �ʒu�̐ݒ�
		selectedBlock->SetPos(pos);

		// �T�C�Y�̐ݒ�
		selectedBlock->SetSize(size);

		// �T�C�Y(�g�嗦)���ς�����Ƃ������Ă�
		if (m_autoUpdateColliderSize == true && isSizeChanged)
		{
			// �u���b�N�T�C�Y�ɂ��R���C�_�[�̐���
			selectedBlock->CreatePhysicsFromScale(size);

			prevSize = size; // �X�V
		}

		if (isRotChanged)
		{
			// ��]���ς�����������Z�b�g
			selectedBlock->SetRot(rotRad);

			// �ҏW���[�h�Ȃ瑦����Transform���X�V���ē���
			if (isEditMode)
			{
				btTransform transform;
				transform.setIdentity();

				btVector3 btPos(pos.x + selectedBlock->GetColliderOffset().x,
					pos.y + selectedBlock->GetColliderOffset().y,
					pos.z + selectedBlock->GetColliderOffset().z);
				transform.setOrigin(btPos);

				D3DXMATRIX matRot;
				D3DXMatrixRotationYawPitchRoll(&matRot, rotRad.y, rotRad.x, rotRad.z);

				D3DXQUATERNION dq;
				D3DXQuaternionRotationMatrix(&dq, &matRot);

				btQuaternion btRot(dq.x, dq.y, dq.z, dq.w);
				transform.setRotation(btRot);

				if (!selectedBlock->GetRigidBody())
				{
					return;
				}
				selectedBlock->GetRigidBody()->setWorldTransform(transform);
				selectedBlock->GetRigidBody()->getMotionState()->setWorldTransform(transform);
			}

			// �V�[�\�[�u���b�N�Ȃ�q���W�����X�V
			if (auto seesaw = dynamic_cast<CSeesawBlock*>(selectedBlock))
			{
				seesaw->RemoveHinge();  // �����q���W�폜
				seesaw->SetHinge();     // �V������]�ɍ��킹�ăq���W�쐬
			}
		}
		else
		{
			// �ҏW���Ă��Ȃ�����degRot��selectedBlock�̒l�ɓ������Ă���
			degRot = D3DXToDegree(selectedBlock->GetRot());
		}

		//*********************************************************************
		// �u���b�N�̍폜
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		if (ImGui::Button("Delete"))
		{
			if (m_autoUpdateColliderSize)
			{
				if (m_blocks[m_selectedIdx])
				{
					// �I�𒆂̃u���b�N���폜
					m_blocks[m_selectedIdx]->Uninit();
				}

				m_blocks.erase(m_blocks.begin() + m_selectedIdx);

				// �I���C���f�b�N�X�𒲐�
				if (m_selectedIdx >= (int)m_blocks.size())
				{
					m_selectedIdx = (int)m_blocks.size() - 1;
				}

				m_prevSelectedIdx = -1;
			}
			else
			{// m_autoUpdateColliderSize��false�̎��͉������Ȃ�
				
			}
		}

		ImGui::PopStyleColor(3);
	}

	// �Ō�ɕۑ�
	m_prevSelectedIdx = m_selectedIdx;
}
//=============================================================================
// �u���b�N�I������
//=============================================================================
void CBlockManager::PickBlockFromMouseClick(void)
{
	// ImGui���}�E�X���g���Ă�Ȃ�I���������L�����Z��
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	// ���N���b�N�̂�
	if (!CManager::GetInputMouse()->GetTrigger(0))
	{
		return;
	}

	// ���C�擾�iCRayCast���g�p�j
	D3DXVECTOR3 rayOrigin, rayDir;
	CRayCast::GetMouseRay(rayOrigin, rayDir);

	float minDist = FLT_MAX;
	int hitIndex = -1;

	for (size_t i = 0; i < m_blocks.size(); ++i)
	{
		CBlock* block = m_blocks[i];

		// ���[���h�s��̎擾�i�ʒu�E��]�E�g����܂ށj
		D3DXMATRIX world = block->GetWorldMatrix();

		D3DXVECTOR3 modelSize = block->GetModelSize();
		D3DXVECTOR3 scale = block->GetSize();

		D3DXVECTOR3 halfSize;
		halfSize.x = modelSize.x * 0.5f;
		halfSize.y = modelSize.y * 0.5f;
		halfSize.z = modelSize.z * 0.5f;

		float dist = 0.0f;
		if (CRayCast::IntersectOBB(rayOrigin, rayDir, world, halfSize, dist))
		{
			if (dist < minDist)
			{
				minDist = dist;
				hitIndex = i;
			}
		}
	}

	// �I����Ԃ𔽉f
	if (hitIndex >= 0)
	{
		// �ȑO�I�΂�Ă����u���b�N���I����
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != hitIndex)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// �V�����I��
		m_selectedIdx = hitIndex;
		m_blocks[m_selectedIdx]->SetSelected(true);
		m_prevSelectedIdx = hitIndex;
	}
}
//=============================================================================
// �R���C�_�[�̒�������
//=============================================================================
void CBlockManager::UpdateCollider(CBlock* selectedBlock)
{
	// �P��R���C�_�[�p
	D3DXVECTOR3 colliderSize = selectedBlock->GetColliderSize();
	static D3DXVECTOR3 prevSize = colliderSize;

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("COLLIDER SIZE");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	bool changed = false;

	ImGui::Text("X:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	changed |= ImGui::DragFloat("##collider_size_x", &colliderSize.x, 0.1f, 0.1f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Y:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	changed |= ImGui::DragFloat("##collider_size_y", &colliderSize.y, 0.1f, 0.1f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Z:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	changed |= ImGui::DragFloat("##collider_size_z", &colliderSize.z, 0.1f, 0.1f, 800.0f, "%.1f");

	if (changed && colliderSize != prevSize)
	{
		colliderSize.x = std::max(colliderSize.x, 0.01f);
		colliderSize.y = std::max(colliderSize.y, 0.01f);
		colliderSize.z = std::max(colliderSize.z, 0.01f);

		selectedBlock->SetColliderManual(colliderSize);

		prevSize = colliderSize;
	}

	D3DXVECTOR3 offset = selectedBlock->GetColliderOffset();
	static D3DXVECTOR3 prevOffset = offset;

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("COLLIDER OFFSET");

	bool offsetChanged = false;

	ImGui::Text("X:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	offsetChanged |= ImGui::DragFloat("##collider_offset_x", &offset.x, 0.1f, -800.0f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Y:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	offsetChanged |= ImGui::DragFloat("##collider_offset_y", &offset.y, 0.1f, -800.0f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Z:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	offsetChanged |= ImGui::DragFloat("##collider_offset_z", &offset.z, 0.1f, -800.0f, 800.0f, "%.1f");

	if (offsetChanged && offset != prevOffset)
	{
		selectedBlock->SetColliderOffset(offset);
		selectedBlock->CreatePhysics(selectedBlock->GetPos(), selectedBlock->GetColliderSize());

		prevOffset = offset;
	}
}
//=============================================================================
// X�t�@�C���p�X�̓ǂݍ���
//=============================================================================
void CBlockManager::LoadConfig(const std::string& filename)
{
	std::ifstream ifs(filename);
	if (!ifs)
	{
		return;
	}

	json j;
	ifs >> j;

	// j �͔z��ɂȂ��Ă�̂Ń��[�v����
	for (auto& block : j)
	{
		int typeInt = block["type"];
		std::string filepath = block["filepath"];
		std::string texFilepath = block["GUItexFilepath"];

		s_FilePathMap[(CBlock::TYPE)typeInt] = filepath;
		s_texFilePathMap[(CBlock::TYPE)typeInt] = texFilepath;
	}
}
//=============================================================================
// �^�C�v����X�t�@�C���p�X���擾
//=============================================================================
const char* CBlockManager::GetFilePathFromType(CBlock::TYPE type)
{
	auto it = s_FilePathMap.find(type);
	return (it != s_FilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// �^�C�v����GUI�e�N�X�`���t�@�C���p�X���擾
//=============================================================================
const char* CBlockManager::GetTexPathFromType(CBlock::TYPE type)
{
	auto it = s_texFilePathMap.find(type);
	return (it != s_texFilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// �u���b�N���̕ۑ�����
//=============================================================================
void CBlockManager::SaveToJson(const char* filename)
{
	// JSON�I�u�W�F�N�g
	json j;

	// 1�Â�JSON��
	for (const auto& block : m_blocks)
	{
		// ���W�A�����p�x�Ɉꎞ�ϊ�
		D3DXVECTOR3 degRot = D3DXToDegree(block->GetRot());
		
		json b;
		b["type"] = block->GetType();												// �u���b�N�̃^�C�v
		b["pos"] = { block->GetPos().x, block->GetPos().y, block->GetPos().z };		// �ʒu
		b["rot"] = { degRot.x, degRot.y, degRot.z };								// ����
		b["size"] = { block->GetSize().x, block->GetSize().y, block->GetSize().z };	// �T�C�Y

		b["collider_size"] =
		{
			block->GetColliderSize().x,
			block->GetColliderSize().y,
			block->GetColliderSize().z
		};

		// �M�A��p�f�[�^��ۑ�
		if (block->GetType() == CBlock::TYPE_GEAR_BODY)
		{
			CGearBlock* gear = dynamic_cast<CGearBlock*>(block);
			if (gear)
			{
				b["rotation_speed"] = gear->GetRotSpeed();  // ��]���x
				b["rotation_dir"] = gear->GetRotDir();      // ��]����
				b["amplitude"] = gear->GetAmplitude();		// �ړ���
				b["period"] = gear->GetPeriod();			// ����
			}
		}

		// �ǉ�
		j.push_back(b);
	}

	// �o�̓t�@�C���X�g���[��
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// �t�@�C�������
		file.close();
	}
}
//=============================================================================
// �u���b�N���̓ǂݍ��ݏ���
//=============================================================================
void CBlockManager::LoadFromJson(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// �J���Ȃ�����
		return;
	}

	json j;
	file >> j;

	// �t�@�C�������
	file.close();

	// �����̃u���b�N������
	for (auto block : m_blocks)
	{
		if (block != nullptr)
		{
			// �u���b�N�̏I������
			block->Uninit();
		}
	}

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();

	// �V���ɐ���
	for (const auto& b : j)
	{
		// �^�C�v�����擾���ė񋓌^�ɃL���X�g
		CBlock::TYPE typeInt = b["type"];
		CBlock::TYPE type = typeInt;

		D3DXVECTOR3 pos(b["pos"][0], b["pos"][1], b["pos"][2]);
		D3DXVECTOR3 degRot(b["rot"][0], b["rot"][1], b["rot"][2]);
		D3DXVECTOR3 size(b["size"][0], b["size"][1], b["size"][2]);

		D3DXVECTOR3 rot = D3DXToRadian(degRot); // �x�����W�A���ɕϊ�

		// �^�C�v����u���b�N����
		CBlock* block = CreateBlock(type, pos);

		if (!block)
		{
			continue;
		}

		block->SetRot(rot);
		block->SetSize(size);

		if (b.contains("collider_size"))
		{
			D3DXVECTOR3 colliderSize(
				b["collider_size"][0],
				b["collider_size"][1],
				b["collider_size"][2]);

			block->SetColliderSize(colliderSize);
			block->UpdateCollider(); // �P��p�Đ���
		}

		// �M�A��p�p�����[�^�𕜌�
		if (type == CBlock::TYPE_GEAR_BODY)
		{
			CGearBlock* gear = dynamic_cast<CGearBlock*>(block);
			if (gear)
			{
				if (b.contains("rotation_speed"))
				{
					gear->SetRotSpeed(b["rotation_speed"]);
				}
				if (b.contains("rotation_dir"))
				{
					gear->SetRotDir(b["rotation_dir"]);
				}
				if (b.contains("amplitude"))
				{
					gear->SetAmplitude(b["amplitude"]);
				}
				if (b.contains("period"))
				{
					gear->SetPeriod(b["period"]);
				}
			}
		}
	}
}
//=============================================================================
// �S�u���b�N�̎擾
//=============================================================================
std::vector<CBlock*>& CBlockManager::GetAllBlocks(void)
{
	return m_blocks;
}
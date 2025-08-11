#include "Rabbit.h"
#include "map.h"
#include "screens.h"

//--------------------------------------------------

char Rabbit[14][RabbitY][RabbitX] =
{
	{
		"        (\\(\\",
		"        ('-')",
		"       o(��--|====>"
	}, // ��� ������ �⺻
	{
		"         /)/)",
		"        ('-')",
		"  <====|--��)o"
	}, // ��� ���� �⺻
	{
		"        (\\(\\ ",
		"        ('-')",
		"       o(����--|====>"
	}, // ��� ������ ����
	{
		"         /)/) ",
		"        ('-') ",
		"<====|--����)o"
	}, // ��� ���� ����
	{
		"        (\\(\\ ",
		"        ('-')",
		"       o(��-|=>"
	}, // �ܰ� ������ �⺻
	{
		"         /)/) ",
		"        ('-') ",
		"      <=|-��)o"
	}, // �ܰ� ���� �⺻
	{
		"        (\\(\\ ",
		"        ('-')",
		"       o(����-|=>"
	}, // �ܰ� ������ ����
	{
		"         /)/) ",
		"        ('-') ",
		"    <=|-����)o"
	}, // �ܰ� ���� ����
	{
		"        (\\(\\ ",
		"        ('-')",
		"       o(��------>"
	}, // â ������ �⺻
	{
		"         /)/) ",
		"        ('-') ",
		"   <------��)o"
	}, // â ���� �⺻
	{
		"        (\\(\\ ",
		"        ('-')",
		"       o(����------>"
	}, // â ������ ����
	{
		"         /)/) ",
		"        ('-') ",
		" <------����)o"
	}, // â ���� ����
	{
		"        (\\(\\",
		"        ('-')",
		"       o(   )"
	}, // �÷��̾� ������
	{
		"         /)/) ",
		"        ('-') ",
		"        (   )o"
	}, // �÷��̾� ����
};

//�ִϸ��̼� ���� ����
int animFrame;         // 0 ~ jumpFrames*2-1 (up + down)
int animRepeatCount;   // �ִϸ��̼� �ݺ� Ƚ��
int jumpFrames;  // �ö󰡴� ������ ����
int maxRepeats;  // �ִ� �ݺ� Ƚ��
int centerX;	// �ִϸ��̼� �߽� X ��ǥ
int baseY;	// �ִϸ��̼� ���� Y ��ǥ (���� ���� ��ġ)
int jumpHeight; // ���� ���� (y ����)
int animFramesTotal; // ��ü �ִϸ��̼� ���� (up+down)

int PrevPlayerHealth; // ���� �÷��̾� ü�� (�ǰ� ���� �Ǵܿ�)

SpeedBuff speedBuffs; // �ӵ� ����
SpeedBuff slowDebuffs; // �ӵ� �����

DWORD g_SKeyLastTime = 0; // SŰ ������ �Է� �ð�

bool animGoingUp = true;  // ���� �� �ö󰡴��� ����

bool isNearItem = false;    // �÷��̾� �ֺ��� �������� ������ true

bool stageClear = false; // �������� Ŭ���� ����

bool g_KeyW = false; // WŰ �Է� ����
bool g_KeyA = false; // AŰ �Է� ����
bool g_KeyD = false; // DŰ �Է� ����
bool g_KeyS = false; // SŰ �Է� ����
bool g_KeySpace = false; // �����̽��� �Է� ����

bool IsMapEnd = false; // �� ���� �����ߴ��� ����

bool halfHealth = false; // ü�� ��ĭ


bool IsDamaged = false; // �÷��̾ �ǰݴ��ߴ��� ����

bool IsNearLadder = false; // �÷��̾ ��ٸ� ��ó�� �ִ��� ����

bool IsInvincibleTime = false; // �÷��̾� ���� �ð� ����

// --------------------------------------------------

bool SetIsNearLadder(bool src)
{
	IsNearLadder = src;
}

bool SetInvincibleTime(bool src)
{
	IsInvincibleTime = src;
}

bool SetMapEnd(bool src)
{
	IsMapEnd = src;
}

bool GetKeyW()
{
	return g_KeyW;
}

bool GetKeyA()
{
	return g_KeyA;
}

bool GetKeyD()
{
	return g_KeyD;
}

bool GetKeyS()
{
	return g_KeyS;
}

bool IsNearItem()
{
	return isNearItem;
}

void SetIsNearItem(bool src)
{
	isNearItem = src;
}

bool StageClear()
{
	return stageClear;
}

// �÷��̾�� ������Ʈ �浹 üũ
bool IsOverlap(Rect player, Rect Obj)
{
	return (player.x < Obj.x + Obj.w) && (player.x + player.w > Obj.x) &&
		(player.y < Obj.y + Obj.h) && (player.y + player.h > Obj.y);
}

// �÷��̾��� �浹 ���� ��ȯ
Rect GetPlayerRect()
{
	return (Rect) { player.Pos.x + 8, player.Pos.y, 4, 3 };
}

// �������� �浹 ���� ��ȯ
Rect GetItemRect(Item item)
{
	if (item.type == E_ITEM_DEBUFF) { return (Rect) { item.x + 1 - GetPlusX(), item.y, item.width - 3, item.height }; }
	return (Rect) { item.x - 4 - GetPlusX(), item.y, item.width + 6, item.height };
}

// ���� �浹 ���� ��ȯ
Rect GetWeaponRect()
{
	int tempX = player.Pos.x;
	// �÷��̾ ������ ���� ���� ��
	if (player.Direction == E_Right) {
		switch (GetSelectedIndex())
		{
		case 0: // ���
			return (Rect) { tempX + 14, player.Pos.y + 2, 8, 1 };
		case 1: // �ܰ�
			return (Rect) { tempX + 14, player.Pos.y + 2, 4, 1 };
		case 2: // â
			return (Rect) { tempX + 14, player.Pos.y + 2, 7, 1 };
		}
	}
	// �÷��̾ ���� ���� ���� ��
	else if (player.Direction == E_Left) {
		switch (GetSelectedIndex())
		{
		case 0: // ���
			return (Rect) { tempX, player.Pos.y + 2, 8, 1 };
		case 1: // �ܰ�
			return (Rect) { tempX + 4, player.Pos.y + 2, 4, 1 };
		case 2: // â
			return (Rect) { tempX + 1, player.Pos.y + 2, 7, 1 };
		}
	}
}

// ������ �Ծ����� üũ
void CheckItemPickup()
{
	// �÷��̾� �浹 ���� �޾ƿ���
	Rect playerRect = GetPlayerRect();

	for (int i = 0; i < g_ItemIdx; i++)
	{
		// ȭ�鿡 ������ �ʴ� ������(�̹� �Ծ��ų� �ٸ� ��������)�� ��� �Ѿ��
		if (g_ItemList[i].isHeld) continue;

		// ������ �浹 ���� �޾ƿ���
		Rect itemRect = GetItemRect(g_ItemList[i]);

		// �÷��̾�� ������ �浹 üũ, �浹��
		if (IsOverlap(playerRect, itemRect))
		{
			// ���� ������ Ÿ�Կ� ����
			switch (g_ItemList[i].type) {
			case E_ITEM_LIFE: // ��� �߰� (����)

				SetIsNearItem(true);  // �������

				// e�� ������ ��
				if (GetAsyncKeyState('E') & 0x8000)
				{
					g_ItemList[i].isHeld = true;  // ȭ�鿡 �� ���̰� ó��
					player.Health += LIFEUP;	// ü�� ����

					PrevPlayerHealth = player.Health; //ü�� ����

				}
				break;
			case E_ITEM_SPEED: // �̵��ӵ� ���� (������)

				SetIsNearItem(true);  // �������

				// e�� ������ ��
				if (GetAsyncKeyState('E') & 0x8000)
				{
					g_ItemList[i].isHeld = true;  // ȭ�鿡 �� ���̰� ó��

					if (!speedBuffs.active)
					{
						speedBuffs.active = true;
						speedBuffs.endTime = GetTickCount();
						player.Speed += SPEEDUP; // ��� ����
					}

					speedBuffs.endTime += DURATION;  // ȿ�� ���� �� �ٽ� �Ծ��� ��� �ð� �ʱ�ȭ

				}
				break;
			case E_ITEM_DEBUFF:	// ����� (����)
				g_ItemList[i].isHeld = true;  // ȭ�鿡 �� ���̰� ó��

				if (!slowDebuffs.active)
				{
					slowDebuffs.active = true;
					slowDebuffs.endTime = GetTickCount();
					player.Speed -= SPEEDDOWN;	// �̵� �ӵ��� ���� X
				}

				player.Health -= LIFEDOWN;	// ü���� �������� ���
				slowDebuffs.endTime += DURATION;  // ȿ�� ���� �� �ٽ� �Ծ��� ��� �ð� �ʱ�ȭ

			}
		}
	}
}

// �ӵ� ���� ���ӽð� üũ �� ���� ó��
void UpdateBuffs()
{
	if (speedBuffs.active && GetTickCount() >= speedBuffs.endTime)
	{
		player.Speed -= SPEEDUP; // ������� ����
		speedBuffs.active = false;
	}

	if (slowDebuffs.active && GetTickCount() >= slowDebuffs.endTime)
	{
		player.Speed += SPEEDDOWN;	// ������� ����
		slowDebuffs.active = false;
	}
}

// ����, ����� ������ �ܿ� �ð� ���
void DrawBuffNDebuff() {
	char buf[32];
	DWORD now = GetTickCount();
	if (speedBuffs.active && now < speedBuffs.endTime) {
		if (slowDebuffs.active && now < slowDebuffs.endTime) {
			_DrawText(59, 2, "�ӵ� ����: ");
			snprintf(buf, sizeof(buf), "%.1fs", (float)(speedBuffs.endTime - now) / 1000); // ms �� �� ��ȯ
			_DrawText(71, 2, buf);
			_DrawText(59, 3, "�ӵ� �����: ");
			snprintf(buf, sizeof(buf), "%.1fs", (float)(slowDebuffs.endTime - now) / 1000);
			_DrawText(73, 3, buf);
		}
		else {
			_DrawText(59, 2, "�ӵ� ����: ");
			snprintf(buf, sizeof(buf), "%.1fs", (float)(speedBuffs.endTime - now) / 1000); // ms �� �� ��ȯ
			_DrawText(71, 2, buf);
		}
	}
	else {
		if (slowDebuffs.active && now < slowDebuffs.endTime) {
			_DrawText(59, 2, "�ӵ� �����: ");
			snprintf(buf, sizeof(buf), "%.1fs", (float)(slowDebuffs.endTime - now) / 1000);
			_DrawText(73, 2, buf);
		}
	}
}

// �䳢 �ִϸ��̼� ���� ���� �ʱ�ȭ
void ClearRabbitAt(int x, int y)
{
	for (int row = 0; row < RabbitY; row++)
	{
		// RabbitX ũ�⸸ŭ ���� �׸���
		char blankLine[RabbitX + 1];
		memset(blankLine, ' ', RabbitX);
		blankLine[RabbitX] = '\0';

		_DrawText(x, y + row, blankLine);
	}
}

// �䳢 �ִϸ��̼� �׸��� �Լ�
void DrawRabbitAt(int x, int y, int idx)
{
	for (int row = 0; row < RabbitY; row++)
	{
		_DrawText(x, y + row, Rabbit[idx][row]);
	}
}

void RabbitSCAnim() // Rabbit stage clear �ִϸ��̼�
{
	_DrawText(20, 10, "�ƹ� Ű�� ���� ���� ���������� �Ѿ��");

	player.Pos.x = RabbitXPos; // �÷��̾� x�� ��ġ �ʱ�ȭ
	player.Pos.y = RabbitYPos; // �÷��̾� y�� ��ġ �ʱ�ȭ

	// RabbitSCAnim���� ���� �������� �䳢 ��ġ�� �����Ͽ�, ���� �����ӿ��� �ش� ��ġ�� ����� �� ���
	static int prevX = -1, prevY = -1;

	// ���� ��ġ �����
	if (prevX >= 0 && prevY >= 0)
	{
		ClearRabbitAt(prevX, prevY);
	}

	// �ö󰡴� ������ �������� ������ �Ǵ�
	int yOffset;
	if (animFrame < jumpFrames)
	{
		// �ö󰡴� ��: yOffset ���� Ŀ�� (0,1,2,...)
		yOffset = (jumpHeight * animFrame) / jumpFrames;
	}
	else
	{
		// �������� ��: yOffset ���� �۾��� (jumpHeight,...1,0)
		int downFrame = animFrame - jumpFrames;
		yOffset = (jumpHeight * (jumpFrames - downFrame)) / jumpFrames;
	}

	int currentY = baseY - yOffset;

	// �䳢 �׸���
	int idx = 12;  // ������ �䳢 ��������Ʈ �ε���
	DrawRabbitAt(centerX, currentY, idx);

	prevX = centerX;
	prevY = currentY;

	// �ִϸ��̼� ������ ����
	animFrame++;

	// �� ����Ŭ ��������
	if (animFrame >= jumpFrames * 2)
	{
		animFrame = 0;
		animRepeatCount++;

		// �ִϸ��̼� �ݺ� ����
		if (animRepeatCount >= maxRepeats)
		{
			animRepeatCount = 0;

			// �ִϸ��̼� ���� �� ���� ��ġ�� �䳢 �׸���
			ClearRabbitAt(prevX, prevY);
			DrawRabbitAt(centerX, baseY, idx);
		}
	}
}

void GetInput() // GetAsyncKeyState�� ���� Ű �Է� ����
{
	g_KeyW = (GetAsyncKeyState('W') & 0x8000); // WŰ(����/�� �̵�) �Է� ����
	g_KeyA = (GetAsyncKeyState('A') & 0x8000); // AŰ(���� �̵�) �Է� ����
	g_KeyD = (GetAsyncKeyState('D') & 0x8000); // DŰ(������ �̵�) �Է� ����
	g_KeyS = (GetAsyncKeyState('S') & 0x8000); // SŰ(�Ʒ� �̵�) �Է� ����

	g_KeySpace = (GetAsyncKeyState(' ') & 0x8000); // �����̽���(����) �Է� ����

}

// Rabbit�� ���� ���� ���� �ִ��� Ȯ�� (g_StagePlatform �Ǵ� g_Map�� '='�� ������ true)
bool CheckGround()
{
	int FpxL = (int)(player.Pos.x + 8) + GetPlusX();
	int FpxR = (int)(player.Pos.x + 12) + GetPlusX();
	int MpxL = (int)(player.Pos.x + 8);
	int MpxR = (int)(player.Pos.x + 12);
	int py = (int)(player.Pos.y + RabbitY);

	// x��ǥ ��� ����
	if (FpxL < 0) FpxL = 0;
	if (FpxR >= MAP_WIDTH) FpxR = MAP_WIDTH - 1;
	if (MpxL < 0) MpxL = 0;
	if (MpxR >= MAP_WIDTH) MpxR = MAP_WIDTH - 1;

	for (int x = FpxL; x <= FpxR; x++)
	{
		int mapStatus = GetMapStatus();

		if (mapStatus < 0 || mapStatus >= 5 || py < 0 || py >= 25) // ȭ�� ������ ������ false ��ȯ
			return false;

		if (g_StagePlatform[mapStatus][py][x] == '=')
			return true;
	}
	for (int x = MpxL; x <= MpxR; x++)
	{
		if (py < 0 || py >= 25) // ȭ�� ������ ������ false ��ȯ
			return false;

		if (g_Map[py][x] == '=')
			return true;
	}
	return false;
}

// �߷� ���� �Լ�: �� �Ʒ��� ������ �ƴϸ� �Ʒ��� ������
void ApplyGravity()
{
	if (!CheckGround())
	{
		if (player.Pos.y > SCREEN_HEIGHT - RabbitY)
			player.Pos.y = SCREEN_HEIGHT - RabbitY;
		player.Pos.y += 1.0f; // �� ĭ�� �Ʒ���
	}
}

// �� �Ʒ� ������ y��ǥ�� ��ȯ (������ -1)
int GetGroundY()
{
	int FpxL = (player.Pos.x + 8) + GetPlusX();
	int FpxR = (player.Pos.x + 12) + GetPlusX();
	int MpxL = (player.Pos.x + 8);
	int MpxR = (player.Pos.x + 12);
	int py = (int)(player.Pos.y + RabbitY);

	if (py >= SCREEN_HEIGHT) return -1;

	// x��ǥ ��� ����
	if (FpxL < 0) FpxL = 0;
	if (FpxR >= MAP_WIDTH) FpxR = MAP_WIDTH - 1;
	if (MpxL < 0) MpxL = 0;
	if (MpxR >= MAP_WIDTH) MpxR = MAP_WIDTH - 1;

	for (int x = FpxL; x <= FpxR; x++)
	{
		int mapStatus = GetMapStatus();

		if (mapStatus < 0 || mapStatus >= 5 || py < 0 || py >= 25) // ȭ�� ������ ������ false ��ȯ
			return false;

		if (g_StagePlatform[mapStatus][py][x] == '=')
			return (py - 1);
	}
	for (int x = MpxL; x <= MpxR; x++)
	{
		if (py < 0 || py >= 25) // ȭ�� ������ ������ false ��ȯ
			return false;

		if (g_Map[py][x] == '=')
			return (py - 1);
	}

	return -1;
}

void JumpFN()
{
	// ���� ����
	if (!player.IsJumping && g_KeyW && CheckGround())
	{
		player.IsJumping = true;
		player.VelY = JUMP_POWER;
	}

	if (player.IsJumping)
	{
		player.VelY += GRAVITY;
		if (player.VelY > MAX_FALL_SPEED)
			player.VelY = MAX_FALL_SPEED;

		// ���� �� �Ʒ��� ������ ����� ����
		if (player.VelY >= 0 && CheckGround())
		{
			int groundY = GetGroundY();
			if (groundY != -1)
				player.Pos.y = groundY - RabbitY + 1; // ���� �ٷ� ���� ����
			else
				player.Pos.y = (int)player.Pos.y;

			player.VelY = 0.0f;
			player.IsJumping = false;
		}

		float nextY = player.Pos.y + player.VelY;
		float step = (player.VelY > 0) ? 1.0f : -1.0f;
		bool landed = false;

		// y�� �̵��� 1�� �ɰ��� �˻�
		while ((step > 0 && player.Pos.y < nextY) || (step < 0 && player.Pos.y > nextY))
		{
			player.Pos.y += step;

			if (player.VelY >= 0 && CheckGround())
			{
				int groundY = GetGroundY();
				if (groundY != -1)
					player.Pos.y = groundY - RabbitY + 1;
				else
					player.Pos.y = (int)player.Pos.y;

				player.VelY = 0.0f;
				player.IsJumping = false;
				landed = true;
				break;
			}
		}

		// ���� �̵��� ó��(�������� ���� ���)
		if (!landed)
			player.Pos.y = nextY;

		// �� �Ʒ��� �������� �� ����
		if (player.Pos.y > SCREEN_HEIGHT - RabbitY)
		{
			player.Pos.y = SCREEN_HEIGHT - RabbitY - 1;
			player.VelY = 0.0f;
			player.IsJumping = false;
		}
	}
}

void AttackFN()
{
	// ���� ����: ���콺 Ŭ������ �� ���� ���� �ƴϸ�
	if (g_KeySpace && !player.IsAttacking)
	{
		// ���������� ����
		player.IsAttacking = true;
		player.AttackFrame = 0;
		player.attackStartTime = GetTickCount();

	}

	// ���� �ִϸ��̼� ó��
	if (player.IsAttacking)
	{
		player.AttackFrame++;


		if (GetTickCount() - player.attackStartTime >= player.attackDuration)
		{
			player.IsAttacking = false;
			player.AttackFrame = 0;

			// ���� �浹 ���� �޾ƿ���
			Rect weaponRect = GetWeaponRect();
		}
	}
}

void moveFN()
{
	// �̵� ó��
	float move = player.IsJumping ? player.Speed * 1.2f : player.Speed;


	// A�� D�� ���ÿ� ������ �� �� false ó��
	if (g_KeyA && g_KeyD)
	{
		g_KeyA = false;
		g_KeyD = false;
	}

	if (g_KeyA)
	{
		if (IsMapEnd) // �� ���� �������� �� �÷��̾� ������ ����
		{
			move = player.IsJumping ? player.Speed * 1.2f : player.Speed;
		}
		else if (player.Pos.x < 25) // ���� ������ �� �÷��̾� ������ 0
		{
			move = 0;
		}
		player.Pos.x -= move;
		player.Direction = 1;
	}
	if (g_KeyD)
	{
		if (IsMapEnd) // �� ���� �������� �� �÷��̾� ������ ����
		{
			move = player.IsJumping ? player.Speed * 1.2f : player.Speed;
		}
		else if (player.Pos.x > 25) // ���� ������ �� �÷��̾� ������ 0
		{
			move = 0;
		}
		player.Pos.x += move;
		player.Direction = 0;
	}

	if (g_KeyS)
	{
		if ((player.Pos.y + RabbitY) > 21) return; // ȭ�� ������ �������� �ʵ���
		DWORD now = GetTickCount64();
		int KeyignoreTime = 250; // Ű �Է� ���� �ð� (ms)

		if (now - g_SKeyLastTime < KeyignoreTime) {
			return; // Ű �Է� ����
		}

		g_SKeyLastTime = now;

		player.Pos.y++;

		CheckGround();

		if (!player.IsJumping && !CheckGround())
		{
			ApplyGravity();
		}
	}
}

void ClimbLadder()
{
	int PxL = (int)(player.Pos.x + 6);
	int PxR = (int)(player.Pos.x + 14);
	int Py = (int)player.Pos.y;

	// x��ǥ ��� ����
	if (PxL < 0) PxL = 0;
	if (PxR >= MAP_WIDTH) PxR = MAP_WIDTH - 1;

	for (int y = Py; y < SCREEN_HEIGHT; y++)
	{
		for (int x = PxL; x <= PxR; x++)
		{
			int mapStatus = GetMapStatus();

			if (mapStatus < 0 || mapStatus >= 5 || Py < 0 || Py >= 25)
				return;

			if (g_StagePlatform[mapStatus][Py][x] == '_')
			{
				IsNearLadder = true; // ��ٸ� ��ó�� ������ true

				if (!player.IsJumping && CheckGround())
				{
					if (GetAsyncKeyState('Q') & 0x8000)
					{// ��ٸ� ��ó���� QŰ�� ������ ��ٸ� �ö󰡱�
						IsNearLadder = false;
						player.Pos.y -= 8.0f; // ��ٸ� ���� �ö󰡱�
					}
				}

				return; // ��ٸ� ��ó�� ������ �Լ� ����
			}
		}
	}

	IsNearLadder = false;
}

void ISOnGoal()
{
	int pxL = (int)(player.Pos.x + 8) + GetPlusX();
	int pxR = (int)(player.Pos.x + 12) + GetPlusX();
	int py = (int)player.Pos.y;

	for (int x = pxL; x <= pxR; x++)
	{
		int mapStatus = GetMapStatus();
		if (mapStatus < 0 || mapStatus >= 5 || py < 0 || py >= 25 || x < 0 || x >= 700)
			return false;

		if (g_StagePlatform[mapStatus][py][x] == '@')// Rabbit�� @�� ��Ҵ��� üũ
		{
			stageClear = true;
			IsInvincibleTime = true;
		}
	}
}

// Ű���� ���� ���� �Լ�
void ClearInputBuffer()
{
	g_Key = NULL;
}

void UpdatePlayer() // �÷��̾� �̵� Ű �Է� ó�� 
{
	if (IsInvincibleTime && !stageClear && (GetTickCount() - player.lastHitTime >= INVINCIBLE_TIME)) {
		SetInvincibleTime(false);
	}

	ISOnGoal(); // �÷��̾ ��ǥ�� �����ߴ��� üũ

	if (!stageClear)
	{
		if (_kbhit())
		{
			ClearInputBuffer();
		}
	}
	else
	{
		if (g_Key != -1)
		{
			stageClear = false;

			system("cls"); // ȭ�� �����

			SetPlusX(0); // �÷��̾ ��ǥ�� �������� ��, ���� x��ǥ�� �ʱ�ȭ
			SetMapStatus(GetMapStatus() + 1);   // �� �������ͽ� 1 ����

			SetMapSetting(false);  // �������� ������ ���� ����

			IsInvincibleTime = false; // ���� �ð� �ʱ�ȭ

			player.Speed = 1; // ������� ����
			speedBuffs.active = false;
			slowDebuffs.active = false;
			player.isBleeding = false; // �÷��̾ �ǰݴ��ߴ��� ���� �ʱ�ȭ	

			player.Pos.x = RabbitXPos; // �÷��̾� x��ġ �ʱ�ȭ
			player.Pos.y = RabbitYPos; // �÷��̾� y��ġ �ʱ�ȭ
		}

	}

	CheckGround(); // �÷��̾ ���� �ִ��� üũ

	if (!player.IsJumping && !CheckGround())
	{
		ApplyGravity();
	}

	int iR = 0;
	int iL = 0;

	switch (player.HeldWeapon->attackSpeed)
	{
	case 3:  // �ܰ�
		iR = 17;
		iL = 2;
		break;
	case 2:  // ���
		iR = 21;
		iL = -2;
		break;
	case 1:  // â
		iR = 20;
		iL = -1;
		break;
	}

	// ��� üũ
	if (player.Pos.x < 0 - iL) player.Pos.x = 0 - iL;
	if (player.Pos.x > SCREEN_WIDTH - iR) player.Pos.x = SCREEN_WIDTH - iR;
	if (player.Pos.y < 0) player.Pos.y = 0;
	if (player.Pos.y > SCREEN_HEIGHT - RabbitY) player.Pos.y = SCREEN_HEIGHT - RabbitY - 1;

	// ���� �� �ö��� �� ���� �迭���� �����Ⱚ���� �ٲ�� ���� ����
	if (player.Pos.y < 2) {
		strcpy(weaponList[0].name, "���");
		strcpy(weaponList[0].sprite, "--|====>");
		weaponList[0].attack = 2;
		weaponList[0].attackSpeed = 2;
	}

	JumpFN();

	AttackFN();

	moveFN();

	ClimbLadder(); // �÷��̾ ��ٸ� ��ó�� �ִ��� üũ
}

char Color = E_White; // �÷��̾� ����

void DrawPlayer()
{
	if (IsNearLadder)
	{
		_DrawText(player.Pos.x - 3, player.Pos.y - 3, "'Q' Ű�� ���� ���� �ö󰡱�");
	}

	if (!player.isBleeding && (IsInvincibleTime || player.Health < PrevPlayerHealth)) // �����ð� + �䳢�� �ǰ� �������� ���� E_Gray��
	{
		Color = E_Gray;
	}
	else if (player.isBleeding && player.Health < PrevPlayerHealth) // �ɰ����� �¾������� ����
	{
		Color = E_Gray;
	}
	else 
	{
		Color = E_White;
	}

	if (player.Health < PrevPlayerHealth)
	{
		PrevPlayerHealth = player.Health; // �÷��̾ �ǰݴ����� �� ���� ü�� ����
	}

	int idx;
	int dir = player.Direction;

	if (!GetWeaponChosen())
	{
		//���� ���� ȭ�鿡 ���� �÷��̾� ��������Ʈ �ε��� ����
		int selectedIndex = GetSelectedIndex();

		if (selectedIndex == 0)
		{
			idx = dir == 0 ? 0 : 1; // ���
		}
		else if (selectedIndex == 1)
		{
			idx = dir == 0 ? 4 : 5; // �ܰ�
		}
		else
		{
			idx = dir == 0 ? 8 : 9; // â
		}
	}
	else
	{
		int weaponType = player.HeldWeapon ? player.HeldWeapon->attackSpeed : 2;
		if (weaponType == 3)      idx = dir == 0 ? 4 : 5; // �ܰ�
		else if (weaponType == 2) idx = dir == 0 ? 0 : 1; // ���
		else                      idx = dir == 0 ? 8 : 9; // â

		if (player.IsAttacking)
		{
			if (weaponType == 3)      idx = dir == 0 ? 6 : 7; // �ܰ�
			else if (weaponType == 2) idx = dir == 0 ? 2 : 3; // ���
			else                      idx = dir == 0 ? 10 : 11; // â
		}
	}

	for (int y = 0; y < RabbitY; y++)
	{
		const char* line = Rabbit[idx][y];
		int len = strlen(line);
		for (int x = 0; x < len; x++)
		{
			if (line[x] != ' ')
			{
				_DrawTextColor((int)player.Pos.x + x, (int)player.Pos.y + y, (char[]) { line[x], '\0' }, Color);
			}
		}
	}
}

void DrawHealth() // �÷��̾� ü�� �׸���
{
	if (player.Health > 0)
	{

		if (player.Health % 2 == 1)
		{
			halfHealth = true; // ü�� ��ĭ �ִ�
		}

		int x = 3;

		for (int i = 0; i < player.Health / 2; i++)
		{
			_DrawText(x, 1, "O"); // ü�� ������ �׸���
			x += 3;
		}

		if (halfHealth)
		{
			halfHealth = false;

			_DrawText(x, 1, "o"); // ü�� ��ĭ ������ �׸���
		}
	}
	else
	{
		ResetMonster(); // ���� alive false ó��
		SetIsGameOver(true);
	}
}

void InitPlayer() // �ʱ�ȭ
{
	g_Key = _GetKey;

	player.Pos.x = RabbitXPos;
	player.Pos.y = RabbitYPos;
	player.Speed = 1.2f;
	player.Health = 10;
	player.VelY = 0.0f;
	player.IsJumping = false;
	player.Direction = 0;

	PrevPlayerHealth = player.Health; // ���� ü�� �ʱ�ȭ

	player.IsAttacking = false;
	player.AttackFrame = 0;
	player.attackStartTime = 0;

	player.isBleeding = false; // �÷��̾ �ǰݴ��ߴ��� ���� �ʱ�ȭ

	// ����
	player.HeldWeapon = &weaponList[GetSelectedIndex()];

	// ���� �ӵ��� ���� ���� ���� �ð� ����
	int speed = player.HeldWeapon->attackSpeed;

	switch (speed)
	{
	case 3:
		player.AttackFrameMax = 10;
		player.attackDuration = 100;
		break; // �ܰ�
	case 2:
		player.AttackFrameMax = 15;
		player.attackDuration = 300;
		break; // ���
	case 1:
		player.AttackFrameMax = 20;
		player.attackDuration = 500;
		break; // â
	}

	animFrame = 0;
	animRepeatCount = 0;
	jumpFrames = 5;
	maxRepeats = 4;
	centerX = 29;
	baseY = 20;
	jumpHeight = 2;
	animFramesTotal = 10;

	isNearItem = false;

	player.lastHitTime = 0;

	speedBuffs.endTime = 0;
	slowDebuffs.endTime = 0;

	speedBuffs.active = false;
	slowDebuffs.active = false;
}
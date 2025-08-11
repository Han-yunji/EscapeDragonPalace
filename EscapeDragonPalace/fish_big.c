#include "init.h"
#include "fish_big.h"
#include "Rabbit.h"

Monster g_BigFishMon;	// ū ����� ���� ����ü ���� ����
BigFish g_BigFishList[STAGE_CNT][BIGFISH_CNT];	// ū ����� ����Ʈ �迭
int g_BigFishListIdx[STAGE_CNT] = { 0, };

// ū ����� ������Ʈ
void UpdateBigFish(unsigned long now)
{
	// ���� ���� ���� ������ �ҷ�����
	BigFish* tempBigFish = g_BigFishList[GetMapStatus()];
	// ���� �ʿ� �ִ� ���� ����ŭ �ݺ��ϱ�
	for (int idx = 0; idx < g_BigFishListIdx[GetMapStatus()]; idx++)
	{
		// ���Ͱ� �׾��� ��� �Ѿ��
		if (!tempBigFish[idx].mon.alive) continue;

		// ���� �̵�
		tempBigFish[idx].pos.x += (tempBigFish[idx].dir == E_Right) ? g_BigFishMon.speed : -g_BigFishMon.speed;

		// ������ ���� �ȿ��� �̵�
		if (tempBigFish[idx].pos.x <= tempBigFish[idx].startPosX)
		{
			tempBigFish[idx].dir = E_Right;
		}
		else if (tempBigFish[idx].pos.x + BIGFISH_WIDTH >= tempBigFish[idx].startPosX + tempBigFish[idx].moveNum)
		{
			tempBigFish[idx].dir = E_Left;
		}
	}
}

// ū ����� �׸���
void DrawBigFish()
{

	// ���� �� ������ �ӽ÷� �ҷ�����
	BigFish* tempBigFish = g_BigFishList[GetMapStatus()];
	for (int idx = 0; idx < g_BigFishListIdx[GetMapStatus()]; idx++)
	{
		// �����ð� ������ �ǰ� ���� ����
		if (tempBigFish[idx].mon.isDamaged && GetTickCount() - tempBigFish[idx].mon.lastHitTime >= MONSTER_INVINCIBLE_TIME)
		{
			tempBigFish[idx].mon.isDamaged = false;
		}

		// ���Ͱ� �׾��� ��� �Ѿ��
		if (!tempBigFish[idx].mon.alive) continue;

		// �ǰ� �� ������, ��� �Ķ���
		_SetColor(tempBigFish[idx].mon.isDamaged ? E_BrightRed : E_BrightBlue);
		int posX = tempBigFish[idx].pos.x - GetPlusX();
		for (int y = 0; y < BIGFISH_HEIGHT; y++)
		{
			for (int x = 0; x < BIGFISH_WIDTH; x++)
			{
				if (g_BigFishGraphic[tempBigFish[idx].dir][y][x] != ' ')
				{
					// ȭ�� ���� ���� ���� ��� �׸���
					if (0 <= posX + x && SCREEN_WIDTH > posX + x)
					{
						if (g_BigFishGraphic[tempBigFish[idx].dir][y][x] == ' ' &&
							(x <= 3 || x >= BIGFISH_WIDTH - 3)) continue;
						_DrawText(posX + x, tempBigFish[idx].pos.y + y,
							(char[]) {
							g_BigFishGraphic[tempBigFish[idx].dir][y][x], 0
						});
					}
				}
			}
		}
	}
}

//����� > �÷��̾� �����ϴ� �Լ�
void BigFishHitPlayer()
{
	BigFish* bigFishList = g_BigFishList[GetMapStatus()];
	DWORD now = GetTickCount(); //�ǰ� �ð� ������ ���� ���� �ð� ��������
	Rect PlayerPos = GetPlayerRect(); //�÷��̾� ��ġ

	for (int idx = 0; idx < g_BigFishListIdx[GetMapStatus()]; idx++)
	{
		int posX = bigFishList[idx].pos.x - GetPlusX(); //����� x��ǥ
		int posY = bigFishList[idx].pos.y; //����� y��ǥ
		Rect MosterPos = { posX, posY, 13, 3 };

		if (!bigFishList[idx].mon.alive) continue; // ���Ͱ� �׾��� ��� �Ѿ��

		if (!(IsOverlap(PlayerPos, MosterPos)))	//�浹���� ���� ��� �ѱ��
			continue;

		// ���� �ð� üũ
		if (now - player.lastHitTime < INVINCIBLE_TIME) {
			continue; // ���� ���� ���¸� ������ ����
		}

		SetInvincibleTime(true);	// �÷��̾� ���� �ð� ����
		player.Health -= bigFishList[idx].attack; // �÷��̾� ü�� 2 ����
		player.lastHitTime = now; // ������ �ǰ� �ð� ����


	}
}

// �÷��̾� > ū ����� �����ϴ� �Լ�
void PlayerHitBigFish()
{
	BigFish* bigFishList = g_BigFishList[GetMapStatus()];
	DWORD now = GetTickCount();
	Rect PlayerWeaponPos = GetWeaponRect();

	for (int idx = 0; idx < g_BigFishListIdx[GetMapStatus()]; idx++)
	{
		int posX = bigFishList[idx].pos.x - GetPlusX();// ����� x��ǥ
		int posY = bigFishList[idx].pos.y;// ����� y��ǥ
		Rect MosterPos = { posX, posY, 13, 3 };

		if (!player.IsAttacking) continue;// �÷��̾ ���� ���� �ƴ� ��� �Ѿ��

		if (!bigFishList[idx].mon.alive) continue; // ���Ͱ� �׾��� ��� �Ѿ��

		if (bigFishList[idx].mon.isDamaged) continue; // ���Ͱ� ���� ������ ��� �Ѿ��

		if (!(IsOverlap(PlayerWeaponPos, MosterPos))) continue;//�浹�� ������ �ѱ��

		if (now - bigFishList[idx].mon.lastHitTime < MONSTER_INVINCIBLE_TIME) continue;
		bigFishList[idx].mon.hp -= player.HeldWeapon->attack; // ����� ü�� ����
		bigFishList[idx].mon.isDamaged = true; // ���� ���·� ����
		bigFishList[idx].mon.lastHitTime = now; // ������ �ǰ� �ð� ����


		if (bigFishList[idx].mon.hp <= 0) {//������ ó�� ����
			bigFishList[idx].mon.alive = false;
		}
	}
}


void ResetBigFish() {// ū ����� �ʱ�ȭ
	for (int i = 0; i < STAGE_CNT; i++)
	{
		BigFish* tempBigFish = g_BigFishList[i];
		for (int idx = 0; idx < g_BigFishListIdx[i]; idx++)
		{
			tempBigFish[idx].mon.alive = false;
		}
	}
}



void InitBigFish()
{
	// ū ����� ����� �Ӽ� ����
	g_BigFishMon = (Monster)
	{
		.alive = true,		// ���� ����
		.hp = 4,			// ü��
		.isDamaged = false,	// �ǰ� ���� (���� ����)
		.lastHitTime = 0,	// ������ �ǰ� �ð�
		.speed = 0.6,		// �̵� �ӵ�
	};

	// ����
	g_BigFishList[E_Jail][g_BigFishListIdx[E_Jail]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 95,		// X ��ǥ
		.pos.y = 11,		// Y ��ǥ
		.startPosX = 95,	// �ʱ� X ��ǥ
		.attack = BIGFISH_ATTACK,	// ���ݷ�
		.moveNum = 62,		// �̵� ����
		.dir = E_Right,		// �ٶ󺸴� ����
	};

	g_BigFishList[E_Jail][g_BigFishListIdx[E_Jail]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 170,
		.pos.y = 11,
		.startPosX = 170,
		.attack = BIGFISH_ATTACK,
		.moveNum = 62,
		.dir = E_Right,
	};

	g_BigFishList[E_Jail][g_BigFishListIdx[E_Jail]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 277,
		.pos.y = 11,
		.startPosX = 277,
		.attack = BIGFISH_ATTACK,
		.moveNum = 74,
		.dir = E_Right,
	};

	g_BigFishList[E_Jail][g_BigFishListIdx[E_Jail]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 490,
		.pos.y = 12,
		.startPosX = 490,
		.attack = BIGFISH_ATTACK,
		.moveNum = 57,
		.dir = E_Right,
	};

	g_BigFishList[E_Jail][g_BigFishListIdx[E_Jail]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 547,
		.pos.y = 21,
		.startPosX = 490,
		.attack = BIGFISH_ATTACK,
		.moveNum = 57,
		.dir = E_Right,
	};

	// ���
	g_BigFishList[E_DragonPalace][g_BigFishListIdx[E_DragonPalace]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 71,
		.pos.y = 14,
		.startPosX = 71,
		.attack = BIGFISH_ATTACK,
		.moveNum = 59,
		.dir = E_Right,
	};

	g_BigFishList[E_DragonPalace][g_BigFishListIdx[E_DragonPalace]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 169,
		.pos.y = 14,
		.startPosX = 169,
		.attack = BIGFISH_ATTACK,
		.moveNum = 36,
		.dir = E_Right,
	};

	g_BigFishList[E_DragonPalace][g_BigFishListIdx[E_DragonPalace]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 245,
		.pos.y = 11,
		.startPosX = 245,
		.attack = BIGFISH_ATTACK,
		.moveNum = 84,
		.dir = E_Right,
	};

	g_BigFishList[E_DragonPalace][g_BigFishListIdx[E_DragonPalace]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 514,
		.pos.y = 11,
		.startPosX = 514,
		.attack = BIGFISH_ATTACK,
		.moveNum = 52,
		.dir = E_Right,
	};

	// �ٴ�1
	g_BigFishList[E_Sea1][g_BigFishListIdx[E_Sea1]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 80,
		.pos.y = 21,
		.startPosX = 80,
		.attack = BIGFISH_ATTACK,
		.moveNum = 94,
		.dir = E_Right,
	};

	g_BigFishList[E_Sea1][g_BigFishListIdx[E_Sea1]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 187,
		.pos.y = 12,
		.startPosX = 187,
		.attack = BIGFISH_ATTACK,
		.moveNum = 72,
		.dir = E_Right,
	};

	g_BigFishList[E_Sea1][g_BigFishListIdx[E_Sea1]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 290,
		.pos.y = 21,
		.startPosX = 290,
		.attack = BIGFISH_ATTACK,
		.moveNum = 56,
		.dir = E_Right,
	};

	g_BigFishList[E_Sea1][g_BigFishListIdx[E_Sea1]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 373,
		.pos.y = 13,
		.startPosX = 373,
		.attack = BIGFISH_ATTACK,
		.moveNum = 72,
		.dir = E_Right,
	};

	g_BigFishList[E_Sea1][g_BigFishListIdx[E_Sea1]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 458,
		.pos.y = 21,
		.startPosX = 458,
		.attack = BIGFISH_ATTACK,
		.moveNum = 119,
		.dir = E_Right,
	};

	// �ٴ�2
	g_BigFishList[E_Sea2][g_BigFishListIdx[E_Sea2]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 231,
		.pos.y = 21,
		.startPosX = 231,
		.attack = BIGFISH_ATTACK,
		.moveNum = 86,
		.dir = E_Right,
	};

	g_BigFishList[E_Sea2][g_BigFishListIdx[E_Sea2]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 313,
		.pos.y = 16,
		.startPosX = 313,
		.attack = BIGFISH_ATTACK,
		.moveNum = 79,
		.dir = E_Right,
	};

	g_BigFishList[E_Sea2][g_BigFishListIdx[E_Sea2]++] = (BigFish)
	{
		.mon = g_BigFishMon,
		.pos.x = 401,
		.pos.y = 16,
		.startPosX = 401,
		.attack = BIGFISH_ATTACK,
		.moveNum = 51,
		.dir = E_Right,
	};
}
#include "init.h"
#include "fish_small.h"
#include "Rabbit.h"

Monster g_SmallFishMon;	// ���� ����� ���� ����ü ���� ����
SmallFish g_SmallFishList[STAGE_CNT][SMALLFISH_CNT];	// ���� ����� ����Ʈ �迭
int g_SmallFishListIdx[STAGE_CNT] = { 0, };

// ���� ����� ������Ʈ
void UpdateSmallFish(unsigned long now)
{
	// ���� ���� ���� ������ �ҷ�����
	SmallFish* tempSmallFish = g_SmallFishList[GetMapStatus()];
	for (int idx = 0; idx < g_SmallFishListIdx[GetMapStatus()]; idx++)
	{
		// ���Ͱ� �׾��� ��� �Ѿ��
		if (!tempSmallFish[idx].mon.alive) continue;

		// ���Ͱ� ȭ�� �ȿ� ���� ���
		if (tempSmallFish[idx].pos.x - GetPlusX() > 0 && tempSmallFish[idx].pos.x - GetPlusX() < SCREEN_WIDTH) {
			// �÷��̾�� ������ Y ��ǥ�� ���� ��
			if (tempSmallFish[idx].pos.y == player.Pos.y + 2) {
				if(tempSmallFish[idx].pos.x - GetPlusX() + 8 >= player.Pos.x + 12) {
					tempSmallFish[idx].isRush = true;	// ���� ���·� ����
				}
			}

		}
		if (tempSmallFish[idx].isRush) {
			// ���� �̵�
			tempSmallFish[idx].pos.x -= g_SmallFishMon.speed;

			// ���Ͱ� ���� ȭ�� ������ �Ѿ�� ��� ���� ó��
			if (tempSmallFish[idx].pos.x <= 0)
			{
				tempSmallFish[idx].mon.alive = false;
				tempSmallFish[idx].isRush = false;	// ���� ���� ����	
			}
		}
	}
}

// ���� ����� �׸���
void DrawSmallFish()
{
	_SetColor(E_Teal);	// �׻� �Ķ���

	// ���� �� �����Ϳ��� ����� ��ġ �޾ƿ���
	SmallFish* tempSmallFish = g_SmallFishList[GetMapStatus()];
	for (int idx = 0; idx < g_SmallFishListIdx[GetMapStatus()]; idx++)
	{
		// ���Ͱ� �׾��� ��� �Ѿ��
		if (!tempSmallFish[idx].mon.alive) continue;

		int tempX = tempSmallFish[idx].pos.x - GetPlusX();

		for (int x = 0; x < SMALLFISH_WIDTH; x++)
		{
			if (g_SmallFishGraphic[x] != ' ')
			{
				// ȭ�� ���� ���� ���� ��� �׸���
				if (0 <= tempX + x && SCREEN_WIDTH > tempX + x)
				{
					_DrawText(tempX + x, tempSmallFish[idx].pos.y, (char[]) { g_SmallFishGraphic[x], 0 });
				}
			}
		}
	}
}

// �÷��̾� > ���� ����� �����ϴ� �Լ�
void PlayerHitSmallFish()
{
	SmallFish* smallFishList = g_SmallFishList[GetMapStatus()];
	int fishCount = g_SmallFishListIdx[GetMapStatus()];
	DWORD now = GetTickCount();
	Rect PlayerWeaponPos = GetWeaponRect();

	for (int idx = 0; idx < fishCount; idx++)
	{
		SmallFish* pFish = &smallFishList[idx];
		int posX = pFish->pos.x - GetPlusX();
		int posY = pFish->pos.y;
		Rect MonsterPos = { posX, posY, 7, 1 };

		if (player.IsAttacking)
		{
			if (!pFish->mon.alive) continue;

			if (!IsOverlap(PlayerWeaponPos, MonsterPos))
				continue;

			if (now - pFish->mon.lastHitTime < MONSTER_INVINCIBLE_TIME)
				continue;

			pFish->mon.hp -= player.HeldWeapon->attack;	// ���� ü�� ����
			pFish->mon.isDamaged = true;	// �ǰ� ���·� ����
			pFish->mon.lastHitTime = now;

			if (pFish->mon.hp <= 0) {
				pFish->mon.alive = false;	// ü���� 0 ���ϰ� �Ǹ� ��� ó��
			}
		}
	}
}

// ���� ����� > �÷��̾� �����ϴ� �Լ�
void SmallFishHitPlayer()
{
	SmallFish* smallFishList = g_SmallFishList[GetMapStatus()];
	int fishCount = g_SmallFishListIdx[GetMapStatus()];
	DWORD now = GetTickCount();

	Rect PlayerPos = { player.Pos.x + 8 + GetPlusX(), player.Pos.y, 5, 3 };

	for (int idx = 0; idx < fishCount; idx++)
	{
		SmallFish* pFish = &smallFishList[idx];
		int posX = pFish->pos.x;
		int posY = pFish->pos.y;
		Rect MonsterPos = { posX, posY, 6, 1 };

		if (!pFish->mon.alive) continue;

		if (!IsOverlap(PlayerPos, MonsterPos))
			continue;

		if (now - player.lastHitTime < INVINCIBLE_TIME)
			continue;

		SetInvincibleTime(true);	// �÷��̾� ���� �ð� ����
		player.Health -= pFish->attack;
		player.lastHitTime = now;
	}
}

void ResetSmallFish() {
	for (int i = 0; i < STAGE_CNT; i++)
	{
		SmallFish* tempSmallFish = g_SmallFishList[i];
		for (int idx = 0; idx < g_SmallFishListIdx[i]; idx++)
		{
			tempSmallFish[idx].mon.alive = false;
		}
	}
}


void InitSmallFish()
{
	// ���� ����� ����� �Ӽ� ����
	g_SmallFishMon = (Monster)
	{
		.alive = true,		// ���� ����
		.hp = 1,			// ü��
		.isDamaged = false,	// �ǰ� ���� (���� ����)
		.lastHitTime = 0,	// ������ �ǰ� �ð�
		.speed = 1.6,		// �̵� �ӵ�
	};

	// ����
	g_SmallFishList[E_Jail][g_SmallFishListIdx[E_Jail]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 470,	// X ��ǥ
		.pos.y = 18,	// Y ��ǥ
		.attack = SMALLFISH_ATTACK,	// ���ݷ�
		.isRush = false
	};

	// ���
	g_SmallFishList[E_DragonPalace][g_SmallFishListIdx[E_DragonPalace]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 375,
		.pos.y = 23,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};

	g_SmallFishList[E_DragonPalace][g_SmallFishListIdx[E_DragonPalace]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 445,
		.pos.y = 7,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};

	// �ٴ�1
	g_SmallFishList[E_Sea1][g_SmallFishListIdx[E_Sea1]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 310,
		.pos.y = 14,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};

	g_SmallFishList[E_Sea1][g_SmallFishListIdx[E_Sea1]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 620,
		.pos.y = 23,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};

	// �ٴ�2
	g_SmallFishList[E_Sea2][g_SmallFishListIdx[E_Sea2]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 236,
		.pos.y = 12,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};

	g_SmallFishList[E_Sea2][g_SmallFishListIdx[E_Sea2]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 412,
		.pos.y = 13,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};

	g_SmallFishList[E_Sea2][g_SmallFishListIdx[E_Sea2]++] = (SmallFish)
	{
		.mon = g_SmallFishMon,
		.pos.x = 570,
		.pos.y = 15,
		.attack = SMALLFISH_ATTACK,
		.isRush = false
	};
}
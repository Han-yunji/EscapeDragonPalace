#pragma once

#define ESC 27 // ESC Ű ��

// �Լ�
void GameStartScreen();
void GameOverScreen();
void GameClearSceen();
void DrawStartScreen();
void ReturnStartScreen();
void DrawGameClearScreen();
bool GetGameStart();
bool GetIsGameOver();
void SetIsGameOver(int src);
bool GetTextE();
void SetTextE(int src);
bool GetControlScreen();
void SetControlScreen(bool src);
void DrawControlsScreen();


static char Controls[25][35] = {
"             ���۹� ����",
"",
"",
"          ��  �� ����",
"                 _____",
"                |  W  |",
"                |_____|",
"           _____ _____ _____",
"          |  A  |  S  |  D  |",
"          |_____|_____|_____|",
" ",
"           ����  �Ʒ�  ������",
"    ",
"     ____________________________",
"    |          SPACEBAR          |",
"    |____________________________|",
"    ",
"                 ����"
};


#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <cstdio>

using namespace std;

#include <stdio.h>
#include <Windows.h>

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;

int nScreenWidth = 120;
int nScreenHeight = 30;

int Rotate(int px, int py, int r) {
	switch (r % 4) {
	case 0: return py * 4 + px;
	case 1: return 12 + py - (px * 4);
	case 2: return 15 - (py * 4) - px;
	case 3: return 3 - py + (px * 4);
	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {

	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++) {
			// Get Index into Piece
			int pi = Rotate(px, py, nRotation);

			// Get Index into Field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false;
				}
			}
		}

	return true;
}

void readScores(int place[], int lbScore[]) {
	fstream inFile;
	inFile.open("Leaderboard.txt");
	int i = 0;

	while (inFile >> place[i])
	{
		
		inFile >> lbScore[i];
		i++;

	}
	inFile.close();
}

void writeScore(int place, int score, int lbScore[]) {
	ofstream out ("temp.txt");
	for (int j = 0; j < 25; j++)
		out << j+1 << " " << lbScore[j] << endl;
	out.close();
	//cout << "Removed" << endl;
	rename("Leaderboard.txt", "LeaderboardBP.txt");
	remove("Leaderboard.txt");
	rename("temp.txt", "Leaderboard.txt");
	//cout << "Renamed" << endl;

}



void highScore(int place, int score, int lbScore[]) {
	int temp = 0;
	if (score > lbScore[place]) {
		cout << "High Score!" << endl << "Place: " << place+1 << endl << endl;
		for (int x = 24; x >= place; x--) {
			//cout << x+1 << " " << place << " " << lbScore[x] << endl; <- Debug
			temp = lbScore[x];
			lbScore[x] = lbScore[x + 1];
			lbScore[x + 1] = temp;
		}

		lbScore[place] = score;
		writeScore(place, score, lbScore);
	}
}

void saveScore(int score, int lbScore[]) {
	int temp = 0;
	int place = 25;
	//for(int i = 0; i < 25-1; i++)
		for (int t = 24; t >= 0; t--) {
			if (score > lbScore[t]) {
				//cout << score << " > " << lbScore[t] << " " << place << "  " << t << endl; // < - Debug Line 
					if (place > 0) {
					place--;
				}

			}
		}
	highScore(place, score, lbScore);
}

int main() {

	// Create Assets 
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Game Logic Var(s)
	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;


	vector<int> vLines;
	int place[26], lbScore[26];
	while (!bGameOver) {

		readScores(place, lbScore);
		// GAME TIMING ==================
		this_thread::sleep_for(50ms); // Game Tick
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// INPUT ========================
		for (int k = 0; k < 4; k++)								// R   L   D   U
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26"[k]))) != 0;

		// GAME LOGIC ===================
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
			else bRotateHold = false;


		if (bForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{
				// Lock Piece in Place
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				nPieceCount++;
				if(nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--;

				// Check if any lines
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1) {
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine) {
							// Removes Line Sets to "="
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}
				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

				// Choose Next Piece;
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;


				// if piece doesn't fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}




		// RENDER OUTPUT ================
		

		// Design Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		// Print Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty())
		{
			// Display
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Tiny Delay

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
						pField[px] = 0;
				}

			vLines.clear();
		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);
	cout << "Game Over, \nScore: " << nScore << endl << endl;
	saveScore(nScore, lbScore);
	cout << "Hall of Fame: " << endl;
	for (int i = 0; i < 10; i++)
		cout << "#" << i+1 << " Score: " << lbScore[i] << endl;
	system("pause");


	return 0;
}
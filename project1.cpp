#include<iostream>
#include<vector>
#include<conio.h>
#include<windows.h>
#include<time.h>
#include<fstream>
#include<algorithm>
#include <mmsystem.h>



#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 30
#define WIN_WIDTH 70

using namespace std;

void playMusic() {
    PlaySound(TEXT("sound.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}


HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

int enemyY[3];
int enemyX[3];
int enemyFlag[3];
char car[4][4] = { '\xB1','\xDB','\xDB','\xB1',
                  '\xDB','\xDB','\xDB','\xDB',
                  '\xB1','\xDB','\xDB','\xB1',
                  '\xDB','\xDB','\xDB','\xDB'};

int carpos = WIN_WIDTH / 2;
int score = 0;
string playerName;

#define COLOR_DEFAULT 7 // Default color (white text on black background)
#define COLOR_CAR 10    // Color for the car (green text)
#define COLOR_ENEMY 12  // Color for the enemy (red text)
//#define COLOR_BORDER 18
#define COLOR_YELLOW 14


enum DifficultyLevel {
    EASY,
    MEDIUM,
    HARD
};

DifficultyLevel currentDifficulty = EASY;

void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

void setcursor(bool visible, DWORD size) {
    if (size == 0)
        size = 50;

    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;

    SetConsoleCursorInfo(console, &lpCursor);
}


void drawBorder() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < 18; j++) {//draw the left and right border
            gotoxy(0 + j, i);//set the cursor to the current column and row
            cout << "|";
            gotoxy(WIN_WIDTH - j, i);
            cout << "|";//bar
        }
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        gotoxy(SCREEN_WIDTH, i);
        cout << "|";
    }
}
void genEnemy(int ind) {
    enemyX[ind] = 17 + rand() % (33);
}

int getSleepDuration() {
    switch (currentDifficulty) {
        case EASY:
            return 40;  // Adjust sleep duration for easy level
        case MEDIUM:
            return 30;  // Adjust sleep duration for medium level
        case HARD:
            return 20;  // Adjust sleep duration for hard level
        default:
            return 40;  // Default sleep duration for unknown difficulty
    }
}

void drawCar() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy(j + carpos, i + 22);
            SetConsoleTextAttribute(console, COLOR_CAR);
            cout << car[i][j];
            SetConsoleTextAttribute(console, COLOR_DEFAULT); // Reset color to default
        }
    }
}
void eraseCar() {//for updating we should erase the car
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy(j + carpos, i + 22); cout << " ";
        }
    }
}
int collision() {
    if (enemyY[0] + 4 >= 23) {
        if (enemyX[0] + 4 - carpos >= 0 && enemyX[0] + 4 - carpos < 9) {//horizonatal position of the enemy align with the current carposition
            return 1;
        }
    }
    return 0;
}

void drawEnemy(int ind) {
    if (enemyFlag[ind] == true) {
        gotoxy(enemyX[ind], enemyY[ind]);
        SetConsoleTextAttribute(console, COLOR_ENEMY);
        cout << '\xDB' << '\xDB' << '\xDB' << '\xDB';
        gotoxy(enemyX[ind], enemyY[ind] + 1);
        cout << '\xB1' << '\xDB' << '\xDB' << '\xB1';
        gotoxy(enemyX[ind], enemyY[ind] + 2);
        cout << '\xDB' << '\xDB' << '\xDB' << '\xDB';
        gotoxy(enemyX[ind], enemyY[ind] + 3);
        cout << '\xB1' << '\xDB' << '\xDB' << '\xB1';
        SetConsoleTextAttribute(console, COLOR_DEFAULT); // Reset color to default
    }
}

void eraseEnemy(int ind) {
    if (enemyFlag[ind] == true) {
        gotoxy(enemyX[ind], enemyY[ind]); cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 1); cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 2); cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 3); cout << "    ";
    }
}

void resetEnemy(int ind) {
    eraseEnemy(ind);
    enemyY[ind] = 1;
    genEnemy(ind);
}
void playCollisionSound() {
    PlaySound(TEXT("music.wav"), NULL, SND_FILENAME | SND_ASYNC);
}



void updateScore() {
    gotoxy(WIN_WIDTH + 7, 5);
    cout << "Score: " << score << endl;
}

void saveScore(int score) {
    ofstream scoreFile("scores.txt", ios::app);
    if (scoreFile.is_open()) {
        scoreFile << playerName << " " << score << endl;
        scoreFile.close();
    }
}

void displayTopScores() {
    system("cls");
    cout << "\t\t----------------------------" << endl;
    cout << "\t\t------ Top 10 Scores --------" << endl;
    cout << "\t\t----------------------------" << endl;

    ifstream scoreFile("scores.txt");
    if (scoreFile.is_open()) {
        vector<pair<string, int>> players;
        string name;
        int s;
        while (scoreFile >> name >> s) {
            players.push_back({name, s});
        }

        players.push_back({playerName, score});
        sort(players.begin(), players.end(), [](const auto &a, const auto &b) {
            return a.second > b.second;
        });

        int count = min(10, static_cast<int>(players.size()));
        for (int i = 0; i < count; ++i) {
            cout << "\t\t" << players[i].first << ": " << players[i].second << endl;
        }

        scoreFile.close();
    } else {
        cout << "Unable to open scores file." << endl;
    }

    cout << "\n\t\tPress any Key to go back to menu.";
    getch();
}

void displayPlayerScore() {
    system("cls");
    cout << "\t\t----------------------------" << endl;
    cout << "\t\t------ Player Score --------" << endl;
    cout << "\t\t----------------------------" << endl;
    cout << "\t\tPlayer Name: " << playerName << endl;
    cout << "\t\tYour Score: " << score << endl;
    cout << "\n\t\tPress any Key to go back to menu.";
    getch();
}

void gameover() {
    playCollisionSound();
    cout << endl;
    cout << "\t\t----------------------------" << endl;
    cout << "\t\t--------- Game Over --------" << endl;
    cout << "\t\t----------------------------" << endl;
    cout << "\t\tPlayer Name: " << playerName << endl;
    cout << "\t\tYour Score: " << score << endl;

     //PlaySound(NULL, 0, 0);



    saveScore(score);
    displayPlayerScore();
    system("cls");
}


void drawInstructions() {
    system("cls");
    cout << "Instructions";
    cout << "\n-----------------";
    cout << "\n Avoid car by moving left or right.";
    cout << "\n\n Press 'Arrow Keys' to move the Car";
    cout << "\n\n Press 'escape' to Exit";
    cout << "\n\n Press any Key to go back to the menu";
    getch();
}

void setDifficulty(DifficultyLevel level) {
    currentDifficulty = level;
    switch (level) {
        case EASY:
            // Adjust game parameters for easy level
            break;
        case MEDIUM:
            // Adjust game parameters for medium level
            break;
        case HARD:
            // Adjust game parameters for hard level
            break;
    }
}

void setDifficultyMenu() {
    system("cls");
    cout << "Select Difficulty Level:";
    cout << "\n1. Easy";
    cout << "\n2. Medium";
    cout << "\n3. Hard";
    cout << "\n\nEnter choice (1-3): ";

    char choice = getche();
    switch (choice) {
        case '1':
            setDifficulty(EASY);
            break;
        case '2':
            setDifficulty(MEDIUM);
            break;
        case '3':
            setDifficulty(HARD);
            break;
        default:
            cout << "\nInvalid choice. Setting to Easy by default.";
            setDifficulty(EASY);
            break;
    }

    cout << "\nDifficulty set. Press any Key to go back to the menu.";
    getch();
}

void play() {
    cout << "Enter your name: ";
    cin >> playerName;

    carpos = -1 + WIN_WIDTH / 2;
    score = 0;
    enemyFlag[0] = 1;
    enemyFlag[1] = 0;
    enemyY[0] = enemyY[1] = 1;

    setDifficulty(currentDifficulty);

    system("cls");
    drawBorder();
    updateScore();
    genEnemy(0);
    genEnemy(1);

    gotoxy(WIN_WIDTH + 7, 2); cout << "Car simulation Game";
    gotoxy(WIN_WIDTH + 6, 4); cout << "----------";
    gotoxy(WIN_WIDTH + 6, 6); cout << "----------";
    gotoxy(WIN_WIDTH + 7, 12); cout << "Control ";
    gotoxy(WIN_WIDTH + 7, 13); cout << "-------- ";
    gotoxy(WIN_WIDTH + 2, 14); cout << "A Key - Left";
    gotoxy(WIN_WIDTH + 2, 15); cout << "D Key - Right";

    gotoxy(18, 5); cout << "Press any key to start";
    getch();
    gotoxy(18, 5); cout << "                      ";

      playMusic();

    while (1) {

        if (kbhit()) {
            char ch = getch();
            if (ch == 'a' || ch == 'A' || ch == 75) {
                if (carpos > 18)
                    carpos -= 4;
            }
            if (ch == 'd' || ch == 'D' || ch == 77) {
                if (carpos < 50)
                    carpos += 4;
            }
            if (ch == 27) {

                break;
            }
        }



        drawCar();
        drawEnemy(0);
        drawEnemy(1);
        if (collision() == 1) {
            //playCollisionSound();
            gameover();
            return;
        }


        Sleep(getSleepDuration());
        eraseCar();
        eraseEnemy(0);
        eraseEnemy(1);

        if (enemyY[0] == 10)
            if (enemyFlag[1] == 0)
                enemyFlag[1] = 1;

        if (enemyFlag[0] == 1)
            enemyY[0] += 1;

        if (enemyFlag[1] == 1)
            enemyY[1] += 1;

        if (enemyY[0] > SCREEN_HEIGHT - 4) {

            resetEnemy(0);
            score++;
            updateScore();
        }
        if (enemyY[1] > SCREEN_HEIGHT - 4) {
            resetEnemy(1);
            score++;
            updateScore();

        }
    }
}

int main() {
    setcursor(0, 0);
    srand((unsigned)time(NULL));

    do {
        system("cls");
        gotoxy(10, 5); cout << " -------------------------- ";
        gotoxy(10, 6); cout << " |        Car simulation Game        | ";
        gotoxy(10, 7); cout << " --------------------------";
        gotoxy(10, 9); cout << "1. Start Game";
        gotoxy(10, 10); cout << "2. Instructions";
        gotoxy(10, 11); cout << "3. Top Scores";
        gotoxy(10, 12); cout << "4. Difficulty Level";
        gotoxy(10, 13); cout << "5. Quit";
        gotoxy(10, 15); cout << "Select option: ";
        char op = getche();

        switch (op) {
            case '1':
                play();
                break;
            case '2':
                drawInstructions();
                break;
            case '3':
                displayTopScores();
                break;
            case '4':
                setDifficultyMenu();
                break;
            case '5':
                exit(0);
        }

    } while (1);

    return 0;
}

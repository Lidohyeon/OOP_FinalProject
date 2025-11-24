#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>
#include <vector>
#include <string>

#include "GameManger.h"
#include "SentenceManager.h"

// 기본 화면 인터페이스
class Screen
{
public:
    virtual ~Screen() {}
    virtual void UpdateScreen() = 0;
    virtual void resizeScreen() = 0;
    virtual void shapeScreen() = 0;
};

// -------------------------------------------------------
// 🎮 게임 플레이 화면 (PlayScreen)
// -------------------------------------------------------
class PlayScreen : public Screen
{
private:
    int currentLevel;
    int gameWidth;
    int gameHeight;
    bool gameRunning;
    int gameAreaWidth;            // 게임 영역 폭 (왼쪽)
    int scoreAreaWidth;           // 점수판 영역 폭 (오른쪽)
    GameManager *gameManager;     // 게임 상태 관리
    SentenceManager *sentenceManager; // 단어 및 문장 관리

    // =========================================================
    // 🎨 [Visual Artist] 화면 그리기 도우미 함수들 (Private)
    // =========================================================

    // 1. 전체 테두리 및 구획 나누기
    void drawFrame()
    {
        attron(COLOR_PAIR(1));

        // 상단 가로선
        mvprintw(0, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++) mvprintw(0, i, "-");
        mvprintw(0, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++) mvprintw(0, i, "-");
        mvprintw(0, gameWidth - 1, "+");

        // 상단 제목 영역
        mvprintw(1, 0, "|");
        mvprintw(1, 2, "SNOW MAN GAME - Level %d", currentLevel);
        mvprintw(1, gameAreaWidth, "|");
        mvprintw(1, gameAreaWidth + 2, "Let's Build a Snowman!"); // 문구 변경
        mvprintw(1, gameWidth - 1, "|");

        // 중간 가로선 (게임영역 상단 구분)
        mvprintw(2, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++) mvprintw(2, i, "-");
        mvprintw(2, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++) mvprintw(2, i, "-");
        mvprintw(2, gameWidth - 1, "+");

        // 세로선 그리기 (왼쪽, 중간, 오른쪽)
        for (int row = 3; row < gameHeight - 2; row++)
        {
            mvprintw(row, 0, "|");             // 왼쪽 끝
            mvprintw(row, gameAreaWidth, "|"); // 중간 구분선
            mvprintw(row, gameWidth - 1, "|"); // 오른쪽 끝
        }

        // 하단 가로선
        mvprintw(gameHeight - 2, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++) mvprintw(gameHeight - 2, i, "-");
        mvprintw(gameHeight - 2, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++) mvprintw(gameHeight - 2, i, "-");
        mvprintw(gameHeight - 2, gameWidth - 1, "+");

        attroff(COLOR_PAIR(1));
    }

    // 2. 큰 눈사람 그리기 (고퀄리티 ASCII Art)
    void drawBigSnowman(int y, int x, bool isComplete)
    {
        if (isComplete)
        {
            // 완성된 눈사람 (모자, 목도리, 단추 디테일 추가)
            attron(COLOR_PAIR(5) | A_BOLD);
            mvprintw(y,     x, "      .---.      ");
            mvprintw(y + 1, x, "     /     \\     "); // 모자 탑
            mvprintw(y + 2, x, "    _/[___]_\\_   "); // 모자 챙
            mvprintw(y + 3, x, "   (  ^ . ^  )   "); // 얼굴
            mvprintw(y + 4, x, "   (  > - <  )   "); // 목도리 매듭
            mvprintw(y + 5, x, "  / .-------. \\  "); // 몸통 상단
            mvprintw(y + 6, x, " /  :   :   :  \\ "); // 몸통 중단 (단추)
            mvprintw(y + 7, x, "(   '...'...'   )"); // 몸통 하단
            mvprintw(y + 8, x, " '-------------' ");
            attroff(COLOR_PAIR(5) | A_BOLD);
        }
        else
        {
            // 녹아있는 눈사람 (물웅덩이와 모자만 둥둥)
            attron(COLOR_PAIR(5));
            mvprintw(y + 5, x, "      ~   ~      ");
            mvprintw(y + 6, x, "    _/[___]_\\_   "); // 바닥에 떨어진 모자
            mvprintw(y + 7, x, "   ( ~ . ~   )   "); // 녹아가는 얼굴
            mvprintw(y + 8, x, "  (___________)  "); // 웅덩이
            attroff(COLOR_PAIR(5));
        }
    }

    // 3. 작은 눈사람 점수판 (직관적인 아이콘 형태)
    void drawLifeSnowmen(int y, int x, int count)
    {
        attron(COLOR_PAIR(2)); // YELLOW
        // 타이틀
        mvprintw(y, x, "[ COLLECTION ]");
        
        // 눈사람 10개를 두 줄로 배치 (5개씩) -> 공간 활용 Up
        for (int i = 0; i < 10; i++)
        {
            int drawY = y + 2 + (i / 5) * 2; // 2줄로 나눔
            int drawX = x + (i % 5) * 6;     // 간격 넓힘

            if (i < count) {
                attron(A_BOLD);
                mvprintw(drawY, drawX, " (8) "); // 획득한 눈사람 (진하게)
                attroff(A_BOLD);
            } else {
                mvprintw(drawY, drawX, " ( ) "); // 빈 자리
            }
        }
        attroff(COLOR_PAIR(2));
    }

    // 4. 텍스트 정보 출력 (점수판 제거, 시간만 표시)
    void drawInfoPanel()
    {
        attron(COLOR_PAIR(5));
        int startX = gameAreaWidth + 2;
        
        // 시간은 중요하니까 잘 보이게 배치
        mvprintw(4, startX, "TIME REMAINING: %s", gameManager->getFormattedTime().c_str());
        
        // 하단 상태바 메시지
        if (gameManager->isTimeUp()) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(gameHeight - 1, 2, "TIME UP! Press ESC to return.");
            attroff(COLOR_PAIR(4) | A_BOLD);
        } else {
            mvprintw(gameHeight - 1, 2, "Playing... ESC: Menu | TAB: Next Input");
        }
        attroff(COLOR_PAIR(5));
    }

    // 5. 입력창 그리기
    void drawInputArea()
    {
        attron(COLOR_PAIR(3));
        int startRow = 22; // 눈사람이 커졌으니 입력창을 조금 내림
        int startX = gameAreaWidth + 2;

        mvprintw(startRow - 2, startX, "[ WORD INPUT ]");
        
        const auto &userInputs = sentenceManager->getInputHandler()->getUserInputs();
        int currentIdx = sentenceManager->getInputHandler()->getCurrentInputIndex();

        for (int i = 0; i < 8 && i < (int)userInputs.size(); i++)
        {
            if (i == currentIdx)
            {
                attron(COLOR_PAIR(2) | A_BOLD); 
                mvprintw(startRow + i, startX, "> %s_", userInputs[i].c_str());
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            else
            {
                mvprintw(startRow + i, startX, "  %s", userInputs[i].c_str());
            }
        }
        attroff(COLOR_PAIR(3));
    }

    // 6. 배경 효과
    void drawBackgroundEffect()
    {
        attron(COLOR_PAIR(3));
        // 일단은 장식용 눈송이만 찍어둡니다.
        // 나중에 sentenceManager->getFallingObjects() 로직이 생기면 교체하세요!
        mvprintw(5, 10, "*");
        mvprintw(8, 25, ".");
        mvprintw(12, 15, "*");
        mvprintw(15, 40, "*");
        mvprintw(20, 5, ".");
        mvprintw(10, 50, "~");
        mvprintw(22, 55, "*");
        
        attroff(COLOR_PAIR(3));
    }

public:
    PlayScreen(int level) : currentLevel(level), gameWidth(120), gameHeight(50), gameRunning(true),
                            gameAreaWidth(60), scoreAreaWidth(58)
    {
        setlocale(LC_ALL, "");
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0);

        if (has_colors())
        {
            start_color();
            init_pair(1, COLOR_WHITE, COLOR_BLUE);   
            init_pair(2, COLOR_YELLOW, COLOR_BLACK); 
            init_pair(3, COLOR_WHITE, COLOR_BLACK);  
            init_pair(4, COLOR_RED, COLOR_BLACK);    
            init_pair(5, COLOR_CYAN, COLOR_BLACK);   
        }

        resizeterm(gameHeight, gameWidth);
        printf("\033[8;%d;%dt", gameHeight, gameWidth);
        fflush(stdout);
        clear();
        refresh();

        gameManager = new GameManager(currentLevel);
        sentenceManager = new SentenceManager();
        gameManager->startGame();
    }

    ~PlayScreen()
    {
        delete gameManager;
        delete sentenceManager;
        endwin();
    }

    void resizeTerminal(int width, int height)
    {
        resizeterm(height, width);
        clear();
    }

    // 필수 함수 구현 (누락 방지)
    void resizeScreen() override
    {
        resizeterm(gameHeight, gameWidth);
        clear();
    }

    // ---------------------------------------------------------
    //  메인 화면 업데이트 함수 (Public)
    // ---------------------------------------------------------
    void UpdateScreen() override
    {
        clear();

        // 1. 데이터 업데이트
        gameManager->updateTime();
        if (gameManager->checkGameEnd())
        {
            gameRunning = false;
        }

        // 2. 프레임(테두리) 그리기
        drawFrame();

        // 3. 배경 및 단어 그리기 (테스트용 커닝페이퍼 포함)
        drawBackgroundEffect();
        
        // 4. UI 정보 그리기
        drawInfoPanel();
        drawInputArea();

        // 5. [핵심 수정] 눈사람 시스템 연동
        
        // 현재 맞춘 단어 개수 (예: 'bright' 하나 맞추면 1)
        int currentMatches = sentenceManager->getCorrectMatches();
        // 전체 단어 개수 (예: 문장이 8단어면 8)
        int totalWords = sentenceManager->getTargetWords().size();

        // 뚱뚱이 조건: 현재 맞춘 개수와 전체 개수가 같아야 함 (다 맞춤!)
        // (단어가 0개일 때 완성되는 버그 방지 위해 totalWords > 0 조건 추가)
        bool isBigSnowmanComplete = (totalWords > 0 && currentMatches == totalWords);
        

        // 큰 눈사람 그리기
        drawBigSnowman(12, gameAreaWidth + 18, isBigSnowmanComplete);

        // 작은 눈사람 그리기 (여기가 중요! 실시간 정답 개수만큼 그림)
        drawLifeSnowmen(6, gameAreaWidth + 2, currentMatches);

        refresh();
    }

    void shapeScreen() override {
        printf("Game screen shaped for level %d\n", currentLevel);
    }

    void runPlayScreen()
    {
        int key;
        while (gameRunning)
        {
            UpdateScreen();
            timeout(100);
            key = ::getch();

            if (key != ERR)
            {
                switch (key)
                {
                case 27: // ESC
                    gameRunning = false;
                    break;
                case '\t': // TAB
                case KEY_DOWN:
                    sentenceManager->getInputHandler()->nextInput();
                    break;
                case KEY_UP:
                    sentenceManager->getInputHandler()->previousInput();
                    break;
                default:
                    if (sentenceManager->getInputHandler()->handleInput(key))
                        sentenceManager->checkAnswers();
                    break;
                }
            }
        }

        gameManager->endGame();
        clear();
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(gameHeight / 2 - 3, gameWidth / 2 - 15, "GAME OVER");
        mvprintw(gameHeight / 2 - 1, gameWidth / 2 - 20, "Final Score: %d", gameManager->getTotalScore());
        mvprintw(gameHeight / 2 + 3, gameWidth / 2 - 15, "Press any key to exit...");
        attroff(COLOR_PAIR(1) | A_BOLD);
        refresh();
        timeout(-1);
        ::getch();
        endwin();
    }
};

class InitialScreen : public Screen
{
private:
    int selectedLevel;
    bool playButtonPressed;

public:
    InitialScreen() : selectedLevel(1), playButtonPressed(false)
    {
        setlocale(LC_ALL, "");
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0);

        if (has_colors())
        {
            start_color();
            init_pair(1, COLOR_CYAN, COLOR_BLACK);
            init_pair(2, COLOR_YELLOW, COLOR_BLACK);
            init_pair(3, COLOR_WHITE, COLOR_BLACK);
        }
    }

    ~InitialScreen() { endwin(); }

    void UpdateScreen() override
    {
        clear();
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(2, 15, "  _____ _   _  _____  _    _   __  __          _   _ ");
        mvprintw(3, 15, " / ____| \\ | |/ _ \\ \\| |  | | |  \\/  |   /\\   | \\ | |");
        mvprintw(4, 15, "| (___ |  \\| | | | | | |  | | | |\\/| |  /  \\  |  \\| |");
        mvprintw(5, 15, " \\___ \\| . ` | | | | | |/\\| | | |  | | / /\\ \\ | . ` |");
        mvprintw(6, 15, " ____) | |\\  | |_| | \\  /\\  / | |  | |/ ____ \\| |\\  |");
        mvprintw(7, 15, "|_____/|_| \\_|\\___/ \\_\\/  \\/ |_|  |_/_/    \\_\\_| \\_|");
        attroff(COLOR_PAIR(1) | A_BOLD);

        attron(COLOR_PAIR(1));
        mvprintw(9, 20, "*** WELCOME TO SNOW MAN GAME ***");
        attroff(COLOR_PAIR(1));

        mvprintw(12, 30, "SELECT LEVEL:");

        for (int i = 1; i <= 3; i++)
        {
            if (selectedLevel == i)
            {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(13 + i, 25, ">>> [%d] Level %d - %s <<<", i, i, (i == 1 ? "Easy" : i == 2 ? "Medium" : "Hard"));
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(13 + i, 29, "[%d] Level %d - %s", i, i, (i == 1 ? "Easy" : i == 2 ? "Medium" : "Hard"));
                attroff(COLOR_PAIR(3));
            }
        }
        attron(COLOR_PAIR(3));
        mvprintw(18, 32, "[P] PLAY GAME");
        mvprintw(19, 34, "[Q] QUIT");
        attroff(COLOR_PAIR(3));
        refresh();
    }

    void resizeScreen() override {}
    void shapeScreen() override {}

    void runInitialScreen()
    {
        int key;
        while (!playButtonPressed)
        {
            UpdateScreen();
            key = ::getch();
            switch (key)
            {
            case KEY_UP:
                if (selectedLevel > 1) selectedLevel--;
                break;
            case KEY_DOWN:
                if (selectedLevel < 3) selectedLevel++;
                break;
            case '1': selectedLevel = 1; break;
            case '2': selectedLevel = 2; break;
            case '3': selectedLevel = 3; break;
            case 'P': case 'p':
                playButtonPressed = true;
                {
                    endwin();
                    PlayScreen *pScreen = new PlayScreen(selectedLevel);
                    pScreen->runPlayScreen();
                    delete pScreen;
                    initscr(); noecho(); cbreak(); keypad(stdscr, TRUE); curs_set(0);
                    if(has_colors()) { start_color(); }
                    playButtonPressed = false;
                }
                break;
            case 'Q': case 'q':
                endwin();
                exit(0);
                break;
            }
        }
    }
};

#endif
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>
#include <cctype>
#include <ctime>
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
    int gameAreaWidth;                // 게임 영역 폭
    int scoreAreaWidth;               // 점수판 영역 폭
    GameManager *gameManager;         // GameManager 추가
    SentenceManager *sentenceManager; // SentenceManager 추가
    std::string noticeMessage;
    time_t noticeTimestamp;

public:
    PlayScreen(int level) : currentLevel(level), gameWidth(120), gameHeight(50), gameRunning(true),
                            gameAreaWidth(60), scoreAreaWidth(58),
                            snowmanCompleted(false), snowmanCompletedTime(0), showCompletedSnowman(false)
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
            init_pair(1, COLOR_WHITE, COLOR_BLUE);   // 헤더
            init_pair(2, COLOR_YELLOW, COLOR_BLACK); // 눈사람
            init_pair(3, COLOR_WHITE, COLOR_BLACK);  // 눈송이
            init_pair(4, COLOR_RED, COLOR_BLACK);    // 목표물
            init_pair(5, COLOR_CYAN, COLOR_BLACK);   // 점수판
            init_pair(6, COLOR_GREEN, COLOR_BLACK);  // 단어 블록
            init_pair(7, COLOR_MAGENTA, COLOR_BLACK); // 아이템 박스
        }

        resizeterm(gameHeight, gameWidth);
        printf("\033[8;%d;%dt", gameHeight, gameWidth);
        fflush(stdout);
        clear();
        refresh();

        gameManager = new GameManager(currentLevel);
        sentenceManager = new SentenceManager();
        sentenceManager->createWordBlocks(gameAreaWidth - 2);
        gameManager->startGame();
        noticeMessage = "";
        noticeTimestamp = 0;
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

        // 단어 블록 이동 (1초 간격)
        if (gameManager->shouldUpdateWordBlocks())
        {
            int landed = sentenceManager->advanceWordBlocks(gameHeight - 3);
            if (landed > 0)
            {
                int penalty = landed * 10;
                gameManager->adjustTime(-penalty);
                noticeMessage = "Word missed! -" + std::to_string(penalty) + "s";
                noticeTimestamp = time(nullptr);
            }
            sentenceManager->advanceItemBox(gameHeight - 3);
        }

        // 30초마다 아이템 박스 생성
        if (!sentenceManager->isItemActive() && gameManager->shouldSpawnItemBox())
        {
            sentenceManager->createItemBox(gameAreaWidth - 2);
        }

        // 게임 종료 조건 확인
        if (gameManager->checkGameEnd())
        {
            gameRunning = false;
        }

        // 전체 프레임 그리기
        drawFrame();

        // 게임 영역 배경 효과
        drawBackgroundEffect();

        // 게임 영역 내용 (왼쪽) - 배경만
        for (int row = 3; row < gameHeight - 2; row++)
        {
            for (int col = 1; col < gameAreaWidth; col++)
            {
                if (row == 5 || row == gameHeight - 5)
                {
                    if (col % 8 == 0)
                    {
                        attron(COLOR_PAIR(3));
                        mvprintw(row, col, "*"); // 눈송이
                        attroff(COLOR_PAIR(3));
                    }
                }
                else if (col % 15 == 0 && row % 6 == 0)
                {
                    attron(COLOR_PAIR(3));
                    mvprintw(row, col, "~"); // 눈 내리는 효과
                    attroff(COLOR_PAIR(3));
                }
                else if (row == gameHeight - 4 && col % 12 == 0)
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(row, col, "X"); // 목표물
                    attroff(COLOR_PAIR(4));
                }
            }
        }

        // 단어 블록 렌더링 (배경보다 먼저 그려서 덮어씌우기)
        attron(COLOR_PAIR(6) | A_BOLD);
        const auto &wordBlocks = sentenceManager->getWordBlocks();
        for (const auto &block : wordBlocks)
        {
            // active 체크와 화면 범위 체크
            if (block.active && block.getY() >= 3 && block.getY() < gameHeight - 2)
            {
                int blockX = block.getX();
                int blockY = block.getY();

                // 단어가 화면 범위 내에 있는지 확인
                if (blockX >= 1 && blockX + (int)block.word.length() < gameAreaWidth - 1)
                {
                    mvprintw(blockY, blockX, "%s", block.word.c_str());
                }
            }
        }
        attroff(COLOR_PAIR(6) | A_BOLD);

        // 큰 눈사람 그리기 (게임 영역 하단)
        int snowmanY = 22; // 화면 하단으로 조정
        int snowmanX = 20;
        drawBigSnowman(snowmanY, snowmanX, showCompletedSnowman);

        // 오른쪽 영역

        // 1. 상단: 게임 정보 패널
        int rightStartX = gameAreaWidth + 2;
        attron(COLOR_PAIR(5));
        mvprintw(4, rightStartX, "=== GAME INFO ===");
        mvprintw(6, rightStartX, "Level: %d", currentLevel);
        mvprintw(7, rightStartX, "Score: %d", gameManager->getTotalScore());
        mvprintw(8, rightStartX, "Time: %s", gameManager->getFormattedTime().c_str());
        mvprintw(9, rightStartX, "Lives: <3 <3 <3");
        mvprintw(11, rightStartX, "Snow Score: %d", gameManager->getSnowflakeScore());
        mvprintw(12, rightStartX, "Target Score: %d", gameManager->getTargetScore());
        mvprintw(13, rightStartX, "Word Bonus: %d", sentenceManager->getScore());

        // 진행 상황 표시
        mvprintw(15, rightStartX, "Progress:");
        if (showCompletedSnowman)
        {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(16, rightStartX, "SNOWMAN COMPLETE!");
            mvprintw(17, rightStartX, "Great job! +500 pts");
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        else if (gameManager->isWaitingForCompletion())
        {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(16, rightStartX, "Complete sentence!");
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        else
        {
            mvprintw(16, rightStartX, "Words: %d/8", gameManager->getCurrentWordIndex());
        }

        mvprintw(18, rightStartX, "Matches: %d/8", sentenceManager->getCorrectMatches());

            // 중간 점수판 영역 (GameManager 데이터 사용)
            attron(COLOR_PAIR(5));
            int info_row = row - 3;
            switch (info_row)
            {
            case 1:
                mvprintw(row, gameAreaWidth + 2, "Level: %d", currentLevel);
                break;
            case 3:
                mvprintw(row, gameAreaWidth + 2, "Score: %d", gameManager->getTotalScore());
                break;
            case 5:
                mvprintw(row, gameAreaWidth + 2, "Time: %s", gameManager->getFormattedTime().c_str());
                break;
            case 7:
                mvprintw(row, gameAreaWidth + 2, "Lives: <3 <3 <3");
                break;
            case 9:
                mvprintw(row, gameAreaWidth + 2, "-------------------");
                break;
            case 11:
                mvprintw(row, gameAreaWidth + 2, "Snow Score: %d", gameManager->getSnowflakeScore());
                break;
            case 13:
                mvprintw(row, gameAreaWidth + 2, "Target Score: %d", gameManager->getTargetScore());
                break;
            case 15:
                mvprintw(row, gameAreaWidth + 2, "Word Bonus: %d", sentenceManager->getScore());
                break;
            case 17:
                mvprintw(row, gameAreaWidth + 2, "-------------------");
                break;
            case 19:
                if (!noticeMessage.empty() && difftime(time(nullptr), noticeTimestamp) < 5)
                {
                    mvprintw(row, gameAreaWidth + 2, "%s", noticeMessage.c_str());
                }
                else
                {
                    for (int i = gameAreaWidth + 1; i < gameAreaWidth + 30; i++)
                        mvprintw(row, i, " ");
                }
                break;
            case 21:
                mvprintw(row, gameAreaWidth + 2, "Multiplier: x%d", gameManager->getScoreMultiplier());
                break;
            case 23:
                mvprintw(row, gameAreaWidth + 2, "ESC - Back to Menu");
                break;
            case 25:
                mvprintw(row, gameAreaWidth + 2, "TAB - Next Input");
                break;
            default:
                for (int i = gameAreaWidth + 1; i < gameAreaWidth + 30; i++)
                    mvprintw(row, i, " ");
                break;
            }
            attroff(COLOR_PAIR(5));

        // 2. 중단: 작은 눈사람 컬렉션
        int collectionY = 20;
        drawLifeSnowmen(collectionY, rightStartX, sentenceManager->getCorrectMatches());

        // 3. 하단: 입력창
        int inputStartY = 32;
        attron(COLOR_PAIR(3));
        mvprintw(inputStartY, rightStartX, "=== WORD INPUT ===");

        const auto &userInputs = sentenceManager->getInputHandler()->getUserInputs();
        int currentIdx = sentenceManager->getInputHandler()->getCurrentInputIndex();

        for (int i = 0; i < 8; i++)
        {
            if (i == currentIdx && !showCompletedSnowman)
            {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(inputStartY + 2 + i, rightStartX, "[%d] > %s_",
                         i + 1, userInputs[i].c_str());
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            else if (input_row == 18)
            {
                mvprintw(row, gameAreaWidth + 32, "Item Key: %s", sentenceManager->getInputHandler()->getItemBuffer().c_str());
            }
            else if (input_row == 19)
            {
                if (sentenceManager->isItemActive())
                {
                    const auto &item = sentenceManager->getItemBox();
                    mvprintw(row, gameAreaWidth + 32, "Type '%c' + ENTER", item.token);
                }
                else
                {
                    mvprintw(row, gameAreaWidth + 32, "No active item box");
                }
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(inputStartY + 2 + i, rightStartX, "[%d]   %s",
                         i + 1, userInputs[i].c_str());
                attroff(COLOR_PAIR(3));
            }
        }

        // 단어 블록 렌더링
        attron(COLOR_PAIR(6) | A_BOLD);
        for (const auto &block : sentenceManager->getWordBlocks())
        {
            if (block.active && block.y >= 3 && block.y < gameHeight - 2)
            {
                mvprintw(block.y, block.x, "%s", block.word.c_str());
            }
        }
        attroff(COLOR_PAIR(6) | A_BOLD);

        // 아이템 박스 렌더링
        if (sentenceManager->isItemActive())
        {
            const auto &item = sentenceManager->getItemBox();
            attron(COLOR_PAIR(7) | A_BOLD);
            mvprintw(item.y, item.x, "[%c]%s", item.token, item.label.c_str());
            attroff(COLOR_PAIR(7) | A_BOLD);
        }

        // 하단 (ASCII 문자 사용)
        attron(COLOR_PAIR(1));
        mvprintw(gameHeight - 2, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++)
            mvprintw(gameHeight - 2, i, "-");
        mvprintw(gameHeight - 2, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++)
            mvprintw(gameHeight - 2, i, "-");
        mvprintw(gameHeight - 2, gameWidth - 1, "+");
        attroff(COLOR_PAIR(1));

        // 상태 메시지 영역
        if (gameManager->isTimeUp())
        {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(gameHeight - 1, 2, "TIME UP! Final Score: %d | Press ESC to return to menu",
                     gameManager->getTotalScore());
            attroff(COLOR_PAIR(4) | A_BOLD);
        }
        else if (!gameManager->isGameRunning() && gameRunning)
        {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(gameHeight - 1, 2, "Game Complete! Score: %d | Press ESC to return to menu",
                     gameManager->getTotalScore());
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        else
        {
            mvprintw(gameHeight - 1, 2, "Playing... | Remaining: %s | Score: %d | ESC: Back to Menu",
                     gameManager->getFormattedTime().c_str(), gameManager->getTotalScore());
        }

        refresh();
    }

    void shapeScreen() override
    {
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
                    // 아이템 입력 우선 처리
                    if (sentenceManager->isItemActive())
                    {
                        std::string submitted;
                        bool consumed = false;
                        bool submittedItem = sentenceManager->getInputHandler()->handleItemInput(key, submitted, consumed);
                        if (submittedItem)
                        {
                            const auto &item = sentenceManager->getItemBox();
                            if (!submitted.empty() && tolower(submitted[0]) == tolower(item.token))
                            {
                                switch (item.effect)
                                {
                                case ItemEffect::AddTime:
                                    gameManager->adjustTime(10);
                                    noticeMessage = "Item: +10s";
                                    break;
                                case ItemEffect::SubtractTime:
                                    gameManager->adjustTime(-10);
                                    noticeMessage = "Item: -10s";
                                    break;
                                case ItemEffect::DoubleScore:
                                    gameManager->setScoreMultiplier(2);
                                    noticeMessage = "Item: Score x2";
                                    break;
                                }
                            }
                            else
                            {
                                noticeMessage = "Item failed";
                            }
                            noticeTimestamp = time(nullptr);
                            sentenceManager->consumeItemBox();
                        }

                        if (consumed)
                        {
                            break;
                        }
                    }

                    // 다른 키들은 InputHandler로 전달
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
                mvprintw(13 + i, 25, ">>> [%d] Level %d - %s <<<", i, i, (i == 1 ? "Easy" : i == 2 ? "Medium"
                                                                                                   : "Hard"));
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(13 + i, 29, "[%d] Level %d - %s", i, i, (i == 1 ? "Easy" : i == 2 ? "Medium"
                                                                                           : "Hard"));
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
                if (selectedLevel > 1)
                    selectedLevel--;
                break;
            case KEY_DOWN:
                if (selectedLevel < 3)
                    selectedLevel++;
                break;
            case '1':
                selectedLevel = 1;
                break;
            case '2':
                selectedLevel = 2;
                break;
            case '3':
                selectedLevel = 3;
                break;
            case 'P':
            case 'p':
                playButtonPressed = true;
                {
                    endwin();
                    PlayScreen *pScreen = new PlayScreen(selectedLevel);
                    pScreen->runPlayScreen();
                    delete pScreen;
                    initscr();
                    noecho();
                    cbreak();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    if (has_colors())
                    {
                        start_color();
                    }
                    playButtonPressed = false;
                }
                break;
            case 'Q':
            case 'q':
                endwin();
                exit(0);
                break;
            }
        }
    }
};

#endif
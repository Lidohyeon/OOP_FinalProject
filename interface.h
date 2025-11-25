#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Linux/Mac용 키보드 입력
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>

#include "GameManger.h"
#include "SentenceManager.h"

class Screen
{
public:
    virtual ~Screen() {}
    virtual void UpdateScreen() = 0;
    virtual void resizeScreen() = 0;
    virtual void shapeScreen() = 0;
};

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

public:
    PlayScreen(int level) : currentLevel(level), gameWidth(120), gameHeight(50), gameRunning(true),
                            gameAreaWidth(60), scoreAreaWidth(58) // 입력 공간을 위해 영역 조정
    {
        // 로케일 설정
        setlocale(LC_ALL, "");

        // ncurses 초기화
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0); // 커서 숨기기

        // 색상 지원
        if (has_colors())
        {
            start_color();
            init_pair(1, COLOR_WHITE, COLOR_BLUE);   // 헤더
            init_pair(2, COLOR_YELLOW, COLOR_BLACK); // 눈사람
            init_pair(3, COLOR_WHITE, COLOR_BLACK);  // 눈송이
            init_pair(4, COLOR_RED, COLOR_BLACK);    // 목표물
            init_pair(5, COLOR_CYAN, COLOR_BLACK);   // 점수판
            init_pair(6, COLOR_GREEN, COLOR_BLACK);  // 단어 블록
        }

        // 터미널 크기를 정확히 120x50으로 강제 설정
        resizeterm(gameHeight, gameWidth);

        // 터미널 윈도우 자체의 크기도 변경 (macOS/Linux)
        printf("\033[8;%d;%dt", gameHeight, gameWidth);
        fflush(stdout);

        clear();
        refresh();

        // GameManager 및 SentenceManager 생성
        gameManager = new GameManager(currentLevel);
        sentenceManager = new SentenceManager();
        sentenceManager->createWordBlocks(gameAreaWidth - 2);
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

    void UpdateScreen() override
    {
        clear();

        // 게임 시간 업데이트
        gameManager->updateTime();

        // 단어 블록 이동 (1초 간격)
        if (gameManager->shouldUpdateWordBlocks())
        {
            sentenceManager->advanceWordBlocks(gameHeight - 3);
        }

        // 게임 종료 조건 확인
        if (gameManager->checkGameEnd())
        {
            gameRunning = false;
        }

        // 상단 헤더 수정 (3개 영역)
        attron(COLOR_PAIR(1));
        mvprintw(0, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++)
            mvprintw(0, i, "-");
        mvprintw(0, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameAreaWidth + 30; i++)
            mvprintw(0, i, "-");
        mvprintw(0, gameAreaWidth + 30, "+");
        for (int i = gameAreaWidth + 31; i < gameWidth - 1; i++)
            mvprintw(0, i, "-");
        mvprintw(0, gameWidth - 1, "+");

        mvprintw(1, 0, "|");
        mvprintw(1, 2, "SNOW MAN GAME - Level %d", currentLevel);
        mvprintw(1, gameAreaWidth, "|");
        mvprintw(1, gameAreaWidth + 5, "GAME STATUS");
        mvprintw(1, gameAreaWidth + 30, "|");
        mvprintw(1, gameAreaWidth + 35, "WORD INPUT");
        mvprintw(1, gameWidth - 1, "|");

        mvprintw(2, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++)
            mvprintw(2, i, "-");
        mvprintw(2, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameAreaWidth + 30; i++)
            mvprintw(2, i, "-");
        mvprintw(2, gameAreaWidth + 30, "+");
        for (int i = gameAreaWidth + 31; i < gameWidth - 1; i++)
            mvprintw(2, i, "-");
        mvprintw(2, gameWidth - 1, "+");
        attroff(COLOR_PAIR(1));

        // 게임 영역, 점수판, 입력 영역
        for (int row = 3; row < gameHeight - 2; row++)
        {
            mvprintw(row, 0, "|");

            // 왼쪽 게임 영역
            for (int col = 1; col < gameAreaWidth; col++)
            {
                if (row == 25 && col == 30)
                {
                    attron(COLOR_PAIR(2));
                    mvprintw(row, col, "@"); // 눈사람
                    attroff(COLOR_PAIR(2));
                }
                else if (row == 5 || row == gameHeight - 5)
                {
                    if (col % 8 == 0)
                    {
                        attron(COLOR_PAIR(3));
                        mvprintw(row, col, "*"); // 눈송이
                        attroff(COLOR_PAIR(3));
                    }
                    else
                    {
                        mvprintw(row, col, " ");
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
                else
                {
                    mvprintw(row, col, " ");
                }
            }

            mvprintw(row, gameAreaWidth, "|"); // 첫 번째 구분선

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
                mvprintw(row, gameAreaWidth + 2, "ESC - Back to Menu");
                break;
            case 21:
                mvprintw(row, gameAreaWidth + 2, "TAB - Next Input");
                break;
            default:
                for (int i = gameAreaWidth + 1; i < gameAreaWidth + 30; i++)
                    mvprintw(row, i, " ");
                break;
            }
            attroff(COLOR_PAIR(5));

            mvprintw(row, gameAreaWidth + 30, "|"); // 두 번째 구분선

            // 오른쪽 단어 입력 영역
            attron(COLOR_PAIR(3));
            int input_row = row - 3;
            const auto &userInputs = sentenceManager->getInputHandler()->getUserInputs();
            int currentIdx = sentenceManager->getInputHandler()->getCurrentInputIndex();

            if (input_row >= 1 && input_row <= 8)
            {
                int inputIndex = input_row - 1;
                if (inputIndex == currentIdx)
                {
                    attron(COLOR_PAIR(2) | A_BOLD); // 현재 입력 중인 필드 강조
                    mvprintw(row, gameAreaWidth + 32, "[%d] > %s_",
                             inputIndex + 1, userInputs[inputIndex].c_str());
                    attroff(COLOR_PAIR(2) | A_BOLD);
                }
                else
                {
                    mvprintw(row, gameAreaWidth + 32, "[%d]   %s",
                             inputIndex + 1, userInputs[inputIndex].c_str());
                }
            }
            else if (input_row == 10)
            {
                mvprintw(row, gameAreaWidth + 32, "----------------------");
            }
            else if (input_row == 11)
            {
                mvprintw(row, gameAreaWidth + 32, "Completed: %d/8",
                         sentenceManager->getInputHandler()->getCompletedInputsCount());
            }
            else if (input_row == 12)
            {
                mvprintw(row, gameAreaWidth + 32, "Matches: %d",
                         sentenceManager->getCorrectMatches());
            }
            else if (input_row == 14)
            {
                mvprintw(row, gameAreaWidth + 32, "Instructions:");
            }
            else if (input_row == 15)
            {
                mvprintw(row, gameAreaWidth + 32, "Type words and");
            }
            else if (input_row == 16)
            {
                mvprintw(row, gameAreaWidth + 32, "press TAB to move");
            }
            else if (input_row == 17)
            {
                mvprintw(row, gameAreaWidth + 32, "to next input");
            }
            else
            {
                for (int i = gameAreaWidth + 31; i < gameWidth - 1; i++)
                    mvprintw(row, i, " ");
            }
            attroff(COLOR_PAIR(3));

            mvprintw(row, gameWidth - 1, "|");
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
            attron(COLOR_PAIR(4) | A_BOLD); // 빨간색
            mvprintw(gameHeight - 1, 2, "TIME UP! Final Score: %d | Press ESC to return to menu",
                     gameManager->getTotalScore());
            attroff(COLOR_PAIR(4) | A_BOLD);
        }
        else if (!gameManager->isGameRunning() && gameRunning)
        {
            attron(COLOR_PAIR(2) | A_BOLD); // 노란색
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

    void resizeScreen() override
    {
        printf("Game screen resized to %dx%d (Terminal: 120x50)\n", gameWidth, gameHeight);
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

            // 논블로킹 입력을 위해 timeout 설정
            timeout(100); // 100ms 대기
            key = ::getch();

            if (key != ERR)
            { // 키가 입력된 경우만 처리
                switch (key)
                {
                case 27: // ESC - 메뉴로 돌아가기
                    gameRunning = false;
                    break;
                case '\t': // TAB 키 - 다음 입력 필드로 이동
                case KEY_DOWN:
                    sentenceManager->getInputHandler()->nextInput();
                    break;
                case KEY_UP:
                    sentenceManager->getInputHandler()->previousInput();
                    break;
                default:
                    // 다른 키들은 InputHandler로 전달
                    if (sentenceManager->getInputHandler()->handleInput(key))
                    {
                        sentenceManager->checkAnswers(); // 입력 완료 시 답안 체크
                    }
                    break;
                }
            }
        }

        // 게임 종료 시 최종 점수 계산
        gameManager->endGame();

        // 최종 결과 화면 표시
        clear();
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(gameHeight / 2 - 3, gameWidth / 2 - 15, "GAME OVER");
        mvprintw(gameHeight / 2 - 1, gameWidth / 2 - 20, "Final Score: %d", gameManager->getTotalScore());
        mvprintw(gameHeight / 2, gameWidth / 2 - 20, "Time Bonus: %d", gameManager->getTimeBonus());
        mvprintw(gameHeight / 2 + 1, gameWidth / 2 - 20, "Level Bonus: %d", gameManager->getLevelBonus());
        mvprintw(gameHeight / 2 + 3, gameWidth / 2 - 15, "Press any key to continue...");
        attroff(COLOR_PAIR(1) | A_BOLD);
        refresh();

        timeout(-1); // 무한 대기
        ::getch();

        endwin();
        printf("\n게임을 종료하고 메인 메뉴로 돌아갑니다...\n");
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
        // 로케일 설정
        setlocale(LC_ALL, "");

        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0);

        if (has_colors())
        {
            start_color();
            init_pair(1, COLOR_CYAN, COLOR_BLACK);   // 타이틀
            init_pair(2, COLOR_YELLOW, COLOR_BLACK); // 선택된 항목
            init_pair(3, COLOR_WHITE, COLOR_BLACK);  // 일반 텍스트
        }
    }

    ~InitialScreen()
    {
        endwin();
    }

    void UpdateScreen() override
    {
        clear();

        // 타이틀 출력 (ASCII 아트로 변경)
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

        // 레벨 선택 메뉴
        for (int i = 1; i <= 3; i++)
        {
            if (selectedLevel == i)
            {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(13 + i, 25, ">>> [%d] Level %d - %s <<<",
                         i, i, (i == 1 ? "Easy" : i == 2 ? "Medium"
                                                         : "Hard"));
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(13 + i, 29, "[%d] Level %d - %s",
                         i, i, (i == 1 ? "Easy" : i == 2 ? "Medium"
                                                         : "Hard"));
                attroff(COLOR_PAIR(3));
            }
        }

        attron(COLOR_PAIR(3));
        mvprintw(18, 32, "[P] PLAY GAME");
        mvprintw(19, 34, "[Q] QUIT");
        mvprintw(22, 15, "Use 1-3 keys or UP/DOWN arrows to select level, P to play");
        attroff(COLOR_PAIR(3));

        refresh();
    }

    void resizeScreen() override
    {
        printf("Screen resized for initial menu\n");
    }

    void shapeScreen() override
    {
        printf("Initial screen shaped and ready\n");
    }

    // 추가 기능 메서드들
    void selectLevel(int level)
    {
        if (level >= 1 && level <= 3)
        {
            selectedLevel = level;
        }
    }

    int getSelectedLevel() const
    {
        return selectedLevel;
    }

    void pressPlayButton()
    {
        playButtonPressed = true;
    }

    bool isPlayButtonPressed() const
    {
        return playButtonPressed;
    }

    void resetPlayButton()
    {
        playButtonPressed = false;
    }

    // 메인 게임 루프 - Play 버튼이 눌릴 때까지 계속 실행
    void runInitialScreen()
    {
        int key;

        while (!playButtonPressed)
        {
            UpdateScreen();

            key = ::getch(); // ncurses getch() 사용

            switch (key)
            {
            case KEY_UP: // ncurses 키 상수 사용
                if (selectedLevel > 1)
                {
                    selectedLevel--;
                }
                break;
            case KEY_DOWN: // ncurses 키 상수 사용
                if (selectedLevel < 3)
                {
                    selectedLevel++;
                }
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
                pressPlayButton();
                {
                    endwin(); // InitialScreen의 ncurses 종료
                    PlayScreen *pScreen = new PlayScreen(selectedLevel);
                    pScreen->runPlayScreen();
                    delete pScreen;
                    // PlayScreen 종료 후 InitialScreen으로 복귀
                    initscr();
                    noecho();
                    cbreak();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    if (has_colors())
                    {
                        start_color();
                        init_pair(1, COLOR_CYAN, COLOR_BLACK);   // 타이틀
                        init_pair(2, COLOR_YELLOW, COLOR_BLACK); // 선택된 항목
                        init_pair(3, COLOR_WHITE, COLOR_BLACK);  // 일반 텍스트
                    }
                    resetPlayButton();
                }
                break;
            case 'Q':
            case 'q':
                endwin();
                printf("\n                           Thanks for playing!\n");
                exit(0);
                break;
            }
        }
    }
};

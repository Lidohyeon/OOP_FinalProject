#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Linux/Mac용 키보드 입력
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>

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
    int gameAreaWidth;  // 게임 영역 폭
    int scoreAreaWidth; // 점수판 영역 폭

public:
    PlayScreen(int level) : currentLevel(level), gameWidth(120), gameHeight(50), gameRunning(true),
                            gameAreaWidth(85), scoreAreaWidth(33)
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
        }

        // 터미널 크기를 정확히 120x50으로 강제 설정
        resizeterm(gameHeight, gameWidth);

        // 터미널 윈도우 자체의 크기도 변경 (macOS/Linux)
        printf("\033[8;%d;%dt", gameHeight, gameWidth);
        fflush(stdout);

        clear();
        refresh();
    }

    ~PlayScreen()
    {
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

        // 상단 헤더 (ASCII 문자 사용)
        attron(COLOR_PAIR(1));
        mvprintw(0, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++)
            mvprintw(0, i, "-");
        mvprintw(0, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++)
            mvprintw(0, i, "-");
        mvprintw(0, gameWidth - 1, "+");

        mvprintw(1, 0, "|");
        mvprintw(1, 2, "SNOW MAN GAME - Level %d", currentLevel);
        for (int i = 25; i < gameAreaWidth; i++)
            mvprintw(1, i, " ");
        mvprintw(1, gameAreaWidth, "|");
        mvprintw(1, gameAreaWidth + 10, "GAME STATUS");
        for (int i = gameAreaWidth + 21; i < gameWidth - 1; i++)
            mvprintw(1, i, " ");
        mvprintw(1, gameWidth - 1, "|");

        mvprintw(2, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++)
            mvprintw(2, i, "-");
        mvprintw(2, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++)
            mvprintw(2, i, "-");
        mvprintw(2, gameWidth - 1, "+");
        attroff(COLOR_PAIR(1));

        // 게임 영역과 점수판 영역
        for (int row = 3; row < gameHeight - 2; row++)
        {
            mvprintw(row, 0, "|");

            // 왼쪽 게임 영역
            for (int col = 1; col < gameAreaWidth; col++)
            {
                if (row == 25 && col == 42)
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

            mvprintw(row, gameAreaWidth, "|"); // 중간 구분선

            // 오른쪽 점수판 영역
            attron(COLOR_PAIR(5));
            int info_row = row - 3;
            switch (info_row)
            {
            case 1:
                mvprintw(row, gameAreaWidth + 2, "Level: %d", currentLevel);
                break;
            case 3:
                mvprintw(row, gameAreaWidth + 2, "Score: 00000");
                break;
            case 5:
                mvprintw(row, gameAreaWidth + 2, "Lives: <3 <3 <3");
                break;
            case 7:
                mvprintw(row, gameAreaWidth + 2, "Time: 00:00");
                break;
            case 9:
                mvprintw(row, gameAreaWidth + 2, "-------------------------");
                break;
            case 11:
                mvprintw(row, gameAreaWidth + 2, "Snowflakes: 0/10");
                break;
            case 13:
                mvprintw(row, gameAreaWidth + 2, "Targets Hit: 0/5");
                break;
            case 15:
                mvprintw(row, gameAreaWidth + 2, "-------------------------");
                break;
            case 17:
                mvprintw(row, gameAreaWidth + 2, "Controls:");
                break;
            case 18:
                mvprintw(row, gameAreaWidth + 3, "W - Move Up");
                break;
            case 19:
                mvprintw(row, gameAreaWidth + 3, "S - Move Down");
                break;
            case 20:
                mvprintw(row, gameAreaWidth + 3, "A - Move Left");
                break;
            case 21:
                mvprintw(row, gameAreaWidth + 3, "D - Move Right");
                break;
            case 22:
                mvprintw(row, gameAreaWidth + 3, "SPACE - Jump");
                break;
            case 24:
                mvprintw(row, gameAreaWidth + 2, "ESC - Pause");
                break;
            case 25:
                mvprintw(row, gameAreaWidth + 2, "Q - Quit to Menu");
                break;
            case 27:
                mvprintw(row, gameAreaWidth + 2, "-------------------------");
                break;
            case 29:
                mvprintw(row, gameAreaWidth + 2, "Objective:");
                break;
            case 30:
                mvprintw(row, gameAreaWidth + 3, "Collect snowflakes");
                break;
            case 31:
                mvprintw(row, gameAreaWidth + 3, "and reach targets! X");
                break;
            default:
                for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++)
                    mvprintw(row, i, " ");
                break;
            }
            attroff(COLOR_PAIR(5));

            mvprintw(row, gameWidth - 1, "|");
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
        mvprintw(gameHeight - 1, 2, "Terminal Size: %dx%d | Press ESC to pause, Q to quit", gameWidth, gameHeight);

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

            key = ::getch(); // ncurses getch() 사용

            switch (key)
            {
            case 27: // ESC
                mvprintw(gameHeight - 1, 2, "Game Paused! Press any key to continue...");
                refresh();
                ::getch(); // ncurses getch() 사용
                break;
            case 'q':
            case 'Q':
                gameRunning = false;
                endwin();
                printf("\n게임을 종료하고 메인 메뉴로 돌아갑니다...\n");
                break;
            case 'w':
            case 'W':
                // 위로 이동
                break;
            case 's':
            case 'S':
                // 아래로 이동
                break;
            case 'a':
            case 'A':
                // 왼쪽으로 이동
                break;
            case 'd':
            case 'D':
                // 오른쪽으로 이동
                break;
            case ' ':
                // 액션 (점프, 공격 등)
                break;
            }
        }
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

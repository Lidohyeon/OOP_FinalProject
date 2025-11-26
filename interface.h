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
#include <algorithm>
#include <cctype>

#include "GameManger.h"
#include "SentenceManager.h"
#include "ItemBox.h"

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
    int gameAreaWidth;                // 게임 영역 폭 (왼쪽)
    int scoreAreaWidth;               // 점수판 영역 폭 (오른쪽)
    GameManager *gameManager;         // 게임 상태 관리
    SentenceManager *sentenceManager; // 단어 및 문장 관리

    // 눈사람 완성 애니메이션 관련 변수 추가
    bool snowmanCompleted;
    time_t snowmanCompletedTime;
    bool showCompletedSnowman;

    // =========================================================
    // 🎨 [Visual Artist] 화면 그리기 도우미 함수들 (Private)
    // =========================================================

    // 1. 전체 테두리 및 구획 나누기 (안전한 ASCII 문자 버전)
    void drawFrame()
    {
        attron(COLOR_PAIR(1));

        // 상단 가로선 (+와 -로 그리기)
        mvprintw(0, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++) mvprintw(0, i, "-");
        mvprintw(0, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++) mvprintw(0, i, "-");
        mvprintw(0, gameWidth - 1, "+");

        // 상단 제목 영역
        mvprintw(1, 0, "|");
        mvprintw(1, 2, "SNOW MAN GAME - Level %d", currentLevel);
        mvprintw(1, gameAreaWidth, "|");
        mvprintw(1, gameAreaWidth + 2, "Let's Build a Snowman!");
        mvprintw(1, gameWidth - 1, "|");

        // 중간 가로선
        mvprintw(2, 0, "+");
        for (int i = 1; i < gameAreaWidth; i++) mvprintw(2, i, "-");
        mvprintw(2, gameAreaWidth, "+");
        for (int i = gameAreaWidth + 1; i < gameWidth - 1; i++) mvprintw(2, i, "-");
        mvprintw(2, gameWidth - 1, "+");

        // 세로선 그리기 (|로 그리기)
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

    // 2. 큰 눈사람 그리기 (옵션 2: 뚱뚱이 찹쌀떡 스타일)
    void drawBigSnowman(int y, int x, bool isComplete)
    {
        if (isComplete)
        {
            attron(COLOR_PAIR(5) | A_BOLD);
            // 얼굴 (납작하고 귀여움)
            mvprintw(y + 3, x, "       .-------.       ");
            mvprintw(y + 4, x, "      (  ^ _ ^  )      "); // 찡긋

            // 몸통 (푸짐함)
            mvprintw(y + 5, x, "   .--'         '--.   ");
            mvprintw(y + 6, x, " _(        :        )_ "); // 나뭇가지 팔 추가!
            mvprintw(y + 7, x, "(_____________________)");
            attroff(COLOR_PAIR(5) | A_BOLD);
        }
        else
    {
        // 동그랗게 녹은 모습
        attron(COLOR_PAIR(5));
        mvprintw(y + 6, x, "         . . .        ");
        mvprintw(y + 7, x, "      (  x _ x  )    ");
        mvprintw(y + 8, x, "     (___________)   ");
        attroff(COLOR_PAIR(5));
    }
    }
        
    // 3. 작은 눈사람 점수판 (2단 미니 스타일)
    void drawLifeSnowmen(int y, int x, int count)
    {
        attron(COLOR_PAIR(2)); // YELLOW
        mvprintw(y, x+6, "[ COLLECTION ]");

        int maxSnowmen = 8;
        int displayCount = std::min(count, maxSnowmen);

        for (int i = 0; i < maxSnowmen; i++)
        {
            // 2줄 간격(padding)을 활용해 머리와 몸통을 따로 그림
            int drawY = y + 2 + (i / 4) * 3; // 간격을 3칸으로 살짝 늘림
            int drawX = x + (i % 4) * 7;     

            if (i < displayCount)
            {
                attron(A_BOLD);
                mvprintw(drawY,     drawX, "  o  "); // 머리
                mvprintw(drawY + 1, drawX, " (:) "); // 몸통
                attroff(A_BOLD);
            }
            else
            {
                // 빈 자리 표시
                mvprintw(drawY,     drawX, "  .  ");
                mvprintw(drawY + 1, drawX, "  .  ");
            }
        }
        attroff(COLOR_PAIR(2));
    }
    
   // 4. 텍스트 정보 출력 (큰 눈사람 위치 미세 조정 버전)
    void drawInfoPanel()
    {
        attron(COLOR_PAIR(5));
        
        // 오른쪽 패널의 중심점 계산
        int rightPanelStart = gameAreaWidth;
        int rightPanelWidth = gameWidth - gameAreaWidth;
        int centerX = rightPanelStart + (rightPanelWidth / 2); 

        // -----------------------------------------------------------
        // 1. 상단: TIME & ITEM 
        // -----------------------------------------------------------
        
        // 시간 박스
        mvprintw(4, centerX - 11, "+--------------------+");
        mvprintw(5, centerX - 11, "|   TIME REMAINING   |");
        
        attron(A_BOLD);
        std::string timeStr = gameManager->getFormattedTime();
        int timeX = centerX - (timeStr.length() / 2); 
        mvprintw(6, centerX - 11, "|                    |"); 
        mvprintw(6, timeX, "%s", timeStr.c_str());
        attroff(A_BOLD);
        
        mvprintw(7, centerX - 11, "+--------------------+");

        // 아이템 박스
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw(9, centerX - 11, "+--------------------+");
        
        std::string itemMsg;
        if (gameManager->shouldDisplayItemEffect()) {
            itemMsg = gameManager->getLastItemEffectMessage();
        } else {
            itemMsg = "ITEM EFFECT READY";
        }
        
        int msgX = centerX - (itemMsg.length() / 2);
        mvprintw(10, centerX - 11, "|                    |");
        mvprintw(10, msgX, "%s", itemMsg.c_str());
        
        mvprintw(11, centerX - 11, "+--------------------+");
        attroff(COLOR_PAIR(4) | A_BOLD);


        // -----------------------------------------------------------
        // 2. 중단: GAME INFO
        // -----------------------------------------------------------
        const char* divider = "==========================";
        int divX = centerX - 13; 

        mvprintw(13, divX, "%s", divider);
        mvprintw(14, centerX - 5, "GAME INFO"); 
        mvprintw(15, divX, "%s", divider);

        mvprintw(16, divX + 2, "LEVEL: %-2d    SCORE: %-4d", currentLevel, gameManager->getTotalScore());

        if (showCompletedSnowman)
        {
            attron(COLOR_PAIR(2) | A_BOLD);
            std::string msg = "   SNOWMAN COMPLETE!    ";
            mvprintw(17, divX + 1, "%s", msg.c_str());
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        else if (gameManager->isWaitingForCompletion())
        {
            attron(COLOR_PAIR(2) | A_BOLD);
            std::string msg = "   COMPLETE SENTENCE!   ";
            mvprintw(17, divX + 1, "%s", msg.c_str());
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        else
        {
            mvprintw(17, divX + 2, "WORDS: %d/8    MATCH: %d/8", 
                     gameManager->getCurrentWordIndex(), sentenceManager->getCorrectMatches());
        }

        mvprintw(18, divX, "%s", divider);
        attroff(COLOR_PAIR(5));


        // -----------------------------------------------------------
        // 3. 하단: Collection & Big Snowman (위치 미세 조정)
        // -----------------------------------------------------------
        
        // [Collection 중앙 정렬]
        int collectionX = centerX - 13;
        int collectionY = 21;
        drawLifeSnowmen(collectionY, collectionX, gameManager->getCollectedSnowmen());

        // [Big Snowman 왼쪽으로 조금 더 이동]
        // 기존: centerX - 8  -> 수정: centerX - 10 (2칸 더 왼쪽으로)
        int snowmanX = centerX - 10;
        int snowmanY = 27; // 높이 유지
        drawBigSnowman(snowmanY, snowmanX, showCompletedSnowman);


        // -----------------------------------------------------------
        // 4. 최하단: 입력창
        // -----------------------------------------------------------
        int inputStartY = 37; 
        
        attron(COLOR_PAIR(3));
        std::string inputTitle = "======== WORD INPUT ========";
        mvprintw(inputStartY, centerX - (inputTitle.length() / 2), "%s", inputTitle.c_str());

        const auto &userInputs = sentenceManager->getInputHandler()->getUserInputs();
        int currentIdx = sentenceManager->getInputHandler()->getCurrentInputIndex();

        int inputLineX = centerX - 10;

        for (int i = 0; i < 8; i++)
        {
            if (i == currentIdx && !showCompletedSnowman)
            {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(inputStartY + 2 + i, inputLineX, "[%d] > %s_",
                         i + 1, userInputs[i].c_str());
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(inputStartY + 2 + i, inputLineX, "[%d]   %s",
                         i + 1, userInputs[i].c_str());
                attroff(COLOR_PAIR(3));
            }
        }

        // 컨트롤 가이드
        int guideY = gameHeight - 2; 
        std::string guide = "TAB: Next | ESC: Menu | Type 'random' for item";
        mvprintw(guideY, centerX - (guide.length()/2), "%s", guide.c_str());
        
        attroff(COLOR_PAIR(3));

        // 상태 메시지
        if (gameManager->isTimeUp())
        {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(gameHeight - 1, 2, "TIME UP! Score: %d | Press ESC", gameManager->getTotalScore());
            attroff(COLOR_PAIR(4) | A_BOLD);
        }
        else if (!gameManager->isGameRunning() && gameRunning)
        {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(gameHeight - 1, 2, "Complete! Score: %d | Press ESC", gameManager->getTotalScore());
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        else
        {
            mvprintw(gameHeight - 1, 2, "Running... | %s | Score: %d", 
                     gameManager->getFormattedTime().c_str(), gameManager->getTotalScore());
        }

        refresh();
    }

    // 5. 입력창 그리기 (drawInfoPanel에서 통합해서 사용 안 함 - 중복 방지)
    void drawInputArea()
    {
        // drawInfoPanel에서 처리하므로 비워둠 or 삭제 가능
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
        }

        resizeterm(gameHeight, gameWidth);
        printf("\033[8;%d;%dt", gameHeight, gameWidth);
        fflush(stdout);
        clear();
        refresh();

        gameManager = new GameManager(currentLevel);
        sentenceManager = new SentenceManager(currentLevel);
        gameManager->startGame(sentenceManager);
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
        sentenceManager->spawnItemBoxIfNeeded(gameAreaWidth, gameHeight - 3);

        // 눈사람 완성 체크 및 애니메이션 처리
        if (sentenceManager->getCorrectMatches() == 8 && !snowmanCompleted)
        {
            snowmanCompleted = true;
            snowmanCompletedTime = time(nullptr);
            showCompletedSnowman = true;
            gameManager->notifySnowmanComplete();
        }

        // 2초 후 입력칸 초기화 및 눈사람 상태 변경
        if (snowmanCompleted && showCompletedSnowman)
        {
            time_t currentTime = time(nullptr);
            if (difftime(currentTime, snowmanCompletedTime) >= 2.0)
            {
                showCompletedSnowman = false;
                snowmanCompleted = false;
                // 입력칸 모두 비우기
                sentenceManager->getInputHandler()->resetInputs();

                gameManager->prepareNextRound(sentenceManager);
            }
        }

        // 단어 블록 이동 (1초 간격)
        if (gameManager->shouldUpdateWordBlocks())
        {
            sentenceManager->advanceWordBlocks(gameHeight - 3); // maxHeight 전달
            if (sentenceManager->getTimePanalty())
            {
                gameManager->applyTimePenalty();
                sentenceManager->setTimePanalty(false);
            }
            sentenceManager->advanceItemBoxes(gameHeight - 3);
        }

        // 단어 생성 처리 (8개 제한 및 완성 체크)
        gameManager->handleWordGeneration(sentenceManager);

        // 게임 종료 조건 확인
        if (gameManager->checkGameEnd())
        {
            gameRunning = false;
        }

        // 전체 프레임 그리기
        drawFrame();

        // 게임 영역 배경 효과
        drawBackgroundEffect();

        // 아이템 효과 알림 (3초간 강조 표시)
        if (gameManager->shouldDisplayItemEffect())
        {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(4, 2, "*** %s ***", gameManager->getLastItemEffectMessage().c_str());
            attroff(COLOR_PAIR(4) | A_BOLD);
        }

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

        // 아이템 박스 렌더링
        attron(COLOR_PAIR(4) | A_BOLD);
        const auto &itemBoxes = sentenceManager->getItemBoxes();
        for (const auto &box : itemBoxes)
        {
            if (box.getIsActive() && box.getY() >= 3 && box.getY() < gameHeight - 2)
            {
                int boxX = box.getX();
                int boxY = box.getY();

                if (boxX >= 1 && boxX + 2 < gameAreaWidth - 1)
                {
                    mvprintw(boxY, boxX, "[?]");
                }
            }
        }
        attroff(COLOR_PAIR(4) | A_BOLD);

        // 오른쪽 영역 (수정된 drawInfoPanel 호출)
        drawInfoPanel();

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
            timeout(50);
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
                {
                    auto handler = sentenceManager->getInputHandler();
                    int beforeIndex = handler->getCurrentInputIndex();
                    if (handler->handleInput(key))
                    {
                        int usedIndex = beforeIndex;
                        std::string submitted = handler->getInputAt(usedIndex);

                        std::string lowered = submitted;
                        std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
                        if (lowered == "random")
                        {
                            ItemBox::ItemType type;
                            if (sentenceManager->tryUseActiveItemBox(type))
                            {
                                gameManager->applyItemEffect(type);
                                handler->clearInput(usedIndex);
                            }
                        }

                        sentenceManager->checkAnswers();
                    }
                }
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
        mvprintw(7, 15, "|_____/|_| \\_|\\___/ \\_\\/  \\/  |_|  |_/_/    \\_\\_| \\_|");
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
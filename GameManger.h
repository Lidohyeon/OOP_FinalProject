#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <ctime>
#include <string>
#include <vector>
#include "SentenceManager.h"
#include "WordBlock.h"
#include "ItemBox.h"

class GameManager
{
private:
    // 점수 관련
    int totalScore;
    int snowflakeScore; // 눈송이 획득 점수
    int targetScore;    // 목표물 점수
    int timeBonus;      // 시간 보너스
    int levelBonus;     // 레벨 보너스

    int timePenaltySeconds;
    int timeAdjustment; // 아이템 효과로 조정된 시간 (초)
    int scoreMultiplier;
    std::string lastItemEffectMessage;
    time_t lastItemEffectTime;
    int collectedSnowmen;

    // 시간 관련
    time_t startTime;  // 게임 시작 시간
    int timeLimit;     // 제한시간 (초)
    int remainingTime; // 남은 시간
    bool timeUp;       // 시간 초과 여부

    // 게임 상태
    int currentLevel;
    bool gameRunning;

    // 단어 이동 제어
    time_t lastWordRenderTime;
    time_t lastWordCreateTime; // 추가: 단어 생성 시간 추적
    int wordRenderInterval;
    double wordCreateInterval; // 추가: 단어 생성 간격 (0.5초)

    // 단어 생성 제어 추가
    int currentWordIndex;       // 현재 생성 중인 단어 인덱스 (0-7)   // 8개 단어 모두 생성 완료 여부
    bool waitingForCompletion;  // 완성 대기 중인지
    std::vector<int> wordOrder; // 랜덤 순서로 생성할 단어 인덱스 배열

    // 점수 계산 상수
    static const int SNOWFLAKE_POINTS = 100;
    static const int TARGET_POINTS = 500;
    static const int TIME_BONUS_MULTIPLIER = 10;
    static const int LEVEL_BONUS_BASE = 1000;

public:
    // 생성자
    GameManager(int level) : currentLevel(level), totalScore(0), snowflakeScore(0),
                             targetScore(0), timeBonus(0), levelBonus(0),
                             gameRunning(false), timeUp(false),
                             wordRenderInterval(1),
                             lastWordRenderTime(0),
                             lastWordCreateTime(0),
                             wordCreateInterval(3.0), // 0.5초에서 3초로 변경
                             currentWordIndex(0),
                             timePenaltySeconds(0),
                             timeAdjustment(0),
                             scoreMultiplier(1),
                             lastItemEffectTime(0),
                             waitingForCompletion(false),
                             collectedSnowmen(0)
    {
        // 레벨에 따른 제한시간 설정
        switch (level)
        {
        case 1:
            timeLimit = 180;

            break; // 3분
        case 2:
            timeLimit = 150;

            break; // 2분 30초
        case 3:
            timeLimit = 120;

            break; // 2분
        default:
            timeLimit = 180;
            break;
        }

        // 랜덤 시드 초기화
        srand(static_cast<unsigned>(time(nullptr)));

        // 단어 순서 초기화 (0-7)
        initializeWordOrder();
    }

    // 랜덤 단어 순서 생성
    void initializeWordOrder()
    {
        wordOrder.clear();
        // 0-7 인덱스를 순서대로 추가
        for (int i = 0; i < 8; i++)
        {
            wordOrder.push_back(i);
        }

        // Fisher-Yates 셔플 알고리즘으로 랜덤하게 섞기
        for (int i = 7; i > 0; i--)
        {
            int j = rand() % (i + 1);
            std::swap(wordOrder[i], wordOrder[j]);
        }
    }

    // 게임 시작
    void startGame(SentenceManager *sentencemanager)
    {

        startTime = time(nullptr);
        gameRunning = true;
        timeUp = false;
        totalScore = 0;
        snowflakeScore = 0;
        targetScore = 0;
        timeBonus = 0;
        levelBonus = currentLevel * LEVEL_BONUS_BASE;
        lastWordRenderTime = startTime;
        lastWordCreateTime = startTime;
        timePenaltySeconds = 0;
        timeAdjustment = 0;
        scoreMultiplier = 1;
        collectedSnowmen = 0;

        // 초기화
        currentWordIndex = 0;

        waitingForCompletion = false;

        // 새로운 랜덤 순서 생성
        initializeWordOrder();

        // 첫 번째 단어 블록 즉시 생성 (랜덤 순서의 첫 번째)
        sentencemanager->createWordBlock(60, wordOrder[currentWordIndex]);
        currentWordIndex++;
    }

    // 시간 업데이트 및 카운트다운
    void updateTime()
    {

        if (!gameRunning)
            return;

        time_t currentTime = time(nullptr);
        int elapsedTime = (int)(currentTime - startTime);
        remainingTime = timeLimit - elapsedTime - timePenaltySeconds + timeAdjustment;

        if (remainingTime <= 0)
        {
            remainingTime = 0;
            timeUp = true;
            gameRunning = false;
        }
    }

    // 점수 관련 메서드
    void addSnowflakeScore()
    {
        snowflakeScore += SNOWFLAKE_POINTS;
        updateTotalScore();
    }

    void addTargetScore()
    {
        targetScore += TARGET_POINTS;
        updateTotalScore();
    }

    // 시간 감소 (객체가 바닥에 떨어졌을 때 페널티)
    void applyTimePenalty(int seconds = 10)
    {
        if (!gameRunning)
            return;

        timePenaltySeconds += seconds;
        updateTime();

        // 시간이 0 이하가 되면 게임 종료
        if (remainingTime <= 0)
        {
            remainingTime = 0;
            timeUp = true;
            gameRunning = false;
        }
    }

    void calculateTimeBonus()
    {
        if (remainingTime > 0)
        {
            timeBonus = remainingTime * TIME_BONUS_MULTIPLIER;
            updateTotalScore();
        }
    }

    void updateTotalScore()
    {
        totalScore = targetScore * scoreMultiplier;
    }

    // Getter 메서드들
    int getTotalScore() const { return totalScore; }
    int getSnowflakeScore() const { return snowflakeScore; }
    int getTargetScore() const { return targetScore; }
    int getTimeBonus() const { return timeBonus; }
    int getLevelBonus() const { return levelBonus; }

    int getRemainingTime() const { return remainingTime; }
    int getTimeLimit() const { return timeLimit; }
    int getTimeAdjustment() const { return timeAdjustment; }
    bool isTimeUp() const { return timeUp; }
    bool isGameRunning() const { return gameRunning; }

    // 시간 포맷팅 (MM:SS 형식)
    std::string getFormattedTime() const
    {
        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        return std::string(buffer);
    }

    // 게임 종료
    void endGame()
    {
        gameRunning = false;
        calculateTimeBonus();
    }

    // 게임 상태 확인
    bool checkGameEnd()
    {
        updateTime();
        return timeUp || !gameRunning;
    }

    bool shouldUpdateWordBlocks()
    {
        time_t now = time(nullptr);
        if (difftime(now, lastWordRenderTime) >= wordRenderInterval)
        {
            lastWordRenderTime = now;
            return true;
        }
        return false;
    }

    bool shouldCreateWordBlock()
    {
        // 이미 8개 모두 생성했거나 완성 대기 중이면 생성하지 않음
        if (waitingForCompletion)
        {
            return false;
        }

        time_t now = time(nullptr);
        if (difftime(now, lastWordCreateTime) >= wordCreateInterval)
        {
            lastWordCreateTime = now;
            return true;
        }
        return false;
    }

    // 단어 생성 처리 (interface.h에서 호출)
    bool handleWordGeneration(SentenceManager *sentenceManager)
    {

        if (shouldCreateWordBlock())
        {
            // 미리 섞어둔 순서대로 단어 생성
            currentWordIndex = currentWordIndex % 8;
            int wordIndexToCreate = wordOrder[currentWordIndex];
            sentenceManager->createWordBlock(58, wordIndexToCreate);
            currentWordIndex++;

            return true;
        }

        // correctMatches가 8이 되면 새로운 문장으로 넘어가기
        if (!waitingForCompletion && sentenceManager->getCorrectMatches() == 8)
        {

            waitingForCompletion = true;

            return true;
        }

        return false;
    }
    void notifySnowmanComplete()
    {
        // 이미 waitingForCompletion이면 무시
        if (waitingForCompletion)
            return;

        waitingForCompletion = true;
        collectedSnowmen++;
    }
    void prepareNextRound(SentenceManager *sentenceManager)
    {
        // 새로운 문장 로드
        sentenceManager->loadRandomSentence(currentLevel);

        // 상태 완전 초기화
        currentWordIndex = 0;
        waitingForCompletion = false;

        // 입력칸 초기화
        sentenceManager->getInputHandler()->resetInputs();

        // 단어 블록 초기화
        auto &blocks = sentenceManager->getWordBlocks();
        blocks.clear();

        // 새로운 랜덤 순서 생성
        initializeWordOrder();

        // 점수 추가
        addTargetScore();

        // 첫 단어 블록 생성
        lastWordCreateTime = time(nullptr);
    }

    void applyItemEffect(ItemBox::ItemType type)
    {
        switch (type)
        {
        case ItemBox::ItemType::TIME_BONUS:
            timeAdjustment += 10;
            lastItemEffectMessage = "TIME +10 SECONDS!";
            break;
        case ItemBox::ItemType::TIME_MINUS:
            timeAdjustment -= 10;
            lastItemEffectMessage = "TIME -10 SECONDS!";
            break;
        case ItemBox::ItemType::SCORE_BOOST:
            scoreMultiplier = 2;
            lastItemEffectMessage = "SCORE MULTIPLIED!";
            break;
        default:
            break;
        }

        lastItemEffectTime = time(nullptr);

        updateTime();
        updateTotalScore();

        if (remainingTime <= 0)
        {
            remainingTime = 0;
            timeUp = true;
            gameRunning = false;
        }
    }

    // Getter 추가
    int getCurrentWordIndex() const { return currentWordIndex; }

    bool shouldDisplayItemEffect(double durationSeconds = 3.0) const
    {
        if (lastItemEffectTime == 0)
        {
            return false;
        }
        return difftime(time(nullptr), lastItemEffectTime) < durationSeconds;
    }

    const std::string &getLastItemEffectMessage() const { return lastItemEffectMessage; }

    bool isWaitingForCompletion() const { return waitingForCompletion; }

    int getCollectedSnowmen() const { return collectedSnowmen; }
};

#endif // GAMEMANAGER_H

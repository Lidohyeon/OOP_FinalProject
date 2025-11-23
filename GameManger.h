#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <ctime>
#include <string>

class GameManager
{
private:
    // 점수 관련
    int totalScore;
    int snowflakeScore; // 눈송이 획득 점수
    int targetScore;    // 목표물 점수
    int timeBonus;      // 시간 보너스
    int levelBonus;     // 레벨 보너스

    // 시간 관련
    time_t startTime;  // 게임 시작 시간
    int timeLimit;     // 제한시간 (초)
    int remainingTime; // 남은 시간
    bool timeUp;       // 시간 초과 여부

    // 게임 상태
    int currentLevel;
    bool gameRunning;

    // 점수 계산 상수
    static const int SNOWFLAKE_POINTS = 100;
    static const int TARGET_POINTS = 500;
    static const int TIME_BONUS_MULTIPLIER = 10;
    static const int LEVEL_BONUS_BASE = 1000;

public:
    // 생성자
    GameManager(int level) : currentLevel(level), totalScore(0), snowflakeScore(0),
                             targetScore(0), timeBonus(0), levelBonus(0),
                             gameRunning(false), timeUp(false)
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
    }

    // 게임 시작
    void startGame()
    {
        startTime = time(nullptr);
        gameRunning = true;
        timeUp = false;
        totalScore = 0;
        snowflakeScore = 0;
        targetScore = 0;
        timeBonus = 0;
        levelBonus = currentLevel * LEVEL_BONUS_BASE;
    }

    // 시간 업데이트 및 카운트다운
    void updateTime()
    {
        if (!gameRunning)
            return;

        time_t currentTime = time(nullptr);
        int elapsedTime = (int)(currentTime - startTime);
        remainingTime = timeLimit - elapsedTime;

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
        if (!gameRunning) return;
        
        remainingTime -= seconds;
        
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
        totalScore = snowflakeScore + targetScore + timeBonus + levelBonus;
    }

    // Getter 메서드들
    int getTotalScore() const { return totalScore; }
    int getSnowflakeScore() const { return snowflakeScore; }
    int getTargetScore() const { return targetScore; }
    int getTimeBonus() const { return timeBonus; }
    int getLevelBonus() const { return levelBonus; }

    int getRemainingTime() const { return remainingTime; }
    int getTimeLimit() const { return timeLimit; }
    bool isTimeUp() const { return timeUp; }
    bool isGameRunning() const { return gameRunning; }

    // 시간 포맷팅 (MM:SS 형식)
    std::string getFormattedTime() const
    {
        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;
        char buffer[6];
        sprintf(buffer, "%02d:%02d", minutes, seconds);
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

    // 레벨 완료 시 호출
    void completeLevel()
    {
        endGame();
        calculateTimeBonus();
    }
};

#endif // GAMEMANAGER_H

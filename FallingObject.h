#ifndef FALLINGOBJECT_H
#define FALLINGOBJECT_H

#include <cstdlib>
#include <ctime>

// 추상 클래스 - 떨어지는 모든 객체의 기본 클래스
class FallingObject
{
protected:
    int x;                      // x 좌표 (가로)
    int y;                      // y 좌표 (세로)
    int initialX;               // 초기 x 좌표 (삭제 시 복원용)
    int initialY;               // 초기 y 좌표 (삭제 시 복원용)
    bool isActive;              // 활성 상태 (화면에 보이는지)
    double fallSpeed;           // 낙하 속도
    int gameAreaWidth;          // 게임 영역 폭 (경계 체크용)
    int gameAreaHeight;         // 게임 영역 높이 (경계 체크용)
    bool hasReachedBottom;      // 바닥에 도달했는지

    // 랜덤 시드 초기화 헬퍼
    static void initRandomSeed()
    {
        static bool seeded = false;
        if (!seeded) {
            srand(static_cast<unsigned int>(time(nullptr)));
            seeded = true;
        }
    }

public:
    // 생성자
    FallingObject(int areaWidth, int areaHeight, double speed = 1.0)
        : gameAreaWidth(areaWidth), gameAreaHeight(areaHeight), 
          fallSpeed(speed), isActive(true), hasReachedBottom(false)
    {
        initRandomSeed();
        y = 3; // 헤더 아래부터 시작
        initialY = y;
    }

    virtual ~FallingObject() {}

    // 생성되면 물체를 내려오게끔 하는 함수 (순수 가상 함수)
    virtual void fall()
    {
        if (!isActive) return;

        y += static_cast<int>(fallSpeed);

        // 바닥에 도달했는지 체크
        if (y >= gameAreaHeight - 3) {
            if (!hasReachedBottom) {
                hasReachedBottom = true;
            }
            y = gameAreaHeight - 3; // 바닥 위치 고정
        }
    }

    // 화면에 그리기 (순수 가상 함수 - 각 클래스에서 구현)
    virtual void draw() const = 0;

    // 바닥 도달 여부 체크
    bool checkAndResetBottomReached()
    {
        if (hasReachedBottom && isActive) {
            return true;
        }
        return false;
    }

    // Getter 메서드들
    int getX() const { return x; }
    int getY() const { return y; }
    bool getIsActive() const { return isActive; }
    bool getHasReachedBottom() const { return hasReachedBottom; }

    // Setter 메서드들
    void setPosition(int newX, int newY) 
    { 
        x = newX; 
        y = newY; 
    }

    void setActive(bool active) { isActive = active; }
    void setFallSpeed(double speed) { fallSpeed = speed; }

    // 리셋
    virtual void reset()
    {
        x = initialX;
        y = initialY;
        isActive = true;
        hasReachedBottom = false;
    }
};

#endif // FALLINGOBJECT_H

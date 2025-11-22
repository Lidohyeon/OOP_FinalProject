#ifndef FALLINGOBJECT_H
#define FALLINGOBJECT_H

#include <string>
#include <cstdlib>
#include <ctime>

class FallingObject
{
protected:
    std::string word;           // 떨어지는 단어
    int x;                      // x 좌표 (가로)
    int y;                      // y 좌표 (세로)
    int initialX;               // 초기 x 좌표 (삭제 시 복원용)
    int initialY;               // 초기 y 좌표 (삭제 시 복원용)
    bool isActive;              // 활성 상태 (화면에 보이는지)
    bool isInInput;             // 입력창에 입력되었는지
    double fallSpeed;           // 낙하 속도
    int gameAreaWidth;          // 게임 영역 폭 (경계 체크용)
    int gameAreaHeight;         // 게임 영역 높이 (경계 체크용)
    bool hasReachedBottom;      // 바닥에 도달했는지

public:
    // 생성자 - 생성되면 화면의 랜덤한 부분으로부터 시작해 내려오기 시작함
    FallingObject(const std::string& w, int areaWidth, int areaHeight, double speed = 1.0)
        : word(w), gameAreaWidth(areaWidth), gameAreaHeight(areaHeight), 
          fallSpeed(speed), isActive(true), isInInput(false), hasReachedBottom(false)
    {
        // 랜덤 시드 초기화 (한 번만 필요)
        static bool seeded = false;
        if (!seeded) {
            srand(static_cast<unsigned int>(time(nullptr)));
            seeded = true;
        }

        // 화면 상단에서 랜덤한 x 위치에 생성 (단어 길이 고려)
        x = rand() % (gameAreaWidth - static_cast<int>(word.length()) - 2) + 1;
        y = 3; // 헤더 아래부터 시작

        // 초기 위치 저장
        initialX = x;
        initialY = y;
    }

    virtual ~FallingObject() {}

    // 생성되면 물체를 내려오게끔 하는 함수
    virtual void movingDownward()
    {
        if (!isActive || isInInput) return;

        y += static_cast<int>(fallSpeed);

        // 바닥에 도달했는지 체크
        if (y >= gameAreaHeight - 3) {
            hasReachedBottom = true;
            y = gameAreaHeight - 3; // 바닥 위치 고정
        }
    }

    // 해당 눈의 단어가 입력되면 화면에서 없어지는 함수
    virtual void enteredInInput()
    {
        if (!isActive) return;
        
        isInInput = true;
        isActive = false; // 화면에서 숨김
    }

    // 해당 눈의 단어가 입력 칸에서 삭제될 때 다시 원래의 위치로 반환되는 함수
    virtual void deletedInput()
    {
        if (!isInInput) return;

        isInInput = false;
        isActive = true;
        
        // 원래 위치로 복원
        x = initialX;
        y = initialY;
        hasReachedBottom = false;
    }

    // Getter 메서드들
    const std::string& getWord() const { return word; }
    int getX() const { return x; }
    int getY() const { return y; }
    bool getIsActive() const { return isActive; }
    bool getIsInInput() const { return isInInput; }
    bool getHasReachedBottom() const { return hasReachedBottom; }

    // Setter 메서드들
    void setPosition(int newX, int newY) 
    { 
        x = newX; 
        y = newY; 
    }

    void setActive(bool active) { isActive = active; }
    void setFallSpeed(double speed) { fallSpeed = speed; }

    // 충돌/매칭 체크용
    bool matchesWord(const std::string& input) const
    {
        return word == input;
    }

    // 디버깅용
    void reset()
    {
        x = initialX;
        y = initialY;
        isActive = true;
        isInInput = false;
        hasReachedBottom = false;
    }
};

#endif // FALLINGOBJECT_H

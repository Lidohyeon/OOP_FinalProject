#ifndef WORDBLOCK_H
#define WORDBLOCK_H

#include "FallingObject.h"
#include <string>
#include <algorithm>
#include <cctype>

// WordBlock: 단어 블록
class WordBlock : public FallingObject
{
private:
    std::string text; // 표시될 단어
    int orderIndex;   // 문장 내 순서 (0~7)
    bool isInInput;   // 입력창에 입력되었는지

public:
    // 기본 생성자 추가 (SentenceManager와의 호환성)
    WordBlock() : FallingObject(60, 45, 1.0), text(""), orderIndex(0), isInInput(false)
    {
        x = 0;
        y = 3;
        initialX = x;
        initialY = y;
    }

    WordBlock(const std::string &word, int order, int areaWidth, int areaHeight, double speed = 1.0)
        : FallingObject(areaWidth, areaHeight, speed), text(word), orderIndex(order), isInInput(false)
    {
        // 화면 상단에서 랜덤한 x 위치에 생성 (단어 길이 고려)
        x = rand() % (gameAreaWidth - static_cast<int>(text.length()) - 2) + 1;
        y = 3; // 게임 영역 상단
        initialX = x;
        initialY = y;
    }

    // 부모 클래스의 순수 가상 함수 구현
    void draw() const override
    {
        if (!isActive)
            return;
        // ncurses로 그리기는 PlayScreen에서 처리
    }

    // fall() 메서드 오버라이드 (추가 로직 포함)
    void fall() override
    {
        if (!isActive || isInInput)
            return;

        // 부모 클래스의 fall() 호출
        FallingObject::fall();

        // active 속성 동기화
        active = isActive;
    }

    // reset() 메서드 오버라이드
    void reset() override
    {
        FallingObject::reset();
        isInInput = false;
        active = isActive;
    }

    // 단어가 입력되면 화면에서 없어짐
    void enteredInInput()
    {
        if (!isActive)
            return;
        isInInput = true;
        isActive = false;
        active = false;
    }

    // 입력 칸에서 삭제되면 원래 위치로 복원
    void deletedInput()
    {
        if (!isInInput)
            return;
        isInInput = false;
        isActive = true;
        active = true;
        x = initialX;
        y = initialY;
        hasReachedBottom = false;
    }

    // Getter 메서드들
    const std::string &getText() const { return text; }
    int getOrderIndex() const { return orderIndex; }
    bool getIsInInput() const { return isInInput; }

    // 매칭 체크 (대소문자 구분 없음)
    bool matchesWord(const std::string &input) const
    {
        std::string inputLower = input;
        std::string textLower = text;

        std::transform(inputLower.begin(), inputLower.end(), inputLower.begin(), ::tolower);
        std::transform(textLower.begin(), textLower.end(), textLower.begin(), ::tolower);

        return inputLower == textLower;
    }

    // 호환성을 위한 public 멤버들
    std::string word; // text와 동기화
    bool active;      // isActive와 동기화

    // 위치 접근 메서드들
    int getXPos() const { return x; }
    int getYPos() const { return y; }

    // word와 active 동기화를 위한 업데이트 함수
    void syncProperties()
    {
        word = text;
        active = isActive;
    }

    // text 설정 함수 추가
    void setText(const std::string &w)
    {
        text = w;
        word = w;
    }

    // 위치 설정 함수들
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    void setActive(bool isActive)
    {
        this->isActive = isActive;
        this->active = isActive;
    }
};

#endif // WORDBLOCK_H

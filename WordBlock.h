#ifndef WORDBLOCK_H
#define WORDBLOCK_H

#include "FallingObject.h"
#include <string>

// WordBlock: 단어 블록
class WordBlock : public FallingObject
{
private:
    std::string text;           // 표시될 단어
    int orderIndex;             // 문장 내 순서 (0~7)
    bool isInInput;             // 입력창에 입력되었는지

public:
    WordBlock(const std::string& word, int order, int areaWidth, int areaHeight, double speed = 1.0)
        : FallingObject(areaWidth, areaHeight, speed), text(word), orderIndex(order), isInInput(false)
    {
        // 화면 상단에서 랜덤한 x 위치에 생성 (단어 길이 고려)
        x = rand() % (gameAreaWidth - static_cast<int>(text.length()) - 2) + 1;
        initialX = x;
    }

    // 화면에 그리기
    void draw() const override
    {
        if (!isActive) return;
        // ncurses로 그리기 (PlayScreen에서 호출)
        // mvprintw(y, x, "%s", text.c_str());
    }

    // 단어가 입력되면 화면에서 없어짐
    void enteredInInput()
    {
        if (!isActive) return;
        isInInput = true;
        isActive = false;
    }

    // 입력 칸에서 삭제되면 원래 위치로 복원
    void deletedInput()
    {
        if (!isInInput) return;
        isInInput = false;
        isActive = true;
        x = initialX;
        y = initialY;
        hasReachedBottom = false;
    }

    // Getter
    const std::string& getText() const { return text; }
    int getOrderIndex() const { return orderIndex; }
    bool getIsInInput() const { return isInInput; }

    // 매칭 체크
    bool matchesWord(const std::string& input) const
    {
        return text == input;
    }

    void reset() override
    {
        FallingObject::reset();
        isInInput = false;
    }
};

#endif // WORDBLOCK_H

#ifndef ITEMBOX_H
#define ITEMBOX_H

#include "FallingObject.h"
#include <string>

// ItemBox: 아이템 박스
class ItemBox : public FallingObject
{
public:
    enum class ItemType {
        EXTRA_LIFE,     // 목숨 +1
        TIME_BONUS,     // 시간 +10초
        SLOW_MOTION,    // 낙하 속도 감소
        SCORE_BOOST     // 점수 2배
    };

private:
    ItemType itemType;
    char displayChar;   // 화면에 표시될 문자 ('L', 'T', 'S', 'B')

public:
    ItemBox(int areaWidth, int areaHeight, double speed = 0.8)
        : FallingObject(areaWidth, areaHeight, speed)
    {
        // 랜덤 아이템 타입 결정
        itemType = static_cast<ItemType>(rand() % 4);
        
        // 아이템 타입에 따라 표시 문자 설정
        switch (itemType) {
            case ItemType::EXTRA_LIFE:
                displayChar = 'L'; // Life
                break;
            case ItemType::TIME_BONUS:
                displayChar = 'T'; // Time
                break;
            case ItemType::SLOW_MOTION:
                displayChar = 'S'; // Slow
                break;
            case ItemType::SCORE_BOOST:
                displayChar = 'B'; // Boost
                break;
        }

        // 랜덤 x 위치 (아이템 박스는 3칸 폭: [?])
        x = rand() % (gameAreaWidth - 4) + 1;
        initialX = x;
    }

    // 화면에 그리기
    void draw() const override
    {
        if (!isActive) return;
        // ncurses로 그리기 (PlayScreen에서 호출)
        // mvprintw(y, x, "[%c]", displayChar);
    }

    // 랜덤 효과 적용 (GameManager와 연동)
    ItemType applyRandomEffect()
    {
        isActive = false; // 획득 시 비활성화
        return itemType;
    }

    // Getter
    ItemType getItemType() const { return itemType; }
    char getDisplayChar() const { return displayChar; }

    // 아이템 효과 설명 문자열
    std::string getEffectDescription() const
    {
        switch (itemType) {
            case ItemType::EXTRA_LIFE:
                return "Extra Life +1";
            case ItemType::TIME_BONUS:
                return "Time +10 sec";
            case ItemType::SLOW_MOTION:
                return "Slow Motion";
            case ItemType::SCORE_BOOST:
                return "Score x2";
            default:
                return "Unknown";
        }
    }
};

#endif // ITEMBOX_H

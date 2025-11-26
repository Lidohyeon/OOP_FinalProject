#ifndef ITEMBOX_H
#define ITEMBOX_H

#include "FallingObject.h"
#include <string>

// ItemBox: 아이템 박스
class ItemBox : public FallingObject
{
public:
    enum class ItemType
    {

        TIME_BONUS, // 시간 +10초
        TIME_MINUS, // 시간 -10초
        SCORE_BOOST // 점수 2배
    };

private:
    ItemType itemType;

public:
    ItemBox(int areaWidth, int areaHeight, double speed = 0.8)
        : FallingObject(areaWidth, areaHeight, speed)
    {
        // 랜덤 아이템 타입 결정
        itemType = static_cast<ItemType>(rand() % 3);

        // 랜덤 x 위치 (아이템 박스는 3칸 폭: [?])
        x = rand() % (gameAreaWidth - 4) + 1;
        initialX = x;
    }

    // 낙하 (바닥 도달 시 페널티 없음)
    void fall() override
    {
        if (!isActive)
            return;

        y += static_cast<int>(fallSpeed);

        // 바닥에 도달하면 그냥 사라짐 (페널티 없음)
        if (y >= gameAreaHeight - 3)
        {
            isActive = false; // 비활성화
            y = gameAreaHeight - 3;
            // hasReachedBottom은 설정하지 않음 → 페널티 적용 안됨
        }
    }

    // 화면에 그리기
    void draw() const override
    {
        if (!isActive)
            return;
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

    // 아이템 효과 설명 문자열
    std::string getEffectDescription() const
    {
        switch (itemType)
        {

        case ItemType::TIME_BONUS:
            return "Time +10 sec";
        case ItemType::TIME_MINUS:
            return "Time -10 sec";
        case ItemType::SCORE_BOOST:
            return "Score x2";
        default:
            return "Unknown";
        }
    }
};

#endif // ITEMBOX_H

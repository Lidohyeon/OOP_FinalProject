#ifndef SENTENCEMANAGER_H
#define SENTENCEMANAGER_H

#include <ctime>
#include <string>
#include <vector>
#include <ncurses.h>

struct WordBlock
{
    std::string word;
    int x;
    int y;
    bool active;
};

enum class ItemEffect
{
    AddTime,
    SubtractTime,
    DoubleScore
};

struct ItemBox
{
    char token;
    std::string label;
    ItemEffect effect;
    int x;
    int y;
    bool active;
};

class InputHandler
{
private:
    std::vector<std::string> userInputs;
    int currentInputIndex;
    std::string currentInput;
    static const int MAX_INPUTS = 8;
    static const int MAX_INPUT_LENGTH = 20;
    bool inputComplete;
    std::string itemBuffer;

public:
    InputHandler() : currentInputIndex(0), inputComplete(false)
    {
        userInputs.resize(MAX_INPUTS, "");
    }

    bool handleInput(int key);
    bool handleItemInput(int key, std::string &submitted, bool &consumed);
    void resetInputs();
    void nextInput();
    void previousInput();

    // Getter 메서드들
    const std::vector<std::string> &getUserInputs() const { return userInputs; }
    int getCurrentInputIndex() const { return currentInputIndex; }
    const std::string &getCurrentInput() const { return currentInput; }
    const std::string &getItemBuffer() const { return itemBuffer; }
    bool isInputComplete() const { return inputComplete; }

    // 입력 상태 확인
    bool allInputsComplete() const;
    int getCompletedInputsCount() const;
};

class SentenceManager
{
private:
    InputHandler *inputHandler;
    std::vector<std::string> targetWords;
    int correctMatches;
    std::vector<WordBlock> wordBlocks;
    int wordAreaWidth;
    ItemBox itemBox;
    bool hasItem;

public:
    SentenceManager() : correctMatches(0), hasItem(false)
    {
        inputHandler = new InputHandler();
        initializeTargetWords();
        wordAreaWidth = 0;
    }

    ~SentenceManager()
    {
        delete inputHandler;
    }

    void initializeTargetWords();
    void checkAnswers();
    void createWordBlocks(int maxWidth);
    int advanceWordBlocks(int maxHeight);
    void createItemBox(int maxWidth);
    void advanceItemBox(int maxHeight);
    const std::vector<WordBlock> &getWordBlocks() const { return wordBlocks; }
    bool isItemActive() const { return hasItem && itemBox.active; }
    const ItemBox &getItemBox() const { return itemBox; }
    void consumeItemBox() { hasItem = false; }

    int getScore() const { return correctMatches * 100; }

    InputHandler *getInputHandler() const { return inputHandler; }
    const std::vector<std::string> &getTargetWords() const { return targetWords; }
    int getCorrectMatches() const { return correctMatches; }
};

#endif // SENTENCEMANAGER_H

/* // 예시 문장들 8개의 단어들로만 구성된.
The bright morning sun warmed the quiet village.

She finally found courage to follow her dreams.

They walked together along the peaceful riverside path.

His sudden smile completely changed her gloomy mood.

The old library smelled like memories and stories.

We shared warm coffee during the rainy afternoon.

The kitten slept safely beside its loving mother.

He learned patience through countless failures and challenges.

Our journey began under a sky full of stars.

The gentle wind carried whispers across the valley.

She protected everything she loved with quiet strength.

A single message unexpectedly changed his entire life.

They celebrated success under lights glowing like fireflies.

Her soft laughter echoed through the empty hallway.

We found hope again after surviving the storm.

The warm candlelight flickered softly in the darkness.

He trusted her completely despite their difficult past.

Nature healed their hearts with calm gentle beauty.

She discovered truth hidden beneath the forgotten letters.

The small miracle happened when they least expected.
*/
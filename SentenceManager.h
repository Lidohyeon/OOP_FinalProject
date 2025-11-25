#ifndef SENTENCEMANAGER_H
#define SENTENCEMANAGER_H

#include <ctime>
#include <string>
#include <vector>
#include <ncurses.h>
#include "Dictionary.h"
#include "WordBlock.h"

// WordBlock 구조체/클래스 정의 제거 (WordBlock.h에서 정의되므로)

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

    // 추가: 개별 입력 필드 조작 메서드
    bool isWordCorrect(int index, const std::string &target) const;
    void clearInput(int index);
    std::string getInputAt(int index) const;
};

class SentenceManager
{
private:
    InputHandler *inputHandler;
    Dictionary *dictionary;
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
        delete dictionary; // 추가: Dictionary 메모리 해제
    }

    // 수정: initializeTargetWords() 삭제, 대신 loadSentenceForLevel() 사용
    void loadSentenceForLevel(int level, int sentenceIndex);

    // 추가: 랜덤 문장 로드
    void loadRandomSentence(int level);

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

    // 추가: 레벨 및 문장 정보 접근
    int getCurrentLevel() const { return dictionary->getCurrentLevel(); }
    int getCurrentSentenceIndex() const { return dictionary->getCurrentSentenceIndex(); }

    // 추가: 전체 문장 반환 (화면에 표시용)
    std::string getFullSentence() const;

    // 추가: Dictionary 접근 (필요 시)
    Dictionary *getDictionary() const { return dictionary; }

    bool getTimePanalty() const { return timePanalty; };
    void setTimePanalty(bool result) { timePanalty = result; };
};

#endif // SENTENCEMANAGER_H
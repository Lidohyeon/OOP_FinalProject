#ifndef SENTENCEMANAGER_H
#define SENTENCEMANAGER_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "Dictionary.h"  // 추가: Dictionary 헤더 포함

class InputHandler
{
private:
    std::vector<std::string> userInputs;
    int currentInputIndex;
    std::string currentInput;
    static const int MAX_INPUTS = 8;
    static const int MAX_INPUT_LENGTH = 20;
    bool inputComplete;

public:
    InputHandler() : currentInputIndex(0), inputComplete(false)
    {
        userInputs.resize(MAX_INPUTS, "");
    }

    bool handleInput(int key);
    void resetInputs();
    void nextInput();
    void previousInput();

    // Getter 메서드들
    const std::vector<std::string> &getUserInputs() const { return userInputs; }
    int getCurrentInputIndex() const { return currentInputIndex; }
    const std::string &getCurrentInput() const { return currentInput; }
    bool isInputComplete() const { return inputComplete; }

    // 입력 상태 확인
    bool allInputsComplete() const;
    int getCompletedInputsCount() const;
    
    // 추가: 개별 입력 필드 조작 메서드
    bool isWordCorrect(int index, const std::string& target) const;
    void clearInput(int index);
    std::string getInputAt(int index) const;
};

class SentenceManager
{
private:
    InputHandler *inputHandler;
    Dictionary *dictionary;          // 추가: Dictionary 객체 포인터
    std::vector<std::string> targetWords;
    int correctMatches;
    int currentLevel;                // 추가: 현재 레벨
    int currentSentenceIndex;        // 추가: 현재 문장 인덱스

public:
    // ✨ 수정: 생성자에 level 매개변수 추가
    SentenceManager(int level);
    
    ~SentenceManager()
    {
        delete inputHandler;
        delete dictionary;           // 추가: Dictionary 메모리 해제
    }

    // 수정: initializeTargetWords() 삭제, 대신 loadSentenceForLevel() 사용
    void loadSentenceForLevel(int level, int sentenceIndex);
    
    // 추가: 랜덤 문장 로드
    void loadRandomSentence(int level);
    
    void checkAnswers();
    int getScore() const { return correctMatches * 100; }

    InputHandler *getInputHandler() const { return inputHandler; }
    const std::vector<std::string> &getTargetWords() const { return targetWords; }
    int getCorrectMatches() const { return correctMatches; }
    
    // 추가: 레벨 및 문장 정보 접근
    int getCurrentLevel() const { return currentLevel; }
    int getCurrentSentenceIndex() const { return currentSentenceIndex; }
    
    // 추가: 전체 문장 반환 (화면에 표시용)
    std::string getFullSentence() const;
    
    // 추가: Dictionary 접근 (필요 시)
    Dictionary* getDictionary() const { return dictionary; }
};

#endif // SENTENCEMANAGER_H
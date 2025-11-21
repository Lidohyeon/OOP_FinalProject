#ifndef SENTENCEMANAGER_H
#define SENTENCEMANAGER_H

#include <string>
#include <vector>
#include <ncurses.h>

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
};

class SentenceManager
{
private:
    InputHandler *inputHandler;
    std::vector<std::string> targetWords;
    int correctMatches;

public:
    SentenceManager() : correctMatches(0)
    {
        inputHandler = new InputHandler();
        initializeTargetWords();
    }

    ~SentenceManager()
    {
        delete inputHandler;
    }

    void initializeTargetWords();
    void checkAnswers();
    int getScore() const { return correctMatches * 100; }

    InputHandler *getInputHandler() const { return inputHandler; }
    const std::vector<std::string> &getTargetWords() const { return targetWords; }
    int getCorrectMatches() const { return correctMatches; }
};

#endif // SENTENCEMANAGER_H

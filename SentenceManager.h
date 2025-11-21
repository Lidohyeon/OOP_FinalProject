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
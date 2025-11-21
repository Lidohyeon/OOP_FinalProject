#include "SentenceManager.h"
#include <algorithm>

bool InputHandler::handleInput(int key)
{
    switch (key)
    {
    case KEY_BACKSPACE:
    case 127:
    case 8:
        if (!userInputs[currentInputIndex].empty())
        {
            userInputs[currentInputIndex].pop_back();
        }
        break;
    case '\n':
    case '\r':
    case KEY_ENTER:
        if (currentInputIndex < MAX_INPUTS - 1)
        {
            currentInputIndex++;
        }
        return true;
    default:
        if (key >= 32 && key <= 126 && userInputs[currentInputIndex].length() < MAX_INPUT_LENGTH)
        {
            userInputs[currentInputIndex] += (char)key;
        }
        break;
    }
    return false;
}

void InputHandler::resetInputs()
{
    for (auto &input : userInputs)
    {
        input.clear();
    }
    currentInputIndex = 0;
    inputComplete = false;
}

void InputHandler::nextInput()
{
    if (currentInputIndex < MAX_INPUTS - 1)
    {
        currentInputIndex++;
    }
}

void InputHandler::previousInput()
{
    if (currentInputIndex > 0)
    {
        currentInputIndex--;
    }
}

bool InputHandler::allInputsComplete() const
{
    for (const auto &input : userInputs)
    {
        if (input.empty())
        {
            return false;
        }
    }
    return true;
}

int InputHandler::getCompletedInputsCount() const
{
    int count = 0;
    for (const auto &input : userInputs)
    {
        if (!input.empty())
        {
            count++;
        }
    }
    return count;
}

void SentenceManager::initializeTargetWords()
{
    targetWords = {
        "bright", "morning", "courage", "dreams",
        "peaceful", "smile", "library", "stories"};
}

void SentenceManager::checkAnswers()
{
    correctMatches = 0;
    const auto &userInputs = inputHandler->getUserInputs();

    for (size_t i = 0; i < userInputs.size() && i < targetWords.size(); i++)
    {
        std::string userWord = userInputs[i];
        std::string targetWord = targetWords[i];

        std::transform(userWord.begin(), userWord.end(), userWord.begin(), ::tolower);
        std::transform(targetWord.begin(), targetWord.end(), targetWord.begin(), ::tolower);

        if (userWord == targetWord)
        {
            correctMatches++;
        }
    }
}

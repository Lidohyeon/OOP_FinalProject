#include "SentenceManager.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

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

bool InputHandler::handleItemInput(int key, std::string &submitted, bool &consumed)
{
    consumed = false;
    switch (key)
    {
    case KEY_BACKSPACE:
    case 127:
    case 8:
        if (!itemBuffer.empty())
        {
            itemBuffer.pop_back();
            consumed = true;
        }
        break;
    case '\n':
    case '\r':
    case KEY_ENTER:
        submitted = itemBuffer;
        itemBuffer.clear();
        consumed = true;
        return true;
    default:
        if (key >= 32 && key <= 126)
        {
            if (itemBuffer.size() < 1)
            {
                itemBuffer = std::string(1, static_cast<char>(key));
                consumed = true;
            }
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
    itemBuffer.clear();
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

void SentenceManager::createWordBlocks(int maxWidth)
{
    wordBlocks.clear();
    wordAreaWidth = maxWidth;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int minX = 2;
    int maxX = wordAreaWidth - 10; // 단어 길이를 고려한 여유 공간

    for (const auto &word : targetWords)
    {
        WordBlock block;
        block.word = word;
        block.y = 3; // 게임 영역 첫 줄
        block.x = minX + (std::rand() % std::max(1, maxX - minX));
        block.active = true;
        wordBlocks.push_back(block);
    }
}

int SentenceManager::advanceWordBlocks(int maxHeight)
{
    bool anyActive = false;
    int landedBlocks = 0;

    for (auto &block : wordBlocks)
    {
        if (!block.active)
        {
            continue;
        }

        anyActive = true;
        if (block.y < maxHeight)
        {
            block.y += 1;
        }
        else
        {
            block.active = false;
            landedBlocks++;
        }
    }

    if (!anyActive && wordAreaWidth > 0 && correctMatches < (int)targetWords.size())
    {
        createWordBlocks(wordAreaWidth);
    }

    return landedBlocks;
}

void SentenceManager::createItemBox(int maxWidth)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    hasItem = true;
    itemBox.active = true;
    itemBox.y = 3;
    int minX = 2;
    int maxX = maxWidth - 6;
    itemBox.x = minX + (std::rand() % std::max(1, maxX - minX));

    int effectPick = std::rand() % 3;
    switch (effectPick)
    {
    case 0:
        itemBox.effect = ItemEffect::AddTime;
        itemBox.token = 'H';
        itemBox.label = "+10s";
        break;
    case 1:
        itemBox.effect = ItemEffect::SubtractTime;
        itemBox.token = 'C';
        itemBox.label = "-10s";
        break;
    default:
        itemBox.effect = ItemEffect::DoubleScore;
        itemBox.token = 'X';
        itemBox.label = "Scorex2";
        break;
    }
}

void SentenceManager::advanceItemBox(int maxHeight)
{
    if (!hasItem || !itemBox.active)
    {
        return;
    }

    if (itemBox.y < maxHeight)
    {
        itemBox.y += 1;
    }
    else
    {
        itemBox.active = false;
        hasItem = false;
    }
}

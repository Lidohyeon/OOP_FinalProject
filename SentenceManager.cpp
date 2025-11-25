#include "SentenceManager.h"
#include <algorithm>
#include <cstdlib>

// ========== InputHandler 구현 ==========
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

// 추가: 개별 단어 정확도 체크
bool InputHandler::isWordCorrect(int index, const std::string &target) const
{
    if (index < 0 || index >= static_cast<int>(userInputs.size()))
    {
        return false;
    }

    std::string userWord = userInputs[index];
    std::string targetWord = target;

    // 대소문자 구분 없이 비교
    std::transform(userWord.begin(), userWord.end(), userWord.begin(), ::tolower);
    std::transform(targetWord.begin(), targetWord.end(), targetWord.begin(), ::tolower);

    return userWord == targetWord;
}

// 추가: 특정 입력 필드 초기화
void InputHandler::clearInput(int index)
{
    if (index >= 0 && index < static_cast<int>(userInputs.size()))
    {
        userInputs[index].clear();
    }
}

// 추가: 특정 입력 필드 값 반환
std::string InputHandler::getInputAt(int index) const
{
    if (index >= 0 && index < static_cast<int>(userInputs.size()))
    {
        return userInputs[index];
    }
    return "";
}

// ========== SentenceManager 구현 (수정됨) ==========

// 수정: 생성자에서 Dictionary 생성 및 초기 문장 로드

// 삭제: initializeTargetWords() 함수 제거
// 이유: Dictionary에서 동적으로 로드하므로 하드코딩된 단어 배열 불필요

// 새로 추가: 특정 레벨의 특정 문장 로드
void SentenceManager::loadSentenceForLevel(int level, int sentenceIndex)
{
    // 레벨 유효성 검사
    if (level < 1 || level > 3)
    {
        level = 1; // 기본값
    }

    // 문장 인덱스 유효성 검사
    int sentenceCount = dictionary->getSentenceCount(level);
    if (sentenceIndex < 0 || sentenceIndex >= sentenceCount)
    {
        sentenceIndex = 0; // 기본값
    }

    // Dictionary에서 단어 가져오기
    targetWords = dictionary->getWordsForLevel(level, sentenceIndex);

    // 현재 상태 업데이트
    currentLevel = level;
    currentSentenceIndex = sentenceIndex;

    // 입력 초기화
    inputHandler->resetInputs();
    correctMatches = 0;
}

// 새로 추가: 랜덤 문장 로드 (게임 시작 시 사용)
void SentenceManager::loadRandomSentence(int level)
{
    if (level < 1 || level > 3)
    {
        level = 1;
    }

    // Dictionary의 랜덤 문장 선택 기능 사용
    targetWords = dictionary->getRandomSentenceWords(level);

    currentLevel = level;
    currentSentenceIndex = dictionary->getCurrentSentenceIndex();

    inputHandler->resetInputs();
    correctMatches = 0;
}

// 수정: 답안 체크 로직 강화
void SentenceManager::checkAnswers()
{
    correctMatches = 0;
    const auto &userInputs = inputHandler->getUserInputs();

    // targetWords와 userInputs 개수가 다르면 경고
    if (userInputs.size() != targetWords.size())
    {
        return; // 안전 장치
    }

    for (size_t i = 0; i < userInputs.size() && i < targetWords.size(); i++)
    {
        std::string userWord = userInputs[i];
        std::string targetWord = targetWords[i];

        // 대소문자 구분 없이 비교
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

void SentenceManager::advanceWordBlocks(int maxHeight)
{
    bool anyActive = false;

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
        }
    }

    if (!anyActive && wordAreaWidth > 0)
    {
        createWordBlocks(wordAreaWidth);
    }
}

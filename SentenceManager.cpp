#include "SentenceManager.h"
#include <algorithm>
#include <cstdlib>
#include "WordBlock.h"
#include "ItemBox.h"

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

void SentenceManager::advanceWordBlocks(int maxHeight)
{
    bool hasReachedBottom = false;

    // 기존 블록들 이동 처리
    for (auto &block : wordBlocks)
    {
        if (!block.active)
        {
            continue;
        }

        // FallingObject의 fall() 함수 호출 - 매 프레임마다 한 칸씩 내려감
        block.fall();

        // active 속성 동기화
        block.active = block.getIsActive();

        // 바닥 도달 체크
        if (block.checkAndResetBottomReached())
        {
            block.active = false;
            hasReachedBottom = true;
        }
    }

    // 바닥에 도달한 블록이 있으면 페널티 설정
    if (hasReachedBottom)
    {
        timePanalty = true;
    }
}

void SentenceManager::advanceItemBoxes(int maxHeight)
{
    (void)maxHeight;

    for (auto &box : itemBoxes)
    {
        if (!box.getIsActive())
        {
            continue;
        }

        box.fall();

        if (!box.getIsActive())
        {
            box.setActive(false);
        }
    }

    itemBoxes.erase(std::remove_if(itemBoxes.begin(), itemBoxes.end(), [](const ItemBox &box)
                                   { return !box.getIsActive(); }),
                    itemBoxes.end());
}

void SentenceManager::createWordBlock(int maxWidth, int wordIndex)
{
    wordAreaWidth = maxWidth;

    // wordIndex가 유효한 범위인지 확인
    if (wordIndex < 0 || wordIndex >= static_cast<int>(targetWords.size()))
    {
        return;
    }

    int minX = 2;
    int maxX = wordAreaWidth - 15; // 단어 길이를 더 여유롭게 고려

    const std::string &word = targetWords[wordIndex];

    // FallingObject를 상속받은 WordBlock 생성
    WordBlock block(word, wordIndex, wordAreaWidth, 45, 1.0);

    // 랜덤 x 위치 설정
    int randomX = minX + (std::rand() % std::max(1, maxX - minX));
    block.setPosition(randomX, 3); // 게임 영역 상단에서 시작
    block.active = true;

    block.syncProperties();

    wordBlocks.push_back(block);
}

void SentenceManager::createItemBox(int maxWidth, int maxHeight)
{
    ItemBox box(maxWidth, maxHeight);
    box.setPosition(rand() % (maxWidth - 4) + 1, 3);
    box.setActive(true);
    itemBoxes.push_back(box);
}

void SentenceManager::spawnItemBoxIfNeeded(int maxWidth, int maxHeight)
{
    time_t now = time(nullptr);
    if (difftime(now, lastItemBoxSpawnTime) >= ITEMBOX_INTERVAL)
    {
        createItemBox(maxWidth, maxHeight);
        lastItemBoxSpawnTime = now;
    }
}

bool SentenceManager::tryUseActiveItemBox(ItemBox::ItemType &typeOut)
{
    for (auto &box : itemBoxes)
    {
        if (box.getIsActive())
        {
            typeOut = box.applyRandomEffect();
            itemBoxes.erase(std::remove_if(itemBoxes.begin(), itemBoxes.end(), [](const ItemBox &b)
                                           { return !b.getIsActive(); }),
                            itemBoxes.end());
            return true;
        }
    }
    return false;
}

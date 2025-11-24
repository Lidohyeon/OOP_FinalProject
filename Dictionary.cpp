#include "Dictionary.h"
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>

Dictionary::Dictionary() : currentLevel(1), currentSentenceIndex(0)
{
    initializeSentences();
    initRandomSeed();
}

void Dictionary::initRandomSeed()
{
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }
}

void Dictionary::initializeSentences()
{
    // ===== Level 1: 쉬운 문장 (7개) =====
    // 짧고 일상적인 단어, 기본 문법 구조
    levelSentences[1] = {
        "The bright morning sun warmed the quiet village",
        "She finally found courage to follow her dreams",
        "They walked together along the peaceful riverside path",
        "The old library smelled like memories and stories",
        "We shared warm coffee during the rainy afternoon",
        "The kitten slept safely beside its loving mother",
        "The gentle wind carried whispers across the valley"
    };
    
    // ===== Level 2: 중간 문장 (7개) =====
    // 약간 복잡한 단어와 문장 구조
    levelSentences[2] = {
        "His sudden smile completely changed her gloomy mood",
        "He learned patience through countless failures and challenges",
        "Our journey began under a sky full of stars",
        "She protected everything she loved with quiet strength",
        "A single message unexpectedly changed his entire life",
        "They celebrated success under lights glowing like fireflies",
        "Her soft laughter echoed through the empty hallway"
    };
    
    // ===== Level 3: 어려운 문장 (6개) =====
    // 추상적이고 복잡한 어휘, 긴 단어
    levelSentences[3] = {
        "We found hope again after surviving the storm",
        "The warm candlelight flickered softly in the darkness",
        "He trusted her completely despite their difficult past",
        "Nature healed their hearts with calm gentle beauty",
        "She discovered truth hidden beneath the forgotten letters",
        "The small miracle happened when they least expected"
    };
}

int Dictionary::getSentenceCount(int level) const
{
    auto it = levelSentences.find(level);
    if (it != levelSentences.end()) {
        return static_cast<int>(it->second.size());
    }
    return 0;
}

std::vector<std::string> Dictionary::splitSentenceIntoWords(const std::string& sentence)
{
    std::vector<std::string> words;
    std::istringstream iss(sentence);
    std::string word;
    
    // 공백 기준으로 단어 분리
    while (iss >> word) {
        // 구두점 제거 (마침표, 쉼표 등)
        word.erase(std::remove_if(word.begin(), word.end(), 
            [](char c) { return c == '.' || c == ',' || c == '!' || c == '?'; }), 
            word.end());
        
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    
    return words;
}

std::vector<std::string> Dictionary::getWordsForLevel(int level, int sentenceIndex)
{
    // 레벨 검증
    if (level < 1 || level > 3) {
        level = 1; // 기본값
    }
    
    // 문장 인덱스 검증
    int sentenceCount = getSentenceCount(level);
    if (sentenceIndex < 0 || sentenceIndex >= sentenceCount) {
        sentenceIndex = 0; // 기본값
    }
    
    // 현재 상태 저장
    currentLevel = level;
    currentSentenceIndex = sentenceIndex;
    
    // 문장 가져오기
    std::string sentence = levelSentences[level][sentenceIndex];
    
    // 단어로 분리
    currentWords = splitSentenceIntoWords(sentence);
    
    return currentWords;
}

std::string Dictionary::getFullSentence(int level, int sentenceIndex) const
{
    // 레벨 검증
    auto it = levelSentences.find(level);
    if (it == levelSentences.end()) {
        return "";
    }
    
    // 인덱스 검증
    const std::vector<std::string>& sentences = it->second;
    if (sentenceIndex < 0 || sentenceIndex >= static_cast<int>(sentences.size())) {
        return "";
    }
    
    return sentences[sentenceIndex];
}

std::vector<std::string> Dictionary::getRandomSentenceWords(int level)
{
    // 레벨 검증
    if (level < 1 || level > 3) {
        level = 1;
    }
    
    int sentenceCount = getSentenceCount(level);
    if (sentenceCount == 0) {
        return std::vector<std::string>();
    }
    
    // 랜덤 문장 선택
    int randomIndex = rand() % sentenceCount;
    
    return getWordsForLevel(level, randomIndex);
}
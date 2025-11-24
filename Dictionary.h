#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <map>

class Dictionary
{
private:
    // 레벨별 문장 저장 (key: 레벨, value: 문장 리스트)
    std::map<int, std::vector<std::string>> levelSentences;
    
    // 현재 로드된 문장의 단어들
    std::vector<std::string> currentWords;
    
    // 현재 레벨과 문장 인덱스
    int currentLevel;
    int currentSentenceIndex;

public:
    // 생성자
    Dictionary();
    
    // 소멸자
    ~Dictionary() {}
    
    // 초기 문장 데이터 로드
    void initializeSentences();
    
    // 레벨별 문장 개수 반환
    int getSentenceCount(int level) const;
    
    // 특정 레벨의 특정 문장을 8개 단어로 분리하여 반환
    std::vector<std::string> getWordsForLevel(int level, int sentenceIndex);
    
    // 특정 레벨의 특정 문장 전체를 반환
    std::string getFullSentence(int level, int sentenceIndex) const;
    
    // 현재 로드된 단어들 반환
    const std::vector<std::string>& getCurrentWords() const { return currentWords; }
    
    // 랜덤으로 레벨에 맞는 문장 선택
    std::vector<std::string> getRandomSentenceWords(int level);
    
    // 현재 레벨 반환
    int getCurrentLevel() const { return currentLevel; }
    
    // 현재 문장 인덱스 반환
    int getCurrentSentenceIndex() const { return currentSentenceIndex; }
    
private:
    // 문장을 단어로 분리하는 헬퍼 함수
    std::vector<std::string> splitSentenceIntoWords(const std::string& sentence);
    
    // 랜덤 시드 초기화
    void initRandomSeed();
};

#endif // DICTIONARY_H
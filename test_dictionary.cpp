#include <iostream>
#include "Dictionary.h"

int main() {
    Dictionary dict;
    
    std::cout << "=== Dictionary Test ===" << std::endl;
    
    // 각 레벨별 테스트
    for (int level = 1; level <= 3; level++) {
        std::cout << "\n[Level " << level << "]" << std::endl;
        std::cout << "Total sentences: " << dict.getSentenceCount(level) << std::endl;
        
        // 첫 번째 문장 테스트
        std::vector<std::string> words = dict.getWordsForLevel(level, 0);
        std::cout << "First sentence words (" << words.size() << "):" << std::endl;
        for (size_t i = 0; i < words.size(); i++) {
            std::cout << "  [" << i << "] " << words[i] << std::endl;
        }
        
        // 전체 문장 출력
        std::cout << "Full sentence: " << dict.getFullSentence(level, 0) << std::endl;
    }
    
    // 랜덤 테스트
    std::cout << "\n=== Random Sentence Test ===" << std::endl;
    std::vector<std::string> randomWords = dict.getRandomSentenceWords(2);
    std::cout << "Random Level 2 sentence:" << std::endl;
    for (const auto& word : randomWords) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
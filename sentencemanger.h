class InputHandler
{
    virtual void isValid(); // 입력한 문장이 옳은 문장인지 확인하는 함수
};

class SentenceManger
{
    virtual void generateSnows(); // 텍스트 파일에서 문장을 불러와 문장들의 단어들로 snow들을 만드는 함수.
};

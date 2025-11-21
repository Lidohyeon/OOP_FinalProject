class FallingObject
{
    FallingObject()
    {
        // 생성자 - 생성되면 화면의 랜덤한 부분으로부터 시작해 내려오기 시작함.
    }
    virtual void movingDownWard(); // 생성되면 물체를 내려오게끔 하는 함수
    virtual void enteredInInput(); // 해당 눈의 단어가 입력되면 화면에서 없어지는
    virtual void deletedInput();   // 해당 눈의 단어가 입력 칸에서 삭제 될때 다시 원래의 위치로 반환되는 함수
};

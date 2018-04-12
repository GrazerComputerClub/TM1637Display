// Functions for 4-Digit display with TM1637 chip
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#ifndef __TM1637Display__
#define __TM1637Display__

#include <map>


// data types
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;


class TM1637Display {
  public:
    static const BYTE cnMaxBrightness   = 7;
    static const BYTE cnDisplayAddress  = 0xC0; //1. digit
    static const BYTE cnDataCmdAutoAddr = 0x40;
    static const BYTE cnDataCmdFixAddr  = 0x44;
    static const BYTE cnDisplayOn       = 0x88; // + B0, B1, B2 = Brightness

  public:
    TM1637Display(BYTE clockPin, BYTE dataPin, BYTE BrightnessPercent);
    ~TM1637Display();
    void Clear();
    void Show(BYTE DigitNumber, const char Data);
    void Show(const char* pData);
    void Show(int Data);
    void SetBrightness(BYTE BrightnessPercent); // 0...100
    void ShowDoublePoint(bool bShow);

  private:
    BYTE m_Brightness;
    bool m_bShowDP;
    BYTE m_CLKPin;
    BYTE m_DIOPin;
    char m_CurrentData[5];
    std::map<BYTE, char> m_Char2SegCode;
    bool m_bACKErr;

  private:
    void writeByte(BYTE data);
    void start(void);
    void stop(void);
    void Init7SegMap();
    BYTE GetSegCode(BYTE DigitNumber, const char Data); //char to binary code for display
    void CLKWait();
};
#endif  // __TM1637Display__



// Functions for 4-Digit display with TM1637 chip
// Code Licence: MIT
// written by m.stroh

#include <wiringPi.h>
#include "TM1637Display.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


namespace TM1637 {
    const int SegA  = 0x01; //0b00000001
    const int SegB  = 0x02; //0b00000010
    const int SegC  = 0x04; //0b00000100
    const int SegD  = 0x08; //0b00001000
    const int SegE  = 0x10; //0b00010000
    const int SegF  = 0x20; //0b00100000
    const int SegG  = 0x40; //0b01000000
    const int SegDP = 0x80; //0b10000000
}

void TM1637Display::Init7SegMap() {
    using namespace TM1637;
 //    - A -
 //   F     B
 //    - G -
 //   E     C
 //    - D -
    m_Char2SegCode['0'] = SegA|SegB|SegC|SegD|SegE|SegF;
    m_Char2SegCode['1'] = SegB|SegC;
    m_Char2SegCode['2'] = SegA|SegB|SegG|SegE|SegD;
    m_Char2SegCode['3'] = SegA|SegB|SegG|SegC|SegD;
    m_Char2SegCode['4'] = SegF|SegG|SegB|SegC;
    m_Char2SegCode['5'] = SegA|SegF|SegG|SegC|SegD;
    m_Char2SegCode['6'] = SegF|SegE|SegG|SegC|SegD;
    m_Char2SegCode['7'] = SegA|SegB|SegC;
    m_Char2SegCode['8'] = SegA|SegB|SegC|SegD|SegE|SegF|SegG;
    m_Char2SegCode['9'] = SegA|SegB|SegC|SegF|SegG;
    m_Char2SegCode['A'] = SegA|SegB|SegC|SegE|SegF|SegG;
    m_Char2SegCode['B'] = SegF|SegE|SegF|SegC|SegD;
    m_Char2SegCode['C'] = SegA|SegF|SegE|SegD;
    m_Char2SegCode['D'] = SegB|SegC|SegG|SegE|SegD;
    m_Char2SegCode['E'] = SegA|SegF|SegG|SegE|SegD;
    m_Char2SegCode['F'] = SegA|SegF|SegG|SegE;
    m_Char2SegCode['H'] = SegF|SegG|SegE|SegB|SegC;
    m_Char2SegCode['I'] = SegE;
    m_Char2SegCode['J'] = SegB|SegC|SegD|SegE;
    m_Char2SegCode['L'] = SegF|SegE|SegD;
    m_Char2SegCode['N'] = SegE|SegG|SegC;
    m_Char2SegCode['O'] = SegE|SegG|SegC|SegD;
    m_Char2SegCode['P'] = SegA|SegB|SegF|SegG|SegE;
    m_Char2SegCode['R'] = SegG|SegE;
    m_Char2SegCode['S'] = SegA|SegF|SegG|SegC|SegD;
    m_Char2SegCode['T'] = SegF|SegG|SegE|SegD;
    m_Char2SegCode['U'] = /*SegF|*/SegE|SegD|/*SegB|*/SegC;
    m_Char2SegCode['Y'] = SegF|SegG|SegB|SegC|SegD;
    m_Char2SegCode['Z'] = SegA|SegB|SegG|SegE|SegD;
    m_Char2SegCode['-']  = SegG;
    m_Char2SegCode['_']  = SegD;
    m_Char2SegCode['\''] = SegB;
    m_Char2SegCode['"']  = SegF|SegB;
    m_Char2SegCode[0xB0] = SegF|SegA|SegB|SegG; //Deg Sign ISO/IEC 8859-1
    m_Char2SegCode['*']  = SegF|SegA|SegB|SegG; //Deg Sign replacement
    m_Char2SegCode['\\'] = SegF|SegG|SegC;
    m_Char2SegCode['/']  = SegE|SegG|SegB;
    m_Char2SegCode['^']  = SegF|SegA|SegB;
}

TM1637Display::TM1637Display(BYTE CLKPin, BYTE DIOPin, BYTE BrightnessPercent) {
    m_CLKPin = CLKPin;
    m_DIOPin = DIOPin;
    m_bShowDP = false;
    memset(m_CurrentData, 0, sizeof(m_CurrentData));
    m_bACKErr = false;

    if (wiringPiSetupGpio()>=0) {
        pinMode(m_CLKPin, OUTPUT);
        pinMode(m_DIOPin, OUTPUT);
        pullUpDnControl(m_DIOPin, PUD_UP);
    } else {
        fprintf(stderr, "\nwiringPiSetup failed, program closed!\n");
        exit(1);
    }
    Init7SegMap();
    SetBrightness(BrightnessPercent); //clearing display included
}

TM1637Display::~TM1637Display() {

}

void TM1637Display::Clear() {
    m_bACKErr = false;
    memset(m_CurrentData, 0, sizeof(m_CurrentData));
    Show(m_CurrentData);
}

void TM1637Display::Show(const char* pData) {   // show up to 4 char

    int nLen = strlen(pData);
    if (nLen>4) nLen = 4;
    start();
    writeByte(cnDataCmdAutoAddr);
    stop();
    start();
    writeByte(cnDisplayAddress);
    for (int DigitNumber=0; DigitNumber<nLen; DigitNumber++) {
        writeByte(GetSegCode(DigitNumber, pData[DigitNumber]));
    }
    for (int DigitNumber=nLen; DigitNumber<4; DigitNumber++) {
        writeByte(GetSegCode(DigitNumber, ' '));
    }
    stop();
    start();
    writeByte(cnDisplayOn + m_Brightness);
    stop();
}

void TM1637Display::Show(BYTE DigitNumber, const char Data) {   // show char at digit number (1...4)
    if (DigitNumber<1 || DigitNumber>4) {
        return;
    }
    DigitNumber--;
    start();
    writeByte(cnDataCmdFixAddr);
    stop();
    start();
    writeByte(cnDisplayAddress | DigitNumber);
    writeByte(GetSegCode(DigitNumber, Data));
    stop();
    start();
    writeByte(cnDisplayOn + m_Brightness);
    stop();
}

void TM1637Display::Show(int nData) {   //show int value
    if (nData>9999) {
        nData = 9999;
    }
    if (nData<-999) {
        nData = -999;
    }
    snprintf(m_CurrentData, sizeof(m_CurrentData), "%4d", nData);
    m_CurrentData[4] = '\0';
    Show(m_CurrentData);
}

void TM1637Display::SetBrightness(BYTE BrightnessPercent) { // BrightnessPercent 0...100%
    BYTE Brightness = BrightnessPercent/100.0*cnMaxBrightness;
    if (Brightness>cnMaxBrightness) {
        Brightness = cnMaxBrightness;
    } else if(Brightness<0) {
        Brightness = 0;
    }
    if (m_Brightness!=Brightness) {
        m_Brightness = Brightness;
        Show(m_CurrentData);  //rewrite display
    }
}

void TM1637Display::ShowDoublePoint(bool bShow) { // shows or hides the double point
    if (m_bShowDP!=bShow) {
        m_bShowDP = bShow;
        Show(2, m_CurrentData[1]); //rewrite display position 1
    }
}

void TM1637Display::writeByte(BYTE data) {
    BYTE Mask = 0x01;

    for (int nBit=0; nBit<8; nBit++) {
        digitalWrite(m_CLKPin, LOW);
        CLKWait();
        digitalWrite(m_DIOPin, (data & Mask) ? HIGH : LOW);
        Mask <<= 1;
        CLKWait();
        digitalWrite(m_CLKPin, HIGH);
        CLKWait();
    }
    pinMode(m_DIOPin, INPUT);    //switch DIO to input for ACK reading
    digitalWrite(m_DIOPin, LOW); //reset DIO before set to output
    digitalWrite(m_CLKPin, LOW); //start ACK reading
    for (int nReadTry=1; nReadTry<=100; nReadTry++) { //read/wait ACK (max. 100us)
        if (HIGH==digitalRead(m_DIOPin)) {
            CLKWait();
            if (100==nReadTry) {
                fprintf(stderr, "no ACK received\n");
                m_bACKErr = true;
            }
        } else {
            break;
        }
    }
    digitalWrite(m_CLKPin, HIGH);
    pinMode(m_DIOPin, OUTPUT);
    digitalWrite(m_CLKPin, LOW);
}

void TM1637Display::start(void) { //start signal
  digitalWrite(m_CLKPin, HIGH);
  CLKWait();
  digitalWrite(m_DIOPin, HIGH);
  //CLKWait();
  digitalWrite(m_DIOPin, LOW);
  CLKWait();
  digitalWrite(m_CLKPin, LOW);
}

void TM1637Display::stop(void) { //stop signal
  digitalWrite(m_CLKPin, LOW);
  CLKWait();
  digitalWrite(m_DIOPin, LOW);
  //CLKWait();
  digitalWrite(m_CLKPin, HIGH);
  CLKWait();
  digitalWrite(m_DIOPin, HIGH);
}

BYTE TM1637Display::GetSegCode(BYTE DigitNumber, const char Data) { //char to binary code for display
    m_CurrentData[DigitNumber] = toupper(Data);
    BYTE SegCode = m_Char2SegCode[m_CurrentData[DigitNumber]];
    if (1==DigitNumber && m_bShowDP) { //DP ony supported at digit number 1 (center)
        SegCode |= TM1637::SegDP;
    } else {
        SegCode &= ~TM1637::SegDP;
    }
    return SegCode;
}

void TM1637Display::CLKWait() {
  //delayMicroseconds(1);
  usleep(10);
}

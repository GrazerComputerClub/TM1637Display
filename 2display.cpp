// 2display program for 4-Digit display with TM1637 chip
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// Written by m.stroh
// Compile: g++ -Wall -o 2display 2display.cpp TM1637Display.cpp -lwiringPi

#include "TM1637Display.h"
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[]) {

  TM1637Display display(23, 24, 60);  // CLK to GPIO 23 and DIO to GPIO 24, 60% brightness

  //printf("2tm1637 - set text to 4-Digit display with TM1637 chip (written by m.stroh)\n\n");
  if (argc>1) { 
   display.Show(argv[1]);
  } else {
    char buf[5]={"\0"}; 
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    int n = read(0, buf, 4);
    if (n>0) {
      display.Show(buf);
    } else {
   	display.Clear();
    }
  }
}

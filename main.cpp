// Test program for 4-Digit display with TM1637 chip
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#include "TM1637Display.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void UserInput(){
  printf("press any key to start");
  getchar();
}

int main(int argc, char *argv[]) {

  TM1637Display display(23, 24, 60);  // CLK to GPIO 23 and DIO to GPIO 24, 60% brightness

  printf("Test program for 4-Digit display with TM1637 chip (written by m.stroh)\n\n");

  printf("Testing int values from 0 to 9999...\n");
  UserInput();
  for (int nValue=0; nValue<=9999; nValue+=11) {
    display.Show(nValue);
  }
  printf("Testing int values from -999 to 1199...\n");
  UserInput();
  for (int nValue=-999; nValue<=1199; nValue++) {
    display.Show(nValue);
  }

  printf("\nTesting set single char and double point...\n");
  UserInput();
  display.Show(1, '2');
  display.Show(2, '3');
  display.ShowDoublePoint(true);
  display.Show(3, '5');
  display.Show(4, '4');
  sleep(1);
  display.ShowDoublePoint(false);
  sleep(1);
  display.ShowDoublePoint(true);
  sleep(1);
  display.ShowDoublePoint(false);

  printf("\nTesting text...\n");
  UserInput();
  display.Show("HELO");
  sleep(1);
  display.Show("HERE");
  sleep(1);
  display.Show("IS");
  sleep(1);
  display.Show("YOUR");
  sleep(1);
  display.Show("PI0 ");

  printf("\nTesting special signs ...\n");
  UserInput();
  display.Show("\\/'\"");
  sleep(2);
  display.Show("_^\xB0*"); // \xB0 Deg Sign
  sleep(2);
  display.Show("TCPU");
  sleep(2);
  display.Show("54*C");


  printf("\nTesting clear...\n");
  UserInput();
  display.Clear();

  printf("\nTesting lower brightness...\n");
  UserInput();
  display.Show("100P");
  display.SetBrightness(100);
  sleep(1);
  display.Show(" 80P");
  display.SetBrightness(80);
  sleep(1);
  display.Show(" 40P");
  display.SetBrightness(40);
  sleep(1);
  display.Show(" 20P");
  display.SetBrightness(20);
  sleep(1);
  display.Show(" 10P");
  display.SetBrightness(10);
  sleep(1);

  printf("\nfinished\n");
  display.Clear();
}

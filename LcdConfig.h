#ifndef LCDCONFIG_H
#define LCDCONFIG_H

bool InitLcd();
void LcdPrint(String msg, int rowId);
void LcdPrint(String msg);
void LcdPrint(String msg1, String msg2);

void LcdPrint(String msg, bool autoScroll);
#endif

#include "ScrollingText.h"

ScrollingText::ScrollingText(MI0283QT9 lcd) {
  this->lcd = lcd;
  this->lastUpdate = millis();
}

void ScrollingText::setText(String text) {
  this->text = text;
}

void ScrollingText::setPosition(unsigned short x, unsigned short y) {
  this->x = x;
  this->y = y;
}

void ScrollingText::setColor(int color) {
  this->color = color;
}

void ScrollingText::setBackgroundColor(int color) {
  this->backgroundColor = color;
}

void ScrollingText::update() {
  unsigned long now = millis();
  if (now - this->lastUpdate >= this->interval) {
    this->startIndex = (this->startIndex + 1) % this->text.length();
    String displayedText = this->text.substring(this->startIndex, this->startIndex + this->displayChars);
    while (displayedText.length() < this->displayChars) {
      displayedText += " ";
    }
    lcd.drawText(this->x, this->y, displayedText, this->color, this->backgroundColor, 2);
    this->lastUpdate = now;
  }
}



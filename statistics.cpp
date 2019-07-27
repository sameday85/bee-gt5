#include "QFile"
#include "QDebug"
#include "QTextStream"
#include "statistics.h"

Statistics::Statistics()
{
    reset();
}

Statistics::Statistics(qlonglong time_, int asked_, int answered_) {
    time = time_;
    asked = asked_;
    answered = answered_;
}

void Statistics::reset() {
    time = 0;
    asked = answered = 0;
}

int Statistics::getAsked() {
    return asked;
}

void Statistics::setAsked(int value) {
    asked = value;
}

void Statistics::incAsked() {
    ++asked;
}

int Statistics::getAnswered() {
    return answered;
}

void Statistics::setAnswered(int value) {
    answered = value;
}

void Statistics::incAnswered() {
    ++answered;
}

qlonglong Statistics::getTime() {
    return time;
}

void Statistics::setTime(qlonglong tm) {
    time = tm;
}

float Statistics::getCorrectPercentage() {
    if (asked <= 0)
        return 0;
    return 100.0f * answered / asked;
}

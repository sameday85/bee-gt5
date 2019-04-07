#!/bin/bash
say() { local IFS=+;/usr/bin/mpg321 -q "http://translate.google.com/translate_tts?ie=UTF-8&client=tw-ob&q=$*&tl=en"; }
say $*

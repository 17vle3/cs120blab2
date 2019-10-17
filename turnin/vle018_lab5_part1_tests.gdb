# Test file for Lab2_introToAVR


echo ======================================================\n
echo Running all tests..."\n\n


# Add tests below

test "0"
setPINA 0x0F
continue 18
expectPORTC 0b01000000
checkResult

test "1"
setPINA 0x0E
continue 18
expectPORTC 0b01100000
checkResult

test "2"
setPINA 0x0D
continue 18
expectPORTC 0b01100000
checkResult

test "3"
setPINA 0x0C
continue 18
expectPORTC 0b01110000
checkResult

test "4"
setPINA 0x0B
continue 18
expectPORTC 0b01110000
checkResult

test "5"
setPINA 0x0A
continue 18
expectPORTC 0b00111000
checkResult

test "6"
setPINA 0x09
continue 18
expectPORTC 0b00111000
checkResult

test "7"
setPINA 0x08
continue 18
expectPORTC 0b00111100
checkResult

test "8"
setPINA 0x07
continue 18
expectPORTC 0b00111100
checkResult

test "9"
setPINA 0x06
continue 18
expectPORTC 0b00111100
checkResult

test "10"
setPINA 0x05
continue 18
expectPORTC 0b00111110
checkResult

test "11"
setPINA 0x04
continue 18
expectPORTC 0b00111110
checkResult

test "12"
setPINA 0x03
continue 18
expectPORTC 0b00111110
checkResult

test "13"
setPINA 0x02
continue 18
expectPORTC 0b00111111
checkResult

test "14"
setPINA 0x01
continue 18
expectPORTC 0b00111111
checkResult

test "15"
setPINA 0x00
continue 18
expectPORTC 0b00111111
checkResult


# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

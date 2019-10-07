# Test file for Lab2_introToAVR


# commands.gdb provides the following functions for ease:
#   test "<message>"
#       Where <message> is the message to print. Must call this at the beginning of every test
#       Example: test "PINA: 0x00 => expect PORTC: 0x01"
#   checkResult
#       Verify if the test passed or failed. Prints "passed." or "failed." accordingly, 
#       Must call this at the end of every test.
#   expectPORTx <val>
#       With x as the port (A,B,C,D)
#       The value the port is epected to have. If not it will print the erroneous actual value
#   setPINx <val>
#       With x as the port or pin (A,B,C,D)
#       The value to set the pin to (can be decimal or hexidecimal
#       Example: setPINA 0x01
#   printPORTx f OR printPINx f 
#       With x as the port or pin (A,B,C,D)
#       With f as a format option which can be: [d] decimal, [x] hexadecmial (default), [t] binary 
#       Example: printPORTC d
#   printDDRx
#       With x as the DDR (A,B,C,D)
#       Example: printDDRB

echo ======================================================\n
echo Running all tests..."\n\n


# Add tests below

test "1"
setPINA 0x01
continue 18
expectPORTC 0b01100000
checkResult

test "2"
setPINA 0x02
continue 18
expectPORTC 0b01100000
checkResult

test "3"
setPINA 0x03
continue 18
expectPORTC 0b01110000
checkResult

test "4"
setPINA 0x04
continue 18
expectPORTC 0b01110000
checkResult

test "5"
setPINA 0x05
continue 18
expectPORTC 0b00111000
checkResult

test "6"
setPINA 0x06
continue 18
expectPORTC 0b00111000
checkResult

test "7"
setPINA 0x07
continue 18
expectPORTC 0b00111100
checkResult

test "8"
setPINA 0x08
continue 18
expectPORTC 0b00111100
checkResult

test "9"
setPINA 0x09
continue 18
expectPORTC 0b00111100
checkResult

test "10"
setPINA 0x0A
continue 18
expectPORTC 0b00111110
checkResult

test "11"
setPINA 0x0B
continue 18
expectPORTC 0b00111110
checkResult

test "12"
setPINA 0x0C
continue 18
expectPORTC 0b00111110
checkResult

test "13"
setPINA 0x0D
continue 18
expectPORTC 0b00111111
checkResult

test "14"
setPINA 0x0D
continue 18
expectPORTC 0b00111111
checkResult

test "15"
setPINA 0x0D
continue 18
expectPORTC 0b00111111
checkResult


# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

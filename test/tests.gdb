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

test "input is 1"
setPINA 0x01
continue 18
expectPORTC 0b1100000
checkResult

test "input is 2"
setPINA 0x01
continue 18
expectPORTC 0b1100000
checkResult

test "input is 3"
setPINA 0x03
continue 18
expectPORTC 0b1110000
checkResult

test "input is 4"
setPINA 0x03
continue 18
expectPORTC 0b1110000
checkResult

test "input is 5"
setPINA 0x05
continue 18
expectPORTC 0b0111000
checkResult

test "input is 6"
setPINA 0x05
continue 18
expectPORTC 0b0111000
checkResult

test "input is 7"
setPINA 0x07
continue 18
expectPORTC 0b0111100
checkResult

test "input is 8"
setPINA 0x08
continue 18
expectPORTC 0b0111100
checkResult

test "input is 9"
setPINA 0x09
continue 18
expectPORTC 0b0111100
checkResult

test "input is 10"
setPINA 0x0A
continue 18
expectPORTC 0b0111110
checkResult

test "input is 11"
setPINA 0x0B
continue 18
expectPORTC 0b0111110
checkResult

test "input is 12"
setPINA 0x0C
continue 18
expectPORTC 0b0111110
checkResult

test "input is 13"
setPINA 0x0D
continue 18
expectPORTC 0b0111111
checkResult

test "input is 14"
setPINA 0x0E
continue 18
expectPORTC 0b0111111
checkResult

test "input is 15"
setPINA 0x0F
continue 18
expectPORTC 0b0111111
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

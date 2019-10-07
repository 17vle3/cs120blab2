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

test "1+1"
setPINA 0x01
setPINB 0x01
continue 18
expectPORTC 0x02
checkResult

test "1111+1111
setPINA 0b1111
setPINB 0b1111
continue 18
expectPORTC 0x08
checkResult

test "1001+1111
setPINA 0b1001
setPINB 0b1111
continue 18
expectPORTC 0x06
checkResult

test "1111 1111 +1111 1111"
setPINA 0b11111111
setPINB 0b11111111
continue 18
expectPORTC 0x10
checkResult

test "1111 1101 +1101 1111"
setPINA 0b11111101
setPINB 0b11011111
continue 18
expectPORTC 0x0E
checkResult


# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

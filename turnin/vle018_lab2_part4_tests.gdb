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

test "total weight at 140"
setPINA 0x00
setPINB 0x8C
setPINC 0x00
continue 18
expectPORTD 0x00
checkResult

test "total weight at 141 (overweight)"
setPINA 0x00
setPINB 0x8D
setPINC 0x00
continue 18
expectPORTD 0x01
checkResult

test "rlly overweight"
setPINA 0x00
setPINB 0x8D
setPINC 0x00
continue 18
expectPortD 0x01
checkResult

test "overweight and diff"
setPINA 0x00
setPINB 0x0D
setPINC 0x80
continue 18
expectPORTD 0x03
checkResult

test "diff"
setPINA 0x80
setPINB 0x00
setPINC 0x00
continue 18
expectPORTD 0x02
checkResult

test "all inputs 0"
setPINA 0x00
setPINB 0x00
setPINC 0x00
continue 18
expectPORTD 0x00
checkResult


# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

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
# upper into lower b
# lower into upper c

test "nothing pressed"
set state = start
setPINA 0x0F
continue 18
expectPORTC 0x00
expect state next
checkResult

test "a0 pressed"
set state = start
setPINA 0x0E
continue 18
expectPORTC 0x01
expect state release
checkResult

test "a0 press then release"
set state = start
setPINA 0x0E
continue 30
setPINA 0x0F
continue 30
expectPORTC 0x01
expect state next
checkResult

test "a1 pressed"
set state = start
setPINA 0x0D
continue 18
expectPORTC 0x00
expect state release
checkResult

test "a1 press then release"
set state = start
setPINA 0x0D
continue 18
setPINA 0x0F
continue 30
expectPORTC 0x00
expect state next
checkResult



# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

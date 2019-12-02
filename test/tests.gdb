# Test file for Lab2_introToAVR

echo ======================================================\n
echo Running all tests..."\n\n


# Add tests below
# upper into lower b
# lower into upper c

test "nothing pressed"
set state = joystick_start
setPINA 0x0F
continue 18
expectPORTB 0x04
expect state joystick_play
checkResult

test "right pressed"
set state = joystick_start
setPINA 0x0F
continue 18
expectPORTB 0x04
expect state joystick_play
set LRTemp = 900
continue 18
expectPORTB 0x08
expect state joystick_wait
checkResult

test "left pressed"
set state = joystick_start
setPINA 0x0F
continue 18
expectPORTB 0x04
expect state joystick_play
set LRTemp = 300
continue 18
expectPORTB 0x02
expect state joystick_wait
checkResult

test "rreset"
set state = joystick_start
setPINA 0x0F
continue 18
expectPORTB 0x04
expect state joystick_play
set LRTemp = 300
continue 18
expectPORTB 0x02
expect state joystick_wait
set LRTemp = 560
continue 18
expectPORTB 0x02
expect state joystick_play
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

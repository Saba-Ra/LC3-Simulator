; This symbol is used for comments ;
; A program to calculate addition of numbers from 1 to N
ORG 0x3000
; Initialize variables
LD R0, INIT ; Load R0 with the initial value (1)
LD R2, N ; Load R2 with loop size 
AND R1, R1, #0 ; Clear R1 (accumulator for sum)
; Loop to add numbers from 1 to 100
LOOP, ADD R1, R1, R0 ; Add current number (R0) to sum (R1)
ADD R0, R0, #1 ; Increment current number
ADD R2, R2, #1 ; Increment loop counter
BRn LOOP ; If counter negative (not reached 0), repeat loop
; Store the result (sum of numbers 1 to 100) in memory
ST R1, RESULT ; Store the sum in memory location 'RESULT' 
; Halt the program
HALT
; Initialize variables
INIT, DEC 1 ; Initial value for the loop (starting from 1)
RESULT, DEC 0 ; Memory location to store the re
N, DEC -3
END
DE SEGMENT CODE
PUBLIC _bcd_to_led
RSEG DE

_bcd_to_led:	MOV A, R7
				MOV DPTR, #TAB
				MOVC A, @A+DPTR
				MOV R7,A
				RET

TAB:DB 3fH,06H,5bH,4FH,66H,6dH,7dH,07H,7fH,6fH		
		
		END
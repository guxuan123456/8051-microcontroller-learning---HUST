DE SEGMENT CODE
PUBLIC _bcd_to_led
RSEG DE

_bcd_to_led:	MOV B, #02H
				MOV A, R7
				DIV AB
				MOV R7, B  

				RET	
		
				END
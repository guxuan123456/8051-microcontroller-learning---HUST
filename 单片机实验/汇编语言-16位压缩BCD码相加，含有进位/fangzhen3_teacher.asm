;File name:    fangzhen3_teacher.asm
;Description:  1字节BCD码相加
;Date:  	   2020.3.24
;Designed by: 	xuan gu
;Source used:    
;   31H+32H+33H+34H BCD data ->35H+36H(高位) BCD data 
;



	        ORG  0000H		
			LJMP MAIN
			ORG  0100H


MAIN:	   	MOV  SP,  #40H           ;赋堆栈指针
            CLR  C               	 ;进位位清零

		    MOV  31H,  #349H          ;31H单元赋值
		    MOV  32H,  #87H          ;32H单元赋值
            MOV  33H,  #69H          ;33H单元赋值
		    MOV  34H,  #82H          ;34H单元赋值
            MOV  35H,  #00H          ;35H单元赋值
		    MOV  36H,  #00H          ;36H单元赋值

		    MOV  A,    31H            ;32H+31H 
		    ADD  A,    32H
		    DA A                       ; BCD调整
		    MOV  35H,    A
		    MOV   A,36H
		    ADDC A,#00H              ;将进位存在高位36H
            MOV  36H,    A 

		    MOV  A,    35H            ;32H+31H+33H 
		    ADD  A,    33H
		    DA A                       ; BCD调整
		    MOV  35H,    A
		    MOV   A,36H
		    ADDC A,#00H              ;将进位存在高位36H
            MOV  36H,    A 
            MOV  A,    35H            ;32H+31H+33H+34H 
		    ADD  A,    34H
		    DA A                       ; BCD调整
		    MOV  35H,    A
		    MOV   A,36H
		    ADDC A,#00H              ;将进位存在高位36H
            MOV 36H,    A 
		 
HERE:	    SJMP  HERE                ;踏步
		    END 

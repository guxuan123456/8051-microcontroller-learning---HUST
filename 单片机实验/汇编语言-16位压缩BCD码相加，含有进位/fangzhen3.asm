;File name:    fangzhen3.asm
;Description:  双字节BCD码相加
;Date:  	   2020.3.24
;Designed by: 	xuan gu
/* 思路：先算低八位三个数相加，结果存到低八位31H中，进位存入30H中
		接着计算高八位三个数相加，再加上30H中的进位信息，得到高八位存入30H，进位存入32H*/

									  

	        ORG  0000H		
			LJMP MAIN
			ORG  0100H


MAIN:	   	MOV  SP,  #40H           ;赋堆栈指针
            CLR  C               	 ;进位位清零

		    MOV  20H,  #49H          ;20H单元赋值
		    MOV  21H,  #87H          ;21H单元赋值
            MOV  22H,  #69H          ;22H单元赋值
		    MOV  23H,  #82H          ;23H单元赋值
            MOV  24H,  #13H          ;24H单元赋值
		    MOV  25H,  #14H          ;25H单元赋值

		    MOV  A,    20H            ;20H+22H 
		    ADD  A,    22H
		    DA A                      ; BCD调整
		    MOV  31H,    A			  ;前两个低位结果存入31H
		    MOV   A,32H
		    ADDC  A,#00H              ;将进位存在32H
            MOV  32H,    A 

		    MOV  A,    31H            ;22H+20H+24H 
		    ADD  A,    24H
		    DA A                       ; BCD调整
		    MOV  31H,    A			  ;低位结果存入31H
		    MOV   A,32H
		    ADDC  A,#00H              ;将进位存在高位32H
            MOV  32H,    A 

			//开始加高八位
			CLR  C 
			MOV  A,    21H            ;21H+低八位进位
		    ADD  A,    32H
		    DA A                      ; BCD调整
		    MOV  30H,    A			  ;高位结果存入30H
		    MOV   A, #00H
		    ADDC  A,#00H              ;将进位存在32H
            MOV  32H,    A 



            MOV  A,    30H            ;21H+23H 
		    ADD  A,    23H
		    DA A                       ; BCD调整
		    MOV  30H,    A
		    MOV   A,32H
		    ADDC A,#00H              ;将进位存在高位32H
            MOV 32H,    A 
		 
		    
            MOV  A,    30H            ;21H+23H+25H 
		    ADD  A,    25H
		    DA A                       ; BCD调整
		    MOV  30H,    A
		    MOV   A,32H
		    ADDC A,#00H              ;将进位存在高位32H
            MOV 32H,    A 

HERE:	    SJMP  HERE                ;踏步
		    END 

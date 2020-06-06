 /* 这里选择不会出现进位的数据且在循环加入数据时不考虑借位情况*/

	  ORG 0000H
	  LJMP MAIN
	  ORG 0100H

MAIN: MOV A,#20H	;初始值低位
	  MOV DPTR,#4050H
	  MOV R2,#10H

	  ACALL INIT
	  ACALL AVERAGE
	  ACALL SORT
	  LJMP DONE

INIT: MOVX @DPTR,A
	  INC DPTR
	  DEC A	
	  MOV B,A
	  MOV A,#01H	   ;高位设置01H
	  MOVX @DPTR,A
	  MOV A,B
	  INC DPTR
	  DJNZ R2,INIT
	  MOV DPTR,#4050H
	  MOV R2,#10H
	  RET

AVERAGE: MOVX A,@DPTR
		 MOV B,A
		 MOV A,20H

		 ADD A,B
		 MOV 20H,A
		 MOV A,21H
		 ADDC A,#00H
		 MOV 21H,A
		 INC DPTR
		 INC DPTR
		 DJNZ R2,AVERAGE
		 MOV DPTR,#4051H
		 MOV R2,#10H
		 SJMP SUMHIGH

//求得所有的和，和存在20H,21H处，进位信息存在22H处。
SUMHIGH: MOVX A,@DPTR
		 MOV B,A
		 MOV A,21H

		 ADD A,B
		 MOV 21H,A
		 MOV A,22H
		 ADDC A,#00H
		 MOV 22H,A
		 INC DPTR
		 INC DPTR
		 DJNZ R2,SUMHIGH

		 MOV R5,#10H
      	 MOV R6,21H
	     MOV R7,20H
	     SJMP DIV_H2
		 RET
DIV_H2:			;除数高8位为00H，被除数高8为非00H，则余数肯定是单字节
	MOV      A,R5
	MOV      R0,A		;R0暂存除数
	MOV      B,A	
	MOV      A,R6					
	DIV      AB			;被除数高8位除以余数
	JB       OV,UIDIV_END;检查到除数低8位也为00H，直接结束程序
	MOV      R6,A					
	MOV      R5,B	;商存在R6，余数存在R5
	MOV      B,#08H	;移位相减共08H次

UIDIV_LOOP2:		;低8位运算
	MOV      A,R7
	RLC     A	 //看看是否进位
	MOV R7,A
    JC   NEXT	//进位说明余数后面加1,此时玉树左移并加上1
    SJMP JIA0	//不进位余数后面加0

JIA0: MOV A,R5
      RLC A	  	 //得到余数，要判断余数和除数的大小
	  MOV R5,A	 //余数存到R5
	  MOV 10H,R0		   //用10H临时储存除数的那个值。
	  CJNE A,10H,CHANGE		//有进位跳转,有进位余数小 
	  SJMP NEXT2

NEXT: MOV A,R5
      ADD A,R5
	  ADD A,#01H   //得到余数，要判断余数和除数的大小
	  MOV R5,A	   ///余数存到R5
	  MOV 10H,R0
	  CJNE A,10H,CHANGE
	  SJMP NEXT2	
CHANGE:	JNC NEXT2
        SJMP NEXT1
NEXT1:
//余数小，不够减
	MOV A,R1
	ADD A,R1
	MOV R1,A
	DJNZ  B,UIDIV_LOOP2	//不够除，在加于数
	SJMP DONE1
NEXT2: //余数够减
    SUBB A,R0	
    MOV R5,A
    MOV A,R1
	ADD A,R1	
	ADD A,#01H
	MOV R1,A
	DJNZ   B,UIDIV_LOOP2	//够除，在加于数
	SJMP DONE1
UIDIV_END:	
	SJMP DONE1

DONE1:	 //结果存入外存储区
	MOV 22H,R1
	MOV 23H,R6
    MOV 24H,R5
	MOV A,22H
	MOV DPTR,#4080H
	MOVX @DPTR,A
	INC DPTR
	MOV A,23H
	MOVX @DPTR,A
	INC DPTR
 	MOV A,24H
	MOVX @DPTR,A
	RET




SORT: MOV DPTR,#4050H
	  MOV R0,#30H
	  MOV R2,#20H
	  SJMP MOVE

MOVE: MOVX A,@DPTR
      MOV @R0,A
	  INC R0
	  INC DPTR
	  DJNZ R2,MOVE
	  MOV R0,#4EH
	  MOV R2,#10H
	  MOV DPTR,#4050H
	  SJMP PAIXU

PAIXU: MOV A,@R0
	   MOVX @DPTR,A
	   INC DPTR
	   INC R0
	   MOV A,@R0
	   MOVX @DPTR,A
	   INC DPTR
	   DEC R0
	   DEC R0
	   DEC R0
	   DJNZ R2,PAIXU
	   RET
	  
DONE: SJMP DONE
END
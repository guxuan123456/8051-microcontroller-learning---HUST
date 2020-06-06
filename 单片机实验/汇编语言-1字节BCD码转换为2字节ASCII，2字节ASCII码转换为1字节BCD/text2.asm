		/*
		File name: e1.asm
		
		Description: 1字节BCD码转换为2字节ASCII
		
		2字节ASCII码转化为1字节BCD码
		
		Date:
		
		Designed by:
		
		Source used: 30H: BCD data
		
		31H, 32H：ASCII data
		
		31H, 32H ->33H?BCD data
		*/
		
		ORG 0000H
		
		LJMP MAIN
		
		ORG 0100H
		
		MAIN:MOV SP, #40H ;赋堆栈指针
		
		MOV 30H,#49H;30H单元赋值
		
		MOV R0, #32H ;R0指针赋值
		
		MOV @R0, #0 ;32H单元清零
		
		MOV A, 30H
		
		XCHD A, @R0 ;拆分字节
		
		ORL 32H, #30H ;转换为ASCII
		
		SWAP A
		
		ORL A, #30H ;转换为ASCII
		
		MOV 31H, A ;结果存于31H单元
		
		MOV A, 31H;从31H单元取值
		
		ANL A, #0FH;转换为BCD
		
		SWAP A
		
		MOV 33H, A;结果存于33H单元
		
		MOV A, 32H;从32H单元取值
		
		ANL A, #0FH;转换为BCD
		
		ORL 33H, A;合并字节
		
		HERE:SJMP HERE ;踏步
		
		END

		
		/*
		File name: text1.asm
		
		Date: 2020.15.03
		
		Designed by: xuan Gu
		
		Source used: R0: jishuqi
		
		DPTR:;片外数据指针
		*/

		
		ORG 0000H
		
		LJMP MAIN
		
		ORG	0100H
		
		MAIN: MOV SP,#40H
		
		FILL: CLR A ; A寄存器清零
		
		MOV R0, #00H;设循环计数器
		
		MOV DPTR, #7000H;设数据指针
		
		FILL1:	MOVX @DPTR, A;传送到片外RAM
		
		INC A;A内容加1
		
		INC DPTR;修改数据指针
		
		INC R0;修改循环计数器
		
		CJNE R0, #00H, FILL1
		
		HERE:	SJMP HERE;原地踏步
		
		END

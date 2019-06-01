# OSExperiment
操作系统模拟进程调度实验
基于VS2015，使用C语言编写



程序运行之后在DOS界面进行命令的输入，每一个命令对应一种操作；总共有七条命令：
-init 								//系统需要先运行init命令进行初始化
-cr <name> <priority>(=1 or 2) 		// create process  
-de <name> 							// delete process 
-req <resource name> <# of units>	// request resource 
-rel <resource name> <# of units>	// release resource 
-to									// time out 
 
查看进程状态和资源状态的命令（list命令）
-list ready	//list all processes in the ready queue 
-list block // list all processes in the block queue 
-list res 	//list all available resources 
#if 1
#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct tcb
{
	char name;//名字
	int status;//当前状态
	int prio;//优先级

	struct tcb *father = NULL;//父结点TCB
	int son_num = 0;//子结点个数
	struct tcb *son[16];//子结点TCB
}TCB;

typedef struct
{
	//char name[8];//资源名字
	int can_ues_resource_num = 5;//可用资源数

	int owner_num = 0;//使用者个数
	TCB owner_TCB[8];//使用者名字数组
	int owner_used_num[8];//使用者使用资源个数
	
	int waitlist_num = 0;//等待队列的个数
	int wait_resource_num[8];//等待资源数目
	TCB waitlist[8];//等待队列
}resource;


#define program_runing	0
#define program_ready	1
#define program_blocked	2

bool runing = false;//系统运行状态

TCB program_runing_now;//当前运行的进程的TCB
TCB program_readylist0;//prio0TCB
TCB program_readylist1[128];//prio1TCB数组
TCB program_readylist2[128];//prio2TCB数组

int program_readylist1_num = 0;//prio1TCB数组使用情况
int program_readylist2_num = 0;//prio2TCB数组使用情况


int main()
{
	int i, j;
	char command[128];
	TCB *p;
	//定义两个资源，采用默认初始值
	resource r1, r2;
	while (1)
	{
		printf("shell>");
		scanf("%s", &command);//获取命令

		//-init命令
		if (strcmp(command, "init") == 0)
		{
			if (runing == true)
			{
				printf("系统已初始化过，无需再进行初始化！\n");
				continue;
			}
			runing = true;
			
			//创建init进程并加入到program_readylist0中
			//将当前运行的进程置为init进程
			program_readylist0.name = ' ';
			program_readylist0.prio = 0;
			program_readylist0.status = program_runing;
			
			//父子节点信息采用默认值
			program_readylist0.father = NULL;
			program_readylist0.son_num = 0;

			//初始化r1和r2资源
			r1.can_ues_resource_num = 5;
			r1.owner_num = 0;
			r1.waitlist_num = 0;
			r2.can_ues_resource_num = 5;
			r2.owner_num = 0;
			r2.waitlist_num = 0;

			program_runing_now = program_readylist0;

			printf("系统初始化成功，init进程正在运行！\n");
		}


		//-cr命令
		else if (strcmp(command, "cr") == 0)
		{
			if (runing != true)
			{
				printf("系统还未进行初始化，请先进行初始化！\n");
				continue;
			}

			//动态申请一个TCB结构体
			p = (TCB*)malloc(sizeof(TCB));
			p->status = program_ready;
			scanf("%s", &command);//获取cr命令第一个参数（名字）
			p->name = command[0];
			scanf("%s", &command); //获取cr命令第二个参数（优先级）
			if (command[0] == '1')
				p->prio = 1;
			else if(command[0] == '2')
				p->prio = 2;
			else//出错判断
			{
				printf("输入的优先级只能为1或2，不能为其他值！\n");
				free(p);
				continue;
			}

			//新建进程的父结点为当前进程
			p->father = &program_runing_now;
			p->son_num = 0;//此时新进程还没有孩子结点
			//将新建进程加入到当期进程的孩子结点
			program_runing_now.son[program_runing_now.son_num] = p;
			program_runing_now.son_num++;
			
			//当前创建的进程优先级更高时，切换进程运行
			if (p->prio > program_runing_now.prio)
			{
				//init进程一开始就存在program_readylist0里面，直接切换新进程就行
				if (program_runing_now.prio == 0)
				{
					//init进程变为ready状态
					program_runing_now.status = program_ready;
					program_runing_now = *p;//切换到新进程执行
					program_runing_now.status = program_runing;//新进程变为执行状态
				}
				else if (program_runing_now.prio == 1)
				{
					//将老的进程压入对应的ready数组，并切换新进程
					program_runing_now.status = program_ready;
					program_readylist1[program_readylist1_num] = program_runing_now;
					program_readylist1_num++;
					program_runing_now = *p;
					program_runing_now.status = program_runing;
				}
				
			}
			else//创建的进程优先级无更高时，存入对应的TCB数组里面
			{
				if (p->prio == 1)
				{
					program_readylist1[program_readylist1_num] = *p;
					program_readylist1_num++;
				}
				else
				{
					program_readylist2[program_readylist2_num] = *p;
					program_readylist2_num++;
				}
			}
			printf("进程%c正在运行！\n", program_runing_now.name);
		}


		//-de命令
		else if (strcmp(command, "de") == 0)
		{
			if (runing != true)
			{
				printf("系统还未进行初始化，请先进行初始化！\n");
				continue;
			}
			
			scanf("%s", &command);//获取要删除的进程名字
			
			if (program_runing_now.name == command[0])//删除的进程是当前进程
			{
				//将当前进程孩子结点的父结点转变为当前进程的父结点
				//（孙子变成孩子）
				p = program_runing_now.father;//p指向当前进程的父结点
				for (i = 0; i < program_runing_now.son_num; i++)
				{ 
					p->son[p->son_num] = program_runing_now.son[i];
					p->son_num++;
				}
				
				//判定有无占据r1资源（只需到拥有者队列去找）
				for (i = 0; i < r1.owner_num; i++)
				{
					//如果有占据资源，则需释放
					if (program_runing_now.name == r1.owner_TCB[i].name)
					{
						r1.can_ues_resource_num += r1.owner_used_num[i];
						printf("当前进程%c释放了%d个r1资源!\n", program_runing_now.name, r1.owner_used_num[i]);
						//后面的拥有者往前移动
						for (j = i; j < r1.owner_num - 1; j++)
						{
							r1.owner_TCB[j] = r1.owner_TCB[j + 1];
							r1.owner_used_num[j] = r1.owner_used_num[j + 1];
						}
						r1.owner_num--;
						break;
					}
				}

				//判定有无占据r2资源（只需到拥有者队列去找）
				for (i = 0; i < r2.owner_num; i++)
				{
					//如果有占据资源，则需释放
					if (program_runing_now.name == r2.owner_TCB[i].name)
					{
						r2.can_ues_resource_num += r2.owner_used_num[i];
						printf("当前进程%c释放了%d个r2资源!\n", program_runing_now.name, r2.owner_used_num[i]);
						//后面的拥有者往前移动
						for (j = i; j < r2.owner_num - 1; j++)
						{
							r2.owner_TCB[j] = r2.owner_TCB[j + 1];
							r2.owner_used_num[j] = r2.owner_used_num[j + 1];
						}
						r2.owner_num--;
						break;
					}
				}


				//重新进行系统调度
				//有优先级为2的进程在ready队列中，切换到该进程执行
				if (program_readylist2_num != 0)
				{
					//弹出最早进程
					program_runing_now = program_readylist2[0];
					program_runing_now.status = program_runing;
					//后面进程往前移动
					for (i = 0; i < program_readylist2_num - 1; i++)
					{
						program_readylist2[i] = program_readylist2[i + 1];
					}
					//数量-1
					program_readylist2_num--;
					printf("删除进程成功，当前系统进程%c正在运行！\n", program_runing_now.name);
				}

				//有优先级为1的进程在ready队列中，切换到该进程执行
				else if (program_readylist1_num != 0)
				{
					//弹出最早进程
					program_runing_now = program_readylist1[0];
					program_runing_now.status = program_runing;
					//后面进程往前移动
					for (i = 0; i < program_readylist1_num - 1; i++)
					{
						program_readylist1[i] = program_readylist1[i + 1];
					}
					//数量-1
					program_readylist1_num--;
					printf("删除进程成功，当前系统进程%c正在运行！\n", program_runing_now.name);
				}
				//优先级1、2无进程了，切换到init进程执行
				else
				{
					program_runing_now = program_readylist0;
					printf("删除进程成功，当前系统进程init正在运行！\n");
				}
				
			}
			//到ready数组中查找有无此进程名字
			else//删除的不是当前进程
			{
				j = 0;
				//先查看此进程有无获取r1或r2的资源，如果有，则直接删除
				//先在堵塞队列里找
				for (i = 0; i < r1.waitlist_num; i++)
				{
					//r1的堵塞队列里有，直接覆盖掉即可
					if (r1.waitlist[i].name == command[0])
					{
						//将当前进程孩子结点的父结点转变为当前进程的父结点
						//（孙子变成孩子）
						p = r1.waitlist[i].father;
						for (j = 0; j < r1.waitlist[i].son_num; j++)
						{
							p->son[p->son_num] = r1.waitlist[i].son[j];
							p->son_num++;
						}
						//后面等待队列覆盖
						for (j = i; j < r1.waitlist_num - 1; j++)
						{
							r1.waitlist[j] = r1.waitlist[j + 1];
							r1.wait_resource_num[j] = r1.wait_resource_num[j + 1];
						}
						r1.waitlist_num--;
						printf("进程%c已从等待r1资源队列中移除！\n", command[0]);
						j = 1;
						break;
					}
				}
				for (i = 0; i < r2.waitlist_num; i++)
				{
					//r1的堵塞队列里有，直接覆盖掉即可
					if (r2.waitlist[i].name == command[0])
					{
						//将当前进程孩子结点的父结点转变为当前进程的父结点
						//（孙子变成孩子）
						p = r1.waitlist[i].father;
						for (j = 0; j < r1.waitlist[i].son_num; j++)
						{
							p->son[p->son_num] = r1.waitlist[i].son[j];
							p->son_num++;
						}
						
						//后面等待队列覆盖
						for (j = i; j < r2.waitlist_num - 1; j++)
						{
							r2.waitlist[j] = r2.waitlist[j + 1];
							r2.wait_resource_num[j] = r2.wait_resource_num[j + 1];
						}
						r2.waitlist_num--;
						printf("进程%c已从等待r2资源队列中移除！\n", command[0]);
						j = 1;
						break;
					}
				}
				if (j != 0)
					continue;



				//然后在拥有者队列查找
				for (i = 0; i < r1.owner_num; i++)
				{
					//有获取r1的资源
					if (r1.owner_TCB[i].name == command[0])
					{
						r1.can_ues_resource_num += r1.owner_used_num[i];//释放资源
						printf("进程%c释放了%d个r1资源!\n", command[0], r1.owner_used_num[i]);
						//后面拥有者往前移动
						for (j = i; j < r1.owner_num - 1; j++)
						{
							r1.owner_TCB[j] = r1.owner_TCB[j + 1];
							r1.owner_used_num[j] = r1.owner_used_num[j + 1];
						}
						r1.owner_num--;
						break;
					}
				}
				for (i = 0; i < r2.owner_num; i++)
				{
					//有获取r2的资源
					if (r2.owner_TCB[i].name == command[0])
					{
						r2.can_ues_resource_num += r2.owner_used_num[i];//释放资源
						printf("进程%c释放了%d个r2资源!\n", command[0], r2.owner_used_num[i]);
						//后面拥有者往前移动
						for (j = i; j < r2.owner_num - 1; j++)
						{
							r2.owner_TCB[j] = r2.owner_TCB[j + 1];
							r2.owner_used_num[j] = r2.owner_used_num[j + 1];
						}
						r2.owner_num--;
						break;
					}
				}
				
				
				
				//最后再从ready队列中将其删除
				//先从优先级为2的进程中查找
				j = 0;
				for (i = 0; i < program_readylist2_num; i++)
				{
					//查到了此名字，用后面的TCB覆盖掉此进程，并且数量-1
					if (program_readylist2[i].name == command[0])
					{
						for (j = i; j < program_readylist2_num - 1; j++)
						{
							program_readylist2[j] = program_readylist2[j + 1];
						}
						program_readylist2_num--;
						printf("删除进程成功，当前系统进程%c正在运行！\n", program_runing_now.name);
						j = 1;
						break;
					}
				}
				if (j != 0)
					continue;

				//再从优先级为1的进程中查找
				for (i = 0; i < program_readylist1_num; i++)
				{
					if (program_readylist1[i].name == command[0])
					{
						for (j = i; j < program_readylist1_num - 1; j++)
						{
							program_readylist1[j] = program_readylist1[j + 1];
						}
						program_readylist1_num--;
						printf("删除进程成功，当前系统进程%c正在运行！\n", program_runing_now.name);
						j = 1;
						break;
					}
				}
				if (j != 0)
					continue;

				printf("当前系统无此进程，删除进程失败；当前系统进程%c正在运行！\n", program_runing_now.name);
			}
		}


		//-req命令
		else if (strcmp(command, "req") == 0)
		{
			if (runing != true)
			{
				printf("系统还未进行初始化，请先进行初始化！\n");
				continue;
			}

			scanf("%s", &command);//获取资源名字
			if (strcmp(command, "r1") == 0)//获取的是r1资源
			{
				scanf("%s", &command);//获取资源的个数
				i = command[0] - '0';//将char型数字转换为int型数字
				if (i > 5)
				{
					printf("获取的资源数量不能超过五个！\n");
					continue;
				}
				

				if (i <= r1.can_ues_resource_num)//当前剩余资源数能够满足
				{
					r1.can_ues_resource_num -= i;//减掉可用资源数
					//保存当前进程使用资源情况
					r1.owner_TCB[r1.owner_num] = program_runing_now;
					r1.owner_used_num[r1.owner_num] = i;
					r1.owner_num++;

					printf("进程%c成功申请到资源r1！进程%c继续运行！\n", program_runing_now.name, program_runing_now.name);
				}
				else//当前剩余资源数不能满足
				{
					//将当前进程加入到等待队列中
					program_runing_now.status = program_blocked;
					r1.waitlist[r1.waitlist_num] = program_runing_now;
					r1.wait_resource_num[r1.waitlist_num] = i;
					r1.waitlist_num++;
					printf("进程%c申请不到资源r1，", program_runing_now.name);


					//重新调度
					//有优先级为2的进程在ready队列中，切换到该进程执行
					if (program_readylist2_num != 0)
					{
						//弹出最早进程
						program_runing_now = program_readylist2[0];
						program_runing_now.status = program_runing;
						//后面进程往前移动
						for (i = 0; i < program_readylist2_num - 1; i++)
						{
							program_readylist2[i] = program_readylist2[i + 1];
						}
						//数量-1
						program_readylist2_num--;
						printf("切换到进程%c运行！\n", program_runing_now.name);
					}

					//有优先级为1的进程在ready队列中，切换到该进程执行
					else if (program_readylist1_num != 0)
					{
						//弹出最早进程
						program_runing_now = program_readylist1[0];
						program_runing_now.status = program_runing;
						//后面进程往前移动
						for (i = 0; i < program_readylist1_num - 1; i++)
						{
							program_readylist1[i] = program_readylist1[i + 1];
						}
						//数量-1
						program_readylist1_num--;
						printf("切换到进程%c运行！\n", program_runing_now.name);
					}
					//优先级1、2无进程了，切换到init进程执行
					else
					{
						program_runing_now = program_readylist0;
						printf("切换到进程init运行！\n");
					}
				}
			}
			else if (strcmp(command, "r2") == 0)//获取的是r2资源
			{
				scanf("%s", &command);//获取资源的个数
				i = command[0] - '0';//将char型数字转换为int型数字
				if (i > 5)
				{
					printf("获取的资源数量不能超过五个！\n");
					continue;
				}


				if (i <= r2.can_ues_resource_num)//当前剩余资源数能够满足
				{
					r2.can_ues_resource_num -= i;//减掉可用资源数
					//保存当前进程使用资源情况
					r2.owner_TCB[r2.owner_num] = program_runing_now;
					r2.owner_used_num[r2.owner_num] = i;
					r2.owner_num++;

					printf("进程%c成功申请到资源r2！进程%c继续运行！\n", program_runing_now.name, program_runing_now.name);
				}
				else//当前剩余资源数不能满足
				{
					//将当前进程加入到等待队列中
					program_runing_now.status = program_blocked;
					r2.waitlist[r2.waitlist_num] = program_runing_now;
					r2.wait_resource_num[r2.waitlist_num] = i;
					r2.waitlist_num++;
					printf("进程%c申请不到资源r2，", program_runing_now.name);


					//重新调度
					//有优先级为2的进程在ready队列中，切换到该进程执行
					if (program_readylist2_num != 0)
					{
						//弹出最早进程
						program_runing_now = program_readylist2[0];
						program_runing_now.status = program_runing;
						//后面进程往前移动
						for (i = 0; i < program_readylist2_num - 1; i++)
						{
							program_readylist2[i] = program_readylist2[i + 1];
						}
						//数量-1
						program_readylist2_num--;
						printf("切换到进程%c运行！\n", program_runing_now.name);
					}

					//有优先级为1的进程在ready队列中，切换到该进程执行
					else if (program_readylist1_num != 0)
					{
						//弹出最早进程
						program_runing_now = program_readylist1[0];
						program_runing_now.status = program_runing;
						//后面进程往前移动
						for (i = 0; i < program_readylist1_num - 1; i++)
						{
							program_readylist1[i] = program_readylist1[i + 1];
						}
						//数量-1
						program_readylist1_num--;
						printf("切换到进程%c运行！\n", program_runing_now.name);
					}
					//优先级1、2无进程了，切换到init进程执行
					else
					{
						program_runing_now = program_readylist0;
						printf("切换到进程init运行！\n");
					}
				}
			}
			else//获取的资源名字不正确
			{
				printf("获取的资源名字不正确！资源名为r1和r2！\n");
				scanf("%s", &command);//获取资源的个数，不然会对以后的命令产生影响
			}

		}


		//-rel命令
		else if (strcmp(command, "rel") == 0)
		{
			if (runing != true)
			{
				printf("系统还未进行初始化，请先进行初始化！\n");
				continue;
			}

			scanf("%s", &command);//获取释放资源的名字
			if (strcmp(command, "r1") == 0)//释放的资源为r1
			{
				scanf("%s", &command);//获取释放的个数
				i = command[0] - '0';

				for (j = 0; j < r1.owner_num; j++)
				{
					//找到使用者存放的位置
					if (program_runing_now.name == r1.owner_TCB[j].name)
					{
						if (i < r1.owner_used_num[j])//当前使用者没有完全释放资源个数
						{
							r1.owner_used_num[j] -= i;//释放资源
							r1.can_ues_resource_num += i;//资源可用数增加
							printf("当前进程%c释放了%d个r1资源！\n", program_runing_now.name, i);
						}
						else//释放所有占用的r1资源
						{
							r1.can_ues_resource_num += r1.owner_used_num[j];//释放当前所有资源
							
							if (i == r1.owner_used_num[j])
								printf("当前进程%c释放了%d个r1资源（全部）！\n", program_runing_now.name, r1.owner_used_num[j]);
							else
								printf("释放的资源数多于拥有数，但是当前进程%c释放了%d个r1资源（所拥有的）！\n", program_runing_now.name, r1.owner_used_num[j]);

							//在其后面的进程往前移动
							for (i = j; i < r1.owner_num - 1; i++)
							{
								r1.owner_TCB[i] = r1.owner_TCB[i + 1];
								r1.owner_used_num[i] = r1.owner_used_num[i + 1];
							}
							r1.owner_num--;
						}	

						//查看有无任务在等待队列中
						if (r1.waitlist_num != 0)
						{
							//释放完资源后有新的进程可以得到资源
							if (r1.wait_resource_num[0] <= r1.can_ues_resource_num)
							{
								printf("此时%c进程可以获得其所需的r1资源个数！\n", r1.owner_TCB[0].name);
								r1.can_ues_resource_num -= r1.wait_resource_num[0];//减去申请的个数
								r1.waitlist[0].status = program_ready;//进程状态改为ready

								//加入到对应的等待队列中
								if (r1.waitlist[0].prio == 1)
								{
									program_readylist1[program_readylist1_num] = r1.waitlist[0];
									program_readylist1_num++;
								}
								else
								{
									program_readylist2[program_readylist2_num] = r1.waitlist[0];
									program_readylist2_num++;
								}
								//等待资源队列往前移动
								for (i = 0; i < r1.waitlist_num - 1; i++)
								{
									r1.waitlist[i] = r1.waitlist[i + 1];
									r1.wait_resource_num[i] = r1.wait_resource_num[i + 1];
								}
								r1.waitlist_num--;
							}
							else
							{
								printf("此时剩余的资源个数不够最近的进程申请，所以暂不申请！\n");
							}
						}
						j--;
						break;
					}
				}
				if (j == r1.owner_num)
					printf("当前进程没有获取r1资源，无法释放资源！\n");
			}
			else if (strcmp(command, "r2") == 0)//释放的资源为r2
			{
				scanf("%s", &command);//获取释放的个数
				i = command[0] - '0';

				for (j = 0; j < r2.owner_num; j++)
				{
					//找到使用者存放的位置
					if (program_runing_now.name == r2.owner_TCB[j].name)
					{
						if (i < r2.owner_used_num[j])//当前使用者没有完全释放资源个数
						{
							r2.owner_used_num[j] -= i;//释放资源
							r2.can_ues_resource_num += i;//资源可用数增加
							printf("当前进程%c释放了%d个r2资源！\n", program_runing_now.name, i);
						}
						else//释放所有占用的r2资源
						{
							r2.can_ues_resource_num += r2.owner_used_num[j];//释放当前所有资源

							if (i == r2.owner_used_num[j])
								printf("当前进程%c释放了%d个r2资源（全部）！\n", program_runing_now.name, r2.owner_used_num[j]);
							else
								printf("释放的资源数多于拥有数，但是当前进程%c释放了%d个r2资源（所拥有的）！\n", program_runing_now.name, r2.owner_used_num[j]);

							//在其后面的进程往前移动
							for (i = j; i < r2.owner_num - 1; i++)
							{
								r2.owner_TCB[i] = r2.owner_TCB[i + 1];
								r2.owner_used_num[i] = r2.owner_used_num[i + 1];
							}
							r2.owner_num--;
						}

						//查看有无任务在等待队列中
						if (r2.waitlist_num != 0)
						{
							//释放完资源后有新的进程可以得到资源
							if (r2.wait_resource_num[0] <= r2.can_ues_resource_num)
							{
								printf("此时%c进程可以获得其所需的r2资源个数！\n", r2.owner_TCB[0].name);
								r2.can_ues_resource_num -= r2.wait_resource_num[0];//减去申请的个数
								r2.waitlist[0].status = program_ready;//进程状态改为ready

																	  //加入到对应的等待队列中
								if (r2.waitlist[0].prio == 1)
								{
									program_readylist1[program_readylist1_num] = r2.waitlist[0];
									program_readylist1_num++;
								}
								else
								{
									program_readylist2[program_readylist2_num] = r2.waitlist[0];
									program_readylist2_num++;
								}
								//等待资源队列往前移动
								for (i = 0; i < r2.waitlist_num - 1; i++)
								{
									r2.waitlist[i] = r2.waitlist[i + 1];
									r2.wait_resource_num[i] = r2.wait_resource_num[i + 1];
								}
								r2.waitlist_num--;
							}
							else
							{
								printf("此时剩余的资源个数不够最近的进程申请，所以暂不申请！\n");
							}
						}
						j--;
						break;
					}
				}
				if (j == r2.owner_num)
					printf("当前进程没有获取r2资源，无法释放资源！\n");
			}
			else//释放的资源不是r1、r2，出错
			{
				printf("释放资源出错，资源名字应为r1或r2！\n");
				scanf("%s", &command);
			}
		}


		//-to命令
		else if (strcmp(command, "to") == 0)
		{
			if (runing != true)
			{
				printf("系统还未进行初始化，请先进行初始化！\n");
				continue;
			}

			//program_readylist2有ready进程
			if (program_readylist2_num != 0)
			{
				//将当前运行的进程压入对应的等待队列
				if (program_runing_now.prio == 2)
				{
					program_runing_now.status = program_ready;
					program_readylist2[program_readylist2_num] = program_runing_now;
					program_readylist2_num++;
				}
				else
				{
					program_runing_now.status = program_ready;
					program_readylist1[program_readylist1_num] = program_runing_now;
					program_readylist1_num++;
				}
				printf("当前进程（%c）时间片用完，", program_runing_now.name);

				//重新调度
				program_runing_now = program_readylist2[0];
				for (i = 0; i < program_readylist2_num - 1; i++)
				{
					program_readylist2[i] = program_readylist2[i + 1];
				}
				program_readylist2_num--;
				printf("切换到进程%c运行！\n", program_runing_now.name);
			}

			//program_readylist1有ready进程
			else if (program_readylist1_num != 0)
			{
				//将当前运行的进程压入对应的等待队列
				if (program_runing_now.prio == 2)
				{
					program_runing_now.status = program_ready;
					program_readylist2[program_readylist2_num] = program_runing_now;
					program_readylist2_num++;
				}
				else
				{
					program_runing_now.status = program_ready;
					program_readylist1[program_readylist1_num] = program_runing_now;
					program_readylist1_num++;
				}
				printf("当前进程（%c）时间片用完，", program_runing_now.name);

				//重新调度
				program_runing_now = program_readylist1[0];
				for (i = 0; i < program_readylist1_num - 1; i++)
				{
					program_readylist1[i] = program_readylist1[i + 1];
				}
				program_readylist1_num--;
				printf("切换到进程%c运行！\n", program_runing_now.name);
			}

			else//program_readylist2和program_readylist1都没有就绪任务了
			{
				//将当前运行的进程压入对应的等待队列
				if (program_runing_now.prio == 2)
				{
					program_runing_now.status = program_ready;
					program_readylist2[program_readylist2_num] = program_runing_now;
					program_readylist2_num++;
				}
				else
				{
					program_runing_now.status = program_ready;
					program_readylist1[program_readylist1_num] = program_runing_now;
					program_readylist1_num++;
				}
				printf("当前进程（%c）时间片用完，", program_runing_now.name);

				//切换到init进程运行
				program_runing_now = program_readylist0;
				printf("切换到进程init运行！\n");
			}
		}


		//-list命令集
		else if (strcmp(command, "list") == 0)
		{
			scanf("%s", &command);

			if (strcmp(command, "ready") == 0)
			{
				printf("就绪队列2中的进程：");
				for (i = 0; i < program_readylist2_num; i++)
				{
					printf("%c  ", program_readylist2[i].name);
				}
				printf("\n");

				printf("就绪队列1中的进程：");
				for (i = 0; i < program_readylist1_num; i++)
				{
					printf("%c  ", program_readylist1[i].name);
				}
				printf("\n");

				printf("0：init\n");
			}

			else if (strcmp(command, "block") == 0)
			{
				printf("等待r1资源的进程：");
				for (i = 0; i < r1.waitlist_num; i++)
				{
					printf("%c  ", r1.waitlist[i].name);
				}
				printf("\n");

				printf("等待r2资源的进程：");
				for (i = 0; i < r2.waitlist_num; i++)
				{
					printf("%c  ", r2.waitlist[i].name);
				}
				printf("\n");
			}

			else if (strcmp(command, "res") == 0)
			{
				printf("r1剩余可用资源数：");
				printf("%d\n", r1.can_ues_resource_num);

				printf("r2剩余可用资源数：");
				printf("%d\n", r2.can_ues_resource_num);
			}

			else
			{
				printf("输入的命令有错，请重新输入！\n");
			}
		}


		else
		{
			printf("输入的命令有错，请重新输入！\n");
		}
	}
}
#endif
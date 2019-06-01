#if 1
#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct tcb
{
	char name;//����
	int status;//��ǰ״̬
	int prio;//���ȼ�

	struct tcb *father = NULL;//�����TCB
	int son_num = 0;//�ӽ�����
	struct tcb *son[16];//�ӽ��TCB
}TCB;

typedef struct
{
	//char name[8];//��Դ����
	int can_ues_resource_num = 5;//������Դ��

	int owner_num = 0;//ʹ���߸���
	TCB owner_TCB[8];//ʹ������������
	int owner_used_num[8];//ʹ����ʹ����Դ����
	
	int waitlist_num = 0;//�ȴ����еĸ���
	int wait_resource_num[8];//�ȴ���Դ��Ŀ
	TCB waitlist[8];//�ȴ�����
}resource;


#define program_runing	0
#define program_ready	1
#define program_blocked	2

bool runing = false;//ϵͳ����״̬

TCB program_runing_now;//��ǰ���еĽ��̵�TCB
TCB program_readylist0;//prio0TCB
TCB program_readylist1[128];//prio1TCB����
TCB program_readylist2[128];//prio2TCB����

int program_readylist1_num = 0;//prio1TCB����ʹ�����
int program_readylist2_num = 0;//prio2TCB����ʹ�����


int main()
{
	int i, j;
	char command[128];
	TCB *p;
	//����������Դ������Ĭ�ϳ�ʼֵ
	resource r1, r2;
	while (1)
	{
		printf("shell>");
		scanf("%s", &command);//��ȡ����

		//-init����
		if (strcmp(command, "init") == 0)
		{
			if (runing == true)
			{
				printf("ϵͳ�ѳ�ʼ�����������ٽ��г�ʼ����\n");
				continue;
			}
			runing = true;
			
			//����init���̲����뵽program_readylist0��
			//����ǰ���еĽ�����Ϊinit����
			program_readylist0.name = ' ';
			program_readylist0.prio = 0;
			program_readylist0.status = program_runing;
			
			//���ӽڵ���Ϣ����Ĭ��ֵ
			program_readylist0.father = NULL;
			program_readylist0.son_num = 0;

			//��ʼ��r1��r2��Դ
			r1.can_ues_resource_num = 5;
			r1.owner_num = 0;
			r1.waitlist_num = 0;
			r2.can_ues_resource_num = 5;
			r2.owner_num = 0;
			r2.waitlist_num = 0;

			program_runing_now = program_readylist0;

			printf("ϵͳ��ʼ���ɹ���init�����������У�\n");
		}


		//-cr����
		else if (strcmp(command, "cr") == 0)
		{
			if (runing != true)
			{
				printf("ϵͳ��δ���г�ʼ�������Ƚ��г�ʼ����\n");
				continue;
			}

			//��̬����һ��TCB�ṹ��
			p = (TCB*)malloc(sizeof(TCB));
			p->status = program_ready;
			scanf("%s", &command);//��ȡcr�����һ�����������֣�
			p->name = command[0];
			scanf("%s", &command); //��ȡcr����ڶ������������ȼ���
			if (command[0] == '1')
				p->prio = 1;
			else if(command[0] == '2')
				p->prio = 2;
			else//�����ж�
			{
				printf("��������ȼ�ֻ��Ϊ1��2������Ϊ����ֵ��\n");
				free(p);
				continue;
			}

			//�½����̵ĸ����Ϊ��ǰ����
			p->father = &program_runing_now;
			p->son_num = 0;//��ʱ�½��̻�û�к��ӽ��
			//���½����̼��뵽���ڽ��̵ĺ��ӽ��
			program_runing_now.son[program_runing_now.son_num] = p;
			program_runing_now.son_num++;
			
			//��ǰ�����Ľ������ȼ�����ʱ���л���������
			if (p->prio > program_runing_now.prio)
			{
				//init����һ��ʼ�ʹ���program_readylist0���棬ֱ���л��½��̾���
				if (program_runing_now.prio == 0)
				{
					//init���̱�Ϊready״̬
					program_runing_now.status = program_ready;
					program_runing_now = *p;//�л����½���ִ��
					program_runing_now.status = program_runing;//�½��̱�Ϊִ��״̬
				}
				else if (program_runing_now.prio == 1)
				{
					//���ϵĽ���ѹ���Ӧ��ready���飬���л��½���
					program_runing_now.status = program_ready;
					program_readylist1[program_readylist1_num] = program_runing_now;
					program_readylist1_num++;
					program_runing_now = *p;
					program_runing_now.status = program_runing;
				}
				
			}
			else//�����Ľ������ȼ��޸���ʱ�������Ӧ��TCB��������
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
			printf("����%c�������У�\n", program_runing_now.name);
		}


		//-de����
		else if (strcmp(command, "de") == 0)
		{
			if (runing != true)
			{
				printf("ϵͳ��δ���г�ʼ�������Ƚ��г�ʼ����\n");
				continue;
			}
			
			scanf("%s", &command);//��ȡҪɾ���Ľ�������
			
			if (program_runing_now.name == command[0])//ɾ���Ľ����ǵ�ǰ����
			{
				//����ǰ���̺��ӽ��ĸ����ת��Ϊ��ǰ���̵ĸ����
				//�����ӱ�ɺ��ӣ�
				p = program_runing_now.father;//pָ��ǰ���̵ĸ����
				for (i = 0; i < program_runing_now.son_num; i++)
				{ 
					p->son[p->son_num] = program_runing_now.son[i];
					p->son_num++;
				}
				
				//�ж�����ռ��r1��Դ��ֻ�赽ӵ���߶���ȥ�ң�
				for (i = 0; i < r1.owner_num; i++)
				{
					//�����ռ����Դ�������ͷ�
					if (program_runing_now.name == r1.owner_TCB[i].name)
					{
						r1.can_ues_resource_num += r1.owner_used_num[i];
						printf("��ǰ����%c�ͷ���%d��r1��Դ!\n", program_runing_now.name, r1.owner_used_num[i]);
						//�����ӵ������ǰ�ƶ�
						for (j = i; j < r1.owner_num - 1; j++)
						{
							r1.owner_TCB[j] = r1.owner_TCB[j + 1];
							r1.owner_used_num[j] = r1.owner_used_num[j + 1];
						}
						r1.owner_num--;
						break;
					}
				}

				//�ж�����ռ��r2��Դ��ֻ�赽ӵ���߶���ȥ�ң�
				for (i = 0; i < r2.owner_num; i++)
				{
					//�����ռ����Դ�������ͷ�
					if (program_runing_now.name == r2.owner_TCB[i].name)
					{
						r2.can_ues_resource_num += r2.owner_used_num[i];
						printf("��ǰ����%c�ͷ���%d��r2��Դ!\n", program_runing_now.name, r2.owner_used_num[i]);
						//�����ӵ������ǰ�ƶ�
						for (j = i; j < r2.owner_num - 1; j++)
						{
							r2.owner_TCB[j] = r2.owner_TCB[j + 1];
							r2.owner_used_num[j] = r2.owner_used_num[j + 1];
						}
						r2.owner_num--;
						break;
					}
				}


				//���½���ϵͳ����
				//�����ȼ�Ϊ2�Ľ�����ready�����У��л����ý���ִ��
				if (program_readylist2_num != 0)
				{
					//�����������
					program_runing_now = program_readylist2[0];
					program_runing_now.status = program_runing;
					//���������ǰ�ƶ�
					for (i = 0; i < program_readylist2_num - 1; i++)
					{
						program_readylist2[i] = program_readylist2[i + 1];
					}
					//����-1
					program_readylist2_num--;
					printf("ɾ�����̳ɹ�����ǰϵͳ����%c�������У�\n", program_runing_now.name);
				}

				//�����ȼ�Ϊ1�Ľ�����ready�����У��л����ý���ִ��
				else if (program_readylist1_num != 0)
				{
					//�����������
					program_runing_now = program_readylist1[0];
					program_runing_now.status = program_runing;
					//���������ǰ�ƶ�
					for (i = 0; i < program_readylist1_num - 1; i++)
					{
						program_readylist1[i] = program_readylist1[i + 1];
					}
					//����-1
					program_readylist1_num--;
					printf("ɾ�����̳ɹ�����ǰϵͳ����%c�������У�\n", program_runing_now.name);
				}
				//���ȼ�1��2�޽����ˣ��л���init����ִ��
				else
				{
					program_runing_now = program_readylist0;
					printf("ɾ�����̳ɹ�����ǰϵͳ����init�������У�\n");
				}
				
			}
			//��ready�����в������޴˽�������
			else//ɾ���Ĳ��ǵ�ǰ����
			{
				j = 0;
				//�Ȳ鿴�˽������޻�ȡr1��r2����Դ������У���ֱ��ɾ��
				//���ڶ�����������
				for (i = 0; i < r1.waitlist_num; i++)
				{
					//r1�Ķ����������У�ֱ�Ӹ��ǵ�����
					if (r1.waitlist[i].name == command[0])
					{
						//����ǰ���̺��ӽ��ĸ����ת��Ϊ��ǰ���̵ĸ����
						//�����ӱ�ɺ��ӣ�
						p = r1.waitlist[i].father;
						for (j = 0; j < r1.waitlist[i].son_num; j++)
						{
							p->son[p->son_num] = r1.waitlist[i].son[j];
							p->son_num++;
						}
						//����ȴ����и���
						for (j = i; j < r1.waitlist_num - 1; j++)
						{
							r1.waitlist[j] = r1.waitlist[j + 1];
							r1.wait_resource_num[j] = r1.wait_resource_num[j + 1];
						}
						r1.waitlist_num--;
						printf("����%c�Ѵӵȴ�r1��Դ�������Ƴ���\n", command[0]);
						j = 1;
						break;
					}
				}
				for (i = 0; i < r2.waitlist_num; i++)
				{
					//r1�Ķ����������У�ֱ�Ӹ��ǵ�����
					if (r2.waitlist[i].name == command[0])
					{
						//����ǰ���̺��ӽ��ĸ����ת��Ϊ��ǰ���̵ĸ����
						//�����ӱ�ɺ��ӣ�
						p = r1.waitlist[i].father;
						for (j = 0; j < r1.waitlist[i].son_num; j++)
						{
							p->son[p->son_num] = r1.waitlist[i].son[j];
							p->son_num++;
						}
						
						//����ȴ����и���
						for (j = i; j < r2.waitlist_num - 1; j++)
						{
							r2.waitlist[j] = r2.waitlist[j + 1];
							r2.wait_resource_num[j] = r2.wait_resource_num[j + 1];
						}
						r2.waitlist_num--;
						printf("����%c�Ѵӵȴ�r2��Դ�������Ƴ���\n", command[0]);
						j = 1;
						break;
					}
				}
				if (j != 0)
					continue;



				//Ȼ����ӵ���߶��в���
				for (i = 0; i < r1.owner_num; i++)
				{
					//�л�ȡr1����Դ
					if (r1.owner_TCB[i].name == command[0])
					{
						r1.can_ues_resource_num += r1.owner_used_num[i];//�ͷ���Դ
						printf("����%c�ͷ���%d��r1��Դ!\n", command[0], r1.owner_used_num[i]);
						//����ӵ������ǰ�ƶ�
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
					//�л�ȡr2����Դ
					if (r2.owner_TCB[i].name == command[0])
					{
						r2.can_ues_resource_num += r2.owner_used_num[i];//�ͷ���Դ
						printf("����%c�ͷ���%d��r2��Դ!\n", command[0], r2.owner_used_num[i]);
						//����ӵ������ǰ�ƶ�
						for (j = i; j < r2.owner_num - 1; j++)
						{
							r2.owner_TCB[j] = r2.owner_TCB[j + 1];
							r2.owner_used_num[j] = r2.owner_used_num[j + 1];
						}
						r2.owner_num--;
						break;
					}
				}
				
				
				
				//����ٴ�ready�����н���ɾ��
				//�ȴ����ȼ�Ϊ2�Ľ����в���
				j = 0;
				for (i = 0; i < program_readylist2_num; i++)
				{
					//�鵽�˴����֣��ú����TCB���ǵ��˽��̣���������-1
					if (program_readylist2[i].name == command[0])
					{
						for (j = i; j < program_readylist2_num - 1; j++)
						{
							program_readylist2[j] = program_readylist2[j + 1];
						}
						program_readylist2_num--;
						printf("ɾ�����̳ɹ�����ǰϵͳ����%c�������У�\n", program_runing_now.name);
						j = 1;
						break;
					}
				}
				if (j != 0)
					continue;

				//�ٴ����ȼ�Ϊ1�Ľ����в���
				for (i = 0; i < program_readylist1_num; i++)
				{
					if (program_readylist1[i].name == command[0])
					{
						for (j = i; j < program_readylist1_num - 1; j++)
						{
							program_readylist1[j] = program_readylist1[j + 1];
						}
						program_readylist1_num--;
						printf("ɾ�����̳ɹ�����ǰϵͳ����%c�������У�\n", program_runing_now.name);
						j = 1;
						break;
					}
				}
				if (j != 0)
					continue;

				printf("��ǰϵͳ�޴˽��̣�ɾ������ʧ�ܣ���ǰϵͳ����%c�������У�\n", program_runing_now.name);
			}
		}


		//-req����
		else if (strcmp(command, "req") == 0)
		{
			if (runing != true)
			{
				printf("ϵͳ��δ���г�ʼ�������Ƚ��г�ʼ����\n");
				continue;
			}

			scanf("%s", &command);//��ȡ��Դ����
			if (strcmp(command, "r1") == 0)//��ȡ����r1��Դ
			{
				scanf("%s", &command);//��ȡ��Դ�ĸ���
				i = command[0] - '0';//��char������ת��Ϊint������
				if (i > 5)
				{
					printf("��ȡ����Դ�������ܳ��������\n");
					continue;
				}
				

				if (i <= r1.can_ues_resource_num)//��ǰʣ����Դ���ܹ�����
				{
					r1.can_ues_resource_num -= i;//����������Դ��
					//���浱ǰ����ʹ����Դ���
					r1.owner_TCB[r1.owner_num] = program_runing_now;
					r1.owner_used_num[r1.owner_num] = i;
					r1.owner_num++;

					printf("����%c�ɹ����뵽��Դr1������%c�������У�\n", program_runing_now.name, program_runing_now.name);
				}
				else//��ǰʣ����Դ����������
				{
					//����ǰ���̼��뵽�ȴ�������
					program_runing_now.status = program_blocked;
					r1.waitlist[r1.waitlist_num] = program_runing_now;
					r1.wait_resource_num[r1.waitlist_num] = i;
					r1.waitlist_num++;
					printf("����%c���벻����Դr1��", program_runing_now.name);


					//���µ���
					//�����ȼ�Ϊ2�Ľ�����ready�����У��л����ý���ִ��
					if (program_readylist2_num != 0)
					{
						//�����������
						program_runing_now = program_readylist2[0];
						program_runing_now.status = program_runing;
						//���������ǰ�ƶ�
						for (i = 0; i < program_readylist2_num - 1; i++)
						{
							program_readylist2[i] = program_readylist2[i + 1];
						}
						//����-1
						program_readylist2_num--;
						printf("�л�������%c���У�\n", program_runing_now.name);
					}

					//�����ȼ�Ϊ1�Ľ�����ready�����У��л����ý���ִ��
					else if (program_readylist1_num != 0)
					{
						//�����������
						program_runing_now = program_readylist1[0];
						program_runing_now.status = program_runing;
						//���������ǰ�ƶ�
						for (i = 0; i < program_readylist1_num - 1; i++)
						{
							program_readylist1[i] = program_readylist1[i + 1];
						}
						//����-1
						program_readylist1_num--;
						printf("�л�������%c���У�\n", program_runing_now.name);
					}
					//���ȼ�1��2�޽����ˣ��л���init����ִ��
					else
					{
						program_runing_now = program_readylist0;
						printf("�л�������init���У�\n");
					}
				}
			}
			else if (strcmp(command, "r2") == 0)//��ȡ����r2��Դ
			{
				scanf("%s", &command);//��ȡ��Դ�ĸ���
				i = command[0] - '0';//��char������ת��Ϊint������
				if (i > 5)
				{
					printf("��ȡ����Դ�������ܳ��������\n");
					continue;
				}


				if (i <= r2.can_ues_resource_num)//��ǰʣ����Դ���ܹ�����
				{
					r2.can_ues_resource_num -= i;//����������Դ��
					//���浱ǰ����ʹ����Դ���
					r2.owner_TCB[r2.owner_num] = program_runing_now;
					r2.owner_used_num[r2.owner_num] = i;
					r2.owner_num++;

					printf("����%c�ɹ����뵽��Դr2������%c�������У�\n", program_runing_now.name, program_runing_now.name);
				}
				else//��ǰʣ����Դ����������
				{
					//����ǰ���̼��뵽�ȴ�������
					program_runing_now.status = program_blocked;
					r2.waitlist[r2.waitlist_num] = program_runing_now;
					r2.wait_resource_num[r2.waitlist_num] = i;
					r2.waitlist_num++;
					printf("����%c���벻����Դr2��", program_runing_now.name);


					//���µ���
					//�����ȼ�Ϊ2�Ľ�����ready�����У��л����ý���ִ��
					if (program_readylist2_num != 0)
					{
						//�����������
						program_runing_now = program_readylist2[0];
						program_runing_now.status = program_runing;
						//���������ǰ�ƶ�
						for (i = 0; i < program_readylist2_num - 1; i++)
						{
							program_readylist2[i] = program_readylist2[i + 1];
						}
						//����-1
						program_readylist2_num--;
						printf("�л�������%c���У�\n", program_runing_now.name);
					}

					//�����ȼ�Ϊ1�Ľ�����ready�����У��л����ý���ִ��
					else if (program_readylist1_num != 0)
					{
						//�����������
						program_runing_now = program_readylist1[0];
						program_runing_now.status = program_runing;
						//���������ǰ�ƶ�
						for (i = 0; i < program_readylist1_num - 1; i++)
						{
							program_readylist1[i] = program_readylist1[i + 1];
						}
						//����-1
						program_readylist1_num--;
						printf("�л�������%c���У�\n", program_runing_now.name);
					}
					//���ȼ�1��2�޽����ˣ��л���init����ִ��
					else
					{
						program_runing_now = program_readylist0;
						printf("�л�������init���У�\n");
					}
				}
			}
			else//��ȡ����Դ���ֲ���ȷ
			{
				printf("��ȡ����Դ���ֲ���ȷ����Դ��Ϊr1��r2��\n");
				scanf("%s", &command);//��ȡ��Դ�ĸ�������Ȼ����Ժ���������Ӱ��
			}

		}


		//-rel����
		else if (strcmp(command, "rel") == 0)
		{
			if (runing != true)
			{
				printf("ϵͳ��δ���г�ʼ�������Ƚ��г�ʼ����\n");
				continue;
			}

			scanf("%s", &command);//��ȡ�ͷ���Դ������
			if (strcmp(command, "r1") == 0)//�ͷŵ���ԴΪr1
			{
				scanf("%s", &command);//��ȡ�ͷŵĸ���
				i = command[0] - '0';

				for (j = 0; j < r1.owner_num; j++)
				{
					//�ҵ�ʹ���ߴ�ŵ�λ��
					if (program_runing_now.name == r1.owner_TCB[j].name)
					{
						if (i < r1.owner_used_num[j])//��ǰʹ����û����ȫ�ͷ���Դ����
						{
							r1.owner_used_num[j] -= i;//�ͷ���Դ
							r1.can_ues_resource_num += i;//��Դ����������
							printf("��ǰ����%c�ͷ���%d��r1��Դ��\n", program_runing_now.name, i);
						}
						else//�ͷ�����ռ�õ�r1��Դ
						{
							r1.can_ues_resource_num += r1.owner_used_num[j];//�ͷŵ�ǰ������Դ
							
							if (i == r1.owner_used_num[j])
								printf("��ǰ����%c�ͷ���%d��r1��Դ��ȫ������\n", program_runing_now.name, r1.owner_used_num[j]);
							else
								printf("�ͷŵ���Դ������ӵ���������ǵ�ǰ����%c�ͷ���%d��r1��Դ����ӵ�еģ���\n", program_runing_now.name, r1.owner_used_num[j]);

							//�������Ľ�����ǰ�ƶ�
							for (i = j; i < r1.owner_num - 1; i++)
							{
								r1.owner_TCB[i] = r1.owner_TCB[i + 1];
								r1.owner_used_num[i] = r1.owner_used_num[i + 1];
							}
							r1.owner_num--;
						}	

						//�鿴���������ڵȴ�������
						if (r1.waitlist_num != 0)
						{
							//�ͷ�����Դ�����µĽ��̿��Եõ���Դ
							if (r1.wait_resource_num[0] <= r1.can_ues_resource_num)
							{
								printf("��ʱ%c���̿��Ի���������r1��Դ������\n", r1.owner_TCB[0].name);
								r1.can_ues_resource_num -= r1.wait_resource_num[0];//��ȥ����ĸ���
								r1.waitlist[0].status = program_ready;//����״̬��Ϊready

								//���뵽��Ӧ�ĵȴ�������
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
								//�ȴ���Դ������ǰ�ƶ�
								for (i = 0; i < r1.waitlist_num - 1; i++)
								{
									r1.waitlist[i] = r1.waitlist[i + 1];
									r1.wait_resource_num[i] = r1.wait_resource_num[i + 1];
								}
								r1.waitlist_num--;
							}
							else
							{
								printf("��ʱʣ�����Դ������������Ľ������룬�����ݲ����룡\n");
							}
						}
						j--;
						break;
					}
				}
				if (j == r1.owner_num)
					printf("��ǰ����û�л�ȡr1��Դ���޷��ͷ���Դ��\n");
			}
			else if (strcmp(command, "r2") == 0)//�ͷŵ���ԴΪr2
			{
				scanf("%s", &command);//��ȡ�ͷŵĸ���
				i = command[0] - '0';

				for (j = 0; j < r2.owner_num; j++)
				{
					//�ҵ�ʹ���ߴ�ŵ�λ��
					if (program_runing_now.name == r2.owner_TCB[j].name)
					{
						if (i < r2.owner_used_num[j])//��ǰʹ����û����ȫ�ͷ���Դ����
						{
							r2.owner_used_num[j] -= i;//�ͷ���Դ
							r2.can_ues_resource_num += i;//��Դ����������
							printf("��ǰ����%c�ͷ���%d��r2��Դ��\n", program_runing_now.name, i);
						}
						else//�ͷ�����ռ�õ�r2��Դ
						{
							r2.can_ues_resource_num += r2.owner_used_num[j];//�ͷŵ�ǰ������Դ

							if (i == r2.owner_used_num[j])
								printf("��ǰ����%c�ͷ���%d��r2��Դ��ȫ������\n", program_runing_now.name, r2.owner_used_num[j]);
							else
								printf("�ͷŵ���Դ������ӵ���������ǵ�ǰ����%c�ͷ���%d��r2��Դ����ӵ�еģ���\n", program_runing_now.name, r2.owner_used_num[j]);

							//�������Ľ�����ǰ�ƶ�
							for (i = j; i < r2.owner_num - 1; i++)
							{
								r2.owner_TCB[i] = r2.owner_TCB[i + 1];
								r2.owner_used_num[i] = r2.owner_used_num[i + 1];
							}
							r2.owner_num--;
						}

						//�鿴���������ڵȴ�������
						if (r2.waitlist_num != 0)
						{
							//�ͷ�����Դ�����µĽ��̿��Եõ���Դ
							if (r2.wait_resource_num[0] <= r2.can_ues_resource_num)
							{
								printf("��ʱ%c���̿��Ի���������r2��Դ������\n", r2.owner_TCB[0].name);
								r2.can_ues_resource_num -= r2.wait_resource_num[0];//��ȥ����ĸ���
								r2.waitlist[0].status = program_ready;//����״̬��Ϊready

																	  //���뵽��Ӧ�ĵȴ�������
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
								//�ȴ���Դ������ǰ�ƶ�
								for (i = 0; i < r2.waitlist_num - 1; i++)
								{
									r2.waitlist[i] = r2.waitlist[i + 1];
									r2.wait_resource_num[i] = r2.wait_resource_num[i + 1];
								}
								r2.waitlist_num--;
							}
							else
							{
								printf("��ʱʣ�����Դ������������Ľ������룬�����ݲ����룡\n");
							}
						}
						j--;
						break;
					}
				}
				if (j == r2.owner_num)
					printf("��ǰ����û�л�ȡr2��Դ���޷��ͷ���Դ��\n");
			}
			else//�ͷŵ���Դ����r1��r2������
			{
				printf("�ͷ���Դ������Դ����ӦΪr1��r2��\n");
				scanf("%s", &command);
			}
		}


		//-to����
		else if (strcmp(command, "to") == 0)
		{
			if (runing != true)
			{
				printf("ϵͳ��δ���г�ʼ�������Ƚ��г�ʼ����\n");
				continue;
			}

			//program_readylist2��ready����
			if (program_readylist2_num != 0)
			{
				//����ǰ���еĽ���ѹ���Ӧ�ĵȴ�����
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
				printf("��ǰ���̣�%c��ʱ��Ƭ���꣬", program_runing_now.name);

				//���µ���
				program_runing_now = program_readylist2[0];
				for (i = 0; i < program_readylist2_num - 1; i++)
				{
					program_readylist2[i] = program_readylist2[i + 1];
				}
				program_readylist2_num--;
				printf("�л�������%c���У�\n", program_runing_now.name);
			}

			//program_readylist1��ready����
			else if (program_readylist1_num != 0)
			{
				//����ǰ���еĽ���ѹ���Ӧ�ĵȴ�����
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
				printf("��ǰ���̣�%c��ʱ��Ƭ���꣬", program_runing_now.name);

				//���µ���
				program_runing_now = program_readylist1[0];
				for (i = 0; i < program_readylist1_num - 1; i++)
				{
					program_readylist1[i] = program_readylist1[i + 1];
				}
				program_readylist1_num--;
				printf("�л�������%c���У�\n", program_runing_now.name);
			}

			else//program_readylist2��program_readylist1��û�о���������
			{
				//����ǰ���еĽ���ѹ���Ӧ�ĵȴ�����
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
				printf("��ǰ���̣�%c��ʱ��Ƭ���꣬", program_runing_now.name);

				//�л���init��������
				program_runing_now = program_readylist0;
				printf("�л�������init���У�\n");
			}
		}


		//-list���
		else if (strcmp(command, "list") == 0)
		{
			scanf("%s", &command);

			if (strcmp(command, "ready") == 0)
			{
				printf("��������2�еĽ��̣�");
				for (i = 0; i < program_readylist2_num; i++)
				{
					printf("%c  ", program_readylist2[i].name);
				}
				printf("\n");

				printf("��������1�еĽ��̣�");
				for (i = 0; i < program_readylist1_num; i++)
				{
					printf("%c  ", program_readylist1[i].name);
				}
				printf("\n");

				printf("0��init\n");
			}

			else if (strcmp(command, "block") == 0)
			{
				printf("�ȴ�r1��Դ�Ľ��̣�");
				for (i = 0; i < r1.waitlist_num; i++)
				{
					printf("%c  ", r1.waitlist[i].name);
				}
				printf("\n");

				printf("�ȴ�r2��Դ�Ľ��̣�");
				for (i = 0; i < r2.waitlist_num; i++)
				{
					printf("%c  ", r2.waitlist[i].name);
				}
				printf("\n");
			}

			else if (strcmp(command, "res") == 0)
			{
				printf("r1ʣ�������Դ����");
				printf("%d\n", r1.can_ues_resource_num);

				printf("r2ʣ�������Դ����");
				printf("%d\n", r2.can_ues_resource_num);
			}

			else
			{
				printf("����������д����������룡\n");
			}
		}


		else
		{
			printf("����������д����������룡\n");
		}
	}
}
#endif
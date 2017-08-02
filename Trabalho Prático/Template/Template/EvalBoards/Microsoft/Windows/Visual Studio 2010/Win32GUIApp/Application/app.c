
/*
*********************************************************************************************************
*
*                                          TRABALHO PRÁTICO - BCC722
*
*                                                  JOGO DA NAVE
*
* Arquivo			: app.c
* Versao			: 
* Aluno(s)			:  OSCAR F C LONGUINHO / LUCAS MANOEL / JOSE AUGUSTO
* Data				:  
* Descricao			: Desenvolver o Jogo da Nave
*********************************************************************************************************


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <app_cfg.h>

#include  <os.h>
#include  <os_cfg_app.h>

// biblioteca GUI
#include "gui.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
  
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define TSKSTART_STK_SIZE 2000
#define TSKBG_STK_SIZE 2000
#define TSKSHIP_STK_SIZE 2000
#define TSKENEMY_STK_SIZE 2000
#define TSKTELA_STK_SIZE 2000
#define TSKSHOT_STK_SIZE 2000
#define TSKMOVENE_STK_SIZE 2000
#define NumEnemy 8
#define Linhas 38
#define Colunas 23
#define LinhaNave 21
#define EnemyLine 5
/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

enum GRAPHIC_OBJS{
	ID_BUTTON_1 = 0,
	ID_EDIT_LINE_1,
	ID_LABEL_1,
};

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   AppStartTaskTCB;
static  CPU_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];
static	OS_TCB   TaskTelaTcb;
static  CPU_STK	 TaskTelaStk[TSKTELA_STK_SIZE];
static	OS_TCB	 TaskEnemyTCB[NumEnemy];
static  CPU_STK  TaskEnemyStk[NumEnemy][TSKENEMY_STK_SIZE];
static	OS_TCB	 TaskMoveEnemyTCB[NumEnemy];
static  CPU_STK  TaskMoveEnemyStk[NumEnemy][TSKMOVENE_STK_SIZE];
//Semáforos
static	OS_SEM	 SemaforoTela;
static  OS_SEM	 SemaforoShipPos;
static  OS_SEM	 SemaforoLabrinto;
static	OS_SEM   SemaforoEnemyCount;
static	OS_SEM	 SemaforoPosEnemy;
static  OS_SEM   SemaforoEnemy;
// imagens usadas no programa
HBITMAP * fundo;
HBITMAP * ship;
HBITMAP * enemy;
HBITMAP * missil;
//Variáveis globais
int imgXPos, imgYPos;
int ShipPos = 18;
int EnemyCount = 0;
int xEnemy[NumEnemy];
int yEnemy[NumEnemy];
int Enemy[NumEnemy];
/*
*********************************************************************************************************
*********************************************************************************************************/
/*
*********************************************************************************************************
							LABIRINTOS - CODIFICAÇÃO DOS OBJETOS
0 - vazio
1 - nave
2 - tiro da nave
3 - inimigo 
4 - tiro do inimigo
5 - limite
*********************************************************************************************************/

//Labirinto dos obstáculos
int LABIRINTO[Linhas][Colunas] = {	 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}//LADO ESQUERO INFERIOR
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}
									,{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5} //LADO DIREITO INFERIOR
									,{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}
   };

/*
*********************************************************************************************************
*                                       EXTERN VARIABLES
*********************************************************************************************************
*/

extern HWND hwnd; // Variaveis declaradas do modulo da GUI
extern HDC hdc;
extern MSG Msg;

/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/

#define  APP_TASK_STOP();                             { while (DEF_ON) { \
                                                            ;            \
                                                        }                \
                                                      }


#define  APP_TEST_FAULT(err_var, err_code)            { if ((err_var) != (err_code)) {   \
                                                            APP_TASK_STOP();             \
                                                        }                                \
                                                      }


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart (void  *p_arg);
static  void  TaskTela(void *p_arg);
static	void  MoveShip(int dir);
static	void  Shot(int pos);
static  void  TaskEnemy(void *p_arg);
static	void  EnemyShot(int x,int y);
static	void  MoveEnemy(void *p_arg);
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               main()
*
* Description : Funcao principal do programa.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : 
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err_os;


    OSInit(&err_os);                                            /* inicializa uC/OS-III.                                */
    APP_TEST_FAULT(err_os, OS_ERR_NONE);

    OSTaskCreate((OS_TCB     *)&AppStartTaskTCB,                /* Cria a tarefa inicial.                             */
                 (CPU_CHAR   *)"App Start Task",
                 (OS_TASK_PTR ) App_TaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppStartTaskStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err_os);
    APP_TEST_FAULT(err_os, OS_ERR_NONE);

	
    OSStart(&err_os);                                           /* Inicia o funcionamento do escalonador. */
    APP_TEST_FAULT(err_os, OS_ERR_NONE);
}

/*
*********************************************************************************************************
*                                           TaskTela()
*
* Description : Desenha itens na Tela
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : App_TaskStart().
*
*********************************************************************************************************
*/

static void TaskTela(void *p_arg){
	int i = 0;
	int j = 0;
	OS_ERR err_os;
	while(1){
		ship = GUI_CreateImage("nave.bmp",60,60);
		DeleteObject(enemy);
		enemy = GUI_CreateImage("inimigo.bmp",35,35);
		DeleteObject(fundo);
		DeleteObject(missil);
		missil = GUI_CreateImage("missil.bmp",60,20);
		OSSemPend((OS_SEM		*)&SemaforoTela,
				  (OS_TICK		*) 0,
				  OS_OPT_PEND_BLOCKING,
				  (CPU_TS		*) 0,
				  (OS_ERR		*)&err_os);
		for (i = 0;i< Linhas;i++){
			for (j = 0;j < Colunas;j++){
				OSSemPend((OS_SEM		*)&SemaforoLabrinto,
						 (OS_TICK		*) 0,
						 OS_OPT_PEND_BLOCKING,
						 (CPU_TS		*) 0,
						 (OS_ERR        *)&err_os);
				switch (LABIRINTO[i][j]) {
				case 1 : GUI_DrawImage(ship,i * 20,LinhaNave*20,60,60,1);
					break;
				case 2 : GUI_DrawImage(missil,i * 20,j*20,60,20,1);
					break;
				case 3 : GUI_DrawImage(enemy,i * 20,j*20,35,35,1);
					break;
				}
				OSSemPost((OS_SEM		*)&SemaforoLabrinto,
						  OS_OPT_POST_1,
						  (OS_ERR		*)&err_os);
			}
		}
		OSSemPost((OS_SEM		*)&SemaforoTela,
					OS_OPT_POST_1,
					(OS_ERR		*)&err_os);
		OSTimeDly(10, OS_OPT_TIME_DLY, &err_os);

		
	}


}

/*
*********************************************************************************************************
*                                           MoveShip()
*
* Description : Movimenta a nave na tela
*
* Arguments   :  dir direção é modulo do movimento (negativo esquera, positivo direita)
*
* Returns     : none.
*
*
*********************************************************************************************************
*/

static  void MoveShip(int dir){
	OS_ERR err_os;
	OSSemPend((OS_SEM		*)&SemaforoShipPos,
			 (OS_TICK		*) 0,
			 OS_OPT_PEND_BLOCKING,
			 (CPU_TS		*) 0,
			 (OS_ERR        *)&err_os);
	OSSemPend((OS_SEM		*)&SemaforoLabrinto,
			 (OS_TICK		*) 0,
			 OS_OPT_PEND_BLOCKING,
			 (CPU_TS		*) 0,
			 (OS_ERR        *)&err_os);
	if (LABIRINTO[ShipPos + dir][LinhaNave] != 5){
		LABIRINTO[ShipPos][LinhaNave] = 0;
		ShipPos = ShipPos + dir;
		LABIRINTO[ShipPos][LinhaNave] = 1;
	}
	OSSemPost((OS_SEM		*)&SemaforoLabrinto,
			  OS_OPT_POST_1,
			  (OS_ERR		*)&err_os);
	OSSemPost((OS_SEM		*)&SemaforoShipPos,
			  OS_OPT_POST_1,
			  (OS_ERR		*)&err_os);
}
/*
*********************************************************************************************************
*                                           Shot()
*
* Description : Tiro da nave.
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : CriaShot(int pos).
*
*********************************************************************************************************
*/
static void Shot(int pos)
{
	OS_ERR err_os;
	int i;
	int y = LinhaNave-1;
	OSSemPend((OS_SEM		*)&SemaforoLabrinto,
				(OS_TICK	*) 0,
				OS_OPT_PEND_BLOCKING,
				(CPU_TS		*) 0,
				(OS_ERR     *)&err_os);
	while ((LABIRINTO[pos][y-1] != 5) &&  (LABIRINTO[pos][y-1] != 3)){
		LABIRINTO[pos][y] = 0;
		y--;
		LABIRINTO[pos][y] = 2;
		OSTimeDly(10, OS_OPT_TIME_DLY, &err_os);
		OSSemPost((OS_SEM		*)&SemaforoLabrinto,
				   OS_OPT_POST_1,
				  (OS_ERR		*)&err_os);
	}
	OSSemPend((OS_SEM		*)&SemaforoLabrinto,
				(OS_TICK		*) 0,
				OS_OPT_PEND_BLOCKING,
				(CPU_TS		*) 0,
				(OS_ERR        *)&err_os);
	if (LABIRINTO[pos][y-1] == 3){
		LABIRINTO[pos][y-1] = 0;
		for(i=0;i<NumEnemy;i++){
			if ((xEnemy[i] == pos) && (yEnemy[i] == (y-1))){
				OSSemPend((OS_SEM	*)&SemaforoEnemy,
						  (OS_TICK	*)0,
						  OS_OPT_PEND_BLOCKING,
						  (CPU_TS	*)0,
						  (OS_ERR	*)&err_os);

						Enemy[i] = 0;

				OSSemPost((OS_SEM	*)&SemaforoEnemy,
						   OS_OPT_POST_1,
						   (OS_ERR	*)&err_os);
			}
		}
		LABIRINTO[pos][y] = 0;
		OSSemPend((OS_SEM	*)&SemaforoEnemyCount,
				  (OS_TICK	*) 0,
				  OS_OPT_PEND_BLOCKING,
				  (CPU_TS	*)0,
				  (OS_ERR	*)&err_os);
		
		EnemyCount--;

		OSSemPost((OS_SEM	*)&SemaforoEnemyCount,
				  OS_OPT_POST_1,
				  (OS_ERR	*)&err_os);
	}
	else
	{
		LABIRINTO[pos][y-1] = 5;
		LABIRINTO[pos][y] = 0;
	}
	OSSemPost((OS_SEM		*)&SemaforoLabrinto,
			   OS_OPT_POST_1,
			   (OS_ERR		*)&err_os);
}
/*
*********************************************************************************************************
*                                           MoveEnemy()
*
* Description : Move o Inimigo.
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : TaskEnemy().
*
*********************************************************************************************************
*/
static void MoveEnemy(void *p_arg){
	int i = *((int *)p_arg);
	while((LABIRINTO[xEnemy[i]+1][yEnemy[i]+1] != 5) ){
		OS_ERR err_os;
		OSSemPend((OS_SEM		*)&SemaforoLabrinto,
					(OS_TICK		*) 0,
					OS_OPT_PEND_BLOCKING,
					(CPU_TS		*) 0,
					(OS_ERR        *)&err_os);
		LABIRINTO[xEnemy[i]][yEnemy[i]] = 0;
		yEnemy[i]++;
		OSSemPend((OS_SEM	*)&SemaforoEnemy,
				  (OS_TICK	*)0,
				  OS_OPT_PEND_BLOCKING,
				  (CPU_TS	*)0,
				  (OS_ERR	*)&err_os);

		LABIRINTO[xEnemy[i]][yEnemy[i]] = Enemy[i];

		OSSemPost((OS_SEM	*)&SemaforoEnemy,
				  OS_OPT_POST_1,
				  (OS_ERR	*)&err_os);
		OSSemPost((OS_SEM		*)&SemaforoLabrinto,
				   OS_OPT_POST_1,
				  (OS_ERR		*)&err_os);
			OSTimeDly(2000, OS_OPT_TIME_DLY, &err_os);

	}
}
/*
*********************************************************************************************************
*                                           TaskEnemy()
*
* Description : Inimigo.
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : App_TaskStart().
*
*********************************************************************************************************
*/
static void TaskEnemy(void *p_arg){
	int j;
	int k;
	OS_ERR err_os;
	int i = *((int *)p_arg);
	srand(OSTimeGet(&err_os));
	xEnemy[i] = (rand() % 35) + 1;
	yEnemy[i] = (rand() % EnemyLine) + 1;
	OSSemPend((OS_SEM		*)&SemaforoLabrinto,
				(OS_TICK		*) 0,
				OS_OPT_PEND_BLOCKING,
				(CPU_TS		*) 0,
				(OS_ERR        *)&err_os);

	LABIRINTO[xEnemy[i]][yEnemy[i]] = Enemy[i];

	OSSemPost((OS_SEM		*)&SemaforoLabrinto,
			   OS_OPT_POST_1,
			  (OS_ERR		*)&err_os);

	OSTaskCreate((OS_TCB     *)&TaskMoveEnemyTCB[i],                /* Cria a tarefa inicial.                             */
                 (CPU_CHAR   *)"Task Move Enemy",
                 (OS_TASK_PTR ) MoveEnemy,
                 (void       *) &i,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
				 (CPU_STK    *)&TaskMoveEnemyStk[i][0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err_os);

	OSTimeDly(20, OS_OPT_TIME_DLY, &err_os);

}
/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Exemplo de tarefa Inicial do sistema.
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : main().
*
*********************************************************************************************************
*/

static  void  App_TaskStart (void  *p_arg)
{
	int i=0;
	int j=0;
	int index[NumEnemy];
	int erroN;
	OS_ERR  err_os;

	erroN = GUI_Init(HandleGUIEvents);	// inicializa a interface grafica

	if( erroN < 0 ) { // se falhou para carregar a Gui, retorna.
		printf("\n Erro ao iniciar a Gui (%d)",erroN);
	}
	//Cria semaforo Tela
	OSSemCreate(&SemaforoTela,"Semaforo Tela",1,&err_os);
	//Desenhar itens na Tela
	OSTaskCreate((OS_TCB		*)&TaskTelaTcb,
				 (CPU_CHAR		*)"TaskTela",
				 (OS_TASK_PTR	 ) TaskTela,
				 (void			*) 0,
				 (OS_PRIO		 ) 10,
				 (CPU_STK		*)&TaskTelaStk[0],
				 (CPU_STK_SIZE   ) TSKTELA_STK_SIZE / 10u,
				 (CPU_STK_SIZE	 ) TSKTELA_STK_SIZE,
				 (OS_MEM_QTY	 ) 0u,
				 (OS_TICK		 ) 0u,
				 (CPU_TS		*) 0,
				 (OS_OPT		 ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR	    *)&err_os);

	//Cria Semaforo nave
	OSSemCreate(&SemaforoShipPos,"Semaforo Pos Nave",1,&err_os);
	//Cria Semaforo Labirinto
	OSSemCreate(&SemaforoLabrinto,"Semaforo Labirinto",1,&err_os);
	//Cria Semaforo Contador de inimigos
	OSSemCreate(&SemaforoEnemyCount,"Semaforo Contador Inimigo",1,&err_os);
	//Cria Semaforo Inimigos
	OSSemCreate(&SemaforoEnemy,"Semaforo Inimigo",1,&err_os);
	//Inicia Posição da Nave
	OSSemPend((OS_SEM		*)&SemaforoLabrinto,
			 (OS_TICK		*) 0,
			 OS_OPT_PEND_BLOCKING,
			 (CPU_TS		*) 0,
			 (OS_ERR        *)&err_os);
	LABIRINTO[ShipPos][LinhaNave] = 1;	
	OSSemPost((OS_SEM		*)&SemaforoLabrinto,
			  OS_OPT_POST_1,
			  (OS_ERR		*)&err_os);
	//Cria inimigos
	OSSemPend((OS_SEM	*)&SemaforoEnemyCount,
			(OS_TICK	*) 0,
			OS_OPT_PEND_BLOCKING,
			(CPU_TS	*)0,
			(OS_ERR	*)&err_os);
	j = 0;

	OSSemPost((OS_SEM	*)&SemaforoEnemyCount,
				OS_OPT_POST_1,
				(OS_ERR	*)&err_os);

	while(j<NumEnemy){
		index[j] = j;
		OSSemPend((OS_SEM	*)&SemaforoEnemy,
				  (OS_TICK	*)0,
				  OS_OPT_PEND_BLOCKING,
				  (CPU_TS	*)0,
				  (OS_ERR	*)&err_os);
		
		Enemy[j] = 3;

		OSSemPost((OS_SEM	*)&SemaforoEnemy,
				   OS_OPT_POST_1,
				   (OS_ERR	*)&err_os);
		OSTaskCreate((OS_TCB		*)&TaskEnemyTCB[j],
						(CPU_CHAR		*)"TaskEnemy" + j,
						(OS_TASK_PTR	 ) TaskEnemy,
						(void			*) &index[j],
						(OS_PRIO		 ) 10,
						(CPU_STK		*)&TaskEnemyStk[j][0],
						(CPU_STK_SIZE   ) TSKENEMY_STK_SIZE / 10u,
						(CPU_STK_SIZE	 ) TSKENEMY_STK_SIZE,
						(OS_MEM_QTY	 ) 0u,
						(OS_TICK		 ) 0u,
						(CPU_TS		*) 0,
						(OS_OPT		 ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
						(OS_ERR	    *)&err_os);
		OSTimeDly(20, OS_OPT_TIME_DLY, &err_os);
		j++;
		OSSemPend((OS_SEM	*)&SemaforoEnemyCount,
				(OS_TICK	*) 0,
				OS_OPT_PEND_BLOCKING,
				(CPU_TS	*)0,
				(OS_ERR	*)&err_os);

		EnemyCount++;

		OSSemPost((OS_SEM	*)&SemaforoEnemyCount,
					OS_OPT_POST_1,
					(OS_ERR	*)&err_os);
	}

	printf("\n Inicio do loop de msg");

    // Loop de mensagens para interface grafica
    while (1)
   		 {
			OSSemPend((OS_SEM		*)&SemaforoTela,
					  (OS_TICK		*) 0,
					  OS_OPT_PEND_BLOCKING,
					  (CPU_TS		*) 0,
					  (OS_ERR		*)&err_os);
			fundo = GUI_CreateImage( "fundo.bmp", 780, 556);
			GUI_DrawImage(fundo,0,0,780 ,556,5);
			OSSemPost((OS_SEM		*)&SemaforoTela,
					OS_OPT_POST_1,
					(OS_ERR		*)&err_os);
			 PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);

			//OSTimeDlyHMSM(0,0,0,40,OS_OPT_TIME_DLY, &err_os);
			OSTimeDly(20, OS_OPT_TIME_DLY, &err_os);

    }

	printf("\n fim do loop de msg");

    DeleteObject(fundo);
}


// Step 4: the Window Procedure
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;
	OS_ERR err_os;
	switch(msg)
    {
		case WM_KEYDOWN:
		  switch (wParam)
		  {
			case VK_HOME:
			  // Insert code here to process the HOME key
			  // ...
			  break;

			case VK_END:
			  // Insert code here to process the END key
			  // ...
			  break;

			case VK_INSERT:
			  // Insert code here to process the INS key
			  // ...
			  break;

			case VK_F2:
			  // Insert code here to process the F2 key
			  // ...
			  break;

			case VK_LEFT:
			  // Insert code here to process the LEFT ARROW key
			  // ...
				MoveShip(-1);
				imgXPos-=20;
			  break;

			case VK_RIGHT:
			  // Insert code here to process the RIGHT ARROW key
			  // ...
				MoveShip(1);
				imgXPos+=20;
			  break;

			case VK_UP:
			  // Insert code here to process the UP ARROW key
			  // ...
				OSSemPend((OS_SEM		*)&SemaforoShipPos,
						 (OS_TICK		*) 0,
						 OS_OPT_PEND_BLOCKING,
						 (CPU_TS		*) 0,
						 (OS_ERR        *)&err_os);
				Shot(ShipPos);
				OSSemPost((OS_SEM		*)&SemaforoShipPos,
						  OS_OPT_POST_1,
						  (OS_ERR		*)&err_os);
				imgYPos-=20;
			  break;

			case VK_DOWN:
			  // Insert code here to process the DOWN ARROW key
			  // ...
				imgYPos+=20;
			  break;

			case VK_DELETE:
			  // Insert code here to process the DELETE key
			  // ...
			  break;
        
			default:
			  // Insert code here to process other noncharacter keystrokes
			  // ...
			  break;
		  } 

		// Handles all Windows Messages 
		
        case WM_COMMAND:

            {
              if(((HWND)lParam) && (HIWORD(wParam) == BN_CLICKED))
              {
                int iMID;
                iMID = LOWORD(wParam);
                switch(iMID)
                {
                  case ID_BUTTON_1:
                      {
                       
                       break;
                       }
                  default:
                       break;
                }
              }
			  else if(HIWORD(wParam)==EN_CHANGE && LOWORD(wParam)==ID_EDIT_LINE_1) 
				{
					
				}
              break;
            }

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

		
		case WM_PAINT: 
			{
			

			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

		
        case WM_DESTROY:
            PostQuitMessage(0);
        break;

		case WM_LBUTTONDOWN: // The user has clicked the mouse button. Capture the mouse the window still receives mouse events when the cursor is moved out.
		//SetCapture( hWnd );
		//BeginMousing();
			return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	   case WM_MOUSEMOVE:
		   if( wParam & MK_LBUTTON )
		   {
			  // The user is moving the mouse while LMB is down. Do rotation/whatever.
			  //OnMousing();
		   }
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

	   case WM_LBUTTONUP:
	   //ReleaseCapture(); // User released mouse button, so no need to keep track of global mouse events.
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

	   case WM_CAPTURECHANGED: // The mouse capture window has changed. If the new capture window is not this window, then we need to stop rotation/whatever.
	   //if( (HWND)lParam != hWnd )
	   //{
		//   EndMousing();
	   //}
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;

}
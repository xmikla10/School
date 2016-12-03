/*
 * Soubor:  proj1.c
 * Datum:   22.04.2015
 * Autor:   Peter Miklánek, xmikla10@stud.fit.vutbr.cz
 * Projekt: IOS č. 2
 * Popis:   Implementujte v jazyce C modifikovaný synchronizační problém Building H20
 */
/* ******************** KNIŽNICE - ZAÈIATOK ******************** */
#include <math.h> /* matematicka kniznica */
#include <stdio.h> /* praca so vstupom/vystupom*/
#include <string.h> /* kvoli funkci strcmp*/
#include <stdlib.h> /* obecne funkcie jazyka C*/
#include <stdbool.h> /* kvoli bool*/
#include <ctype.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
/* ************ KNIŽNICE - KONIEC ************ */
#define E_OK 0
#define E_SEM 1
#define E_SHM 2
#define E_PARAM 3
/*unnamed semaphores -> man */
/* ************ Struktura pre parametry - ZACIATOK ************ */
 typedef struct Params
{
        int N_Kyslik;
        int GH_preVodik;
        int GO_preKyslik;
        int B_bond;

}Params;
/* ********** Struktura pre parametry - KONIEC ************ */
FILE *vystup = NULL;
sem_t *s_kyslik, *s_vodik, *s_H2o, *s_cekarna, *s_citac, *s_citac_z, *s_zapis_2, *s_started;
int *cislo_procesu = NULL, *pocitadlo_waiting_H = NULL, *cislo_bonded = NULL, *started_H = NULL, *pocitadlo_waiting_O = NULL, *pocet = 0;
int shm_procesu = 0, shm_H_finished = 0, shm_O_finished = 0, shm_begin_bonding = 0, shm_bonded = 0;
/* ******************* KONŠTANTY - ZAČIATOK ******************** */
const char *HELPMSG = "Program Projekt IOS 2\n"
                      "Autor: Peter Miklanek\n";
/* ******************** KONŠTANTY - KONIEC ******************** */
bool Spracovanie_Argumentov( int argc, char *argv[], Params *Z);
int nacitaj_zdroje(void);
void uvolni_zdroje(void);
int PROCESS(Params Z);
void bond(void);
/* ******************** MAIN - ZAČIATOK ******************** */
int main(int argc, char *argv[])
{
        Params Z;
        if (Spracovanie_Argumentov(argc, argv, &Z) == true )
        {
                printf("OK, program spravne implementovany\n");
        }
        else
        {
                return EXIT_FAILURE;
        }  
        PROCESS(Z);
}
/* ******************** MAIN - KONIEC ******************** */
/* ******************** PROCESS - ZACIATOK ******************** */
int PROCESS(Params Z)
{
        vystup = fopen("h2o.out", "w+" ); 
        int zdroje = nacitaj_zdroje();
        if(zdroje != E_OK) 
        {
                if(zdroje == E_SEM) 
                {
                        fprintf(stderr, "Nastala chyba pri vytvareni semaforu. Program bude ukoncen.\n");
                } 
                else  
                {
                        fprintf(stderr, "Nastala chyba pri alokaci pameti. Program bude ukoncen.\n");
                }
                uvolni_zdroje();
                return EXIT_FAILURE;
        }     

        for ( int a = 0 ; a < 2 ; a++ )
        {       
                pid_t Hlavny_Proces = fork();
                if ( Hlavny_Proces < 0 )
        {
                return EXIT_FAILURE;   
        }
        *cislo_procesu = 0 ;
        *pocitadlo_waiting_H = 0 ;
        *pocitadlo_waiting_O = 0 ;
        if ( Hlavny_Proces == 0 )
        {   
            if ( a == 0 )
            {
                    for ( int I = 1 ; I <= (Z.N_Kyslik*2) ; I++ )
                    {       
                            pid_t Proces_Vodik = fork();
                            if ( Proces_Vodik < 0 )  // proces sa nevytvoril
                            {
                                    return EXIT_FAILURE;
                            }
                            else if ( Proces_Vodik == 0 ) //proces je rodic, nechceme a ukoncime
                            {   
                                    //fflush(vystup);
                                    sem_wait(s_vodik);

                                    sem_wait(s_started);
                                    
                                    (*cislo_procesu)++;
                                    fprintf(vystup,"%d     : H %d    : started\n", (*cislo_procesu), I);    
                                    printf("vodik %d   %d\n",(*pocitadlo_waiting_H), (*pocitadlo_waiting_O) );  
                                    sem_post(s_started);
                                    fflush(vystup);
                                    if ( (*pocitadlo_waiting_H) >= 1 && (*pocitadlo_waiting_O) >=1 )
                                    {
                                             sem_wait(s_started);
                                            (*cislo_procesu)++;
                                            fprintf(vystup,"%d     : H %d    : ready\n", (*cislo_procesu), I);
                                            (*pocitadlo_waiting_H)--;
                                            (*pocitadlo_waiting_O)--;
                                            sem_post(s_started);
                                            fflush(vystup);
                                            //bond();
                                    }
                                    else
                                    {   
                                         sem_wait(s_started);
                                        (*cislo_procesu)++;
                                        (*pocitadlo_waiting_H)++;
                                        fprintf(vystup,"%d     : H %d    : waiting\n", (*cislo_procesu), I);
                                        
                                        sem_post(s_started);
                                        fflush(vystup);
                                    }      

                                    sem_post(s_vodik);
                                    exit(0);
                            }
                    } 
            }
            if ( a == 1 )
            {
                    for ( int I = 1 ; I <= (Z.N_Kyslik) ; I++ )
                    {    
                            pid_t Proces_Kyslik = fork();
                            if ( Proces_Kyslik < 0 )  // proces sa nevytvoril
                            {
                                    return EXIT_FAILURE;
                            }
                            else if ( Proces_Kyslik == 0 ) //proces je rodic, nechceme a ukoncime
                            {     
                                    //fflush(vystup);
                                    sem_wait(s_kyslik);

                                    sem_wait(s_started);
                                    (*cislo_procesu)++;
                                    
                                    fprintf(vystup,"%d     : O %d    : started\n", (*cislo_procesu), I); 
                                    printf("kyslik %d   %d\n",(*pocitadlo_waiting_H), (*pocitadlo_waiting_O) );     
                                    sem_post(s_started);
                                    fflush(vystup);
                                    if ( (*pocitadlo_waiting_H) >= 2 && (*pocitadlo_waiting_O) >=0 )
                                    {
                                            sem_wait(s_started);
                                            (*cislo_procesu)++;
                                            fprintf(vystup,"%d     : O %d    : ready\n", (*cislo_procesu), I);
                                            (*pocitadlo_waiting_H) = (*pocitadlo_waiting_H) - 2;
                                            
                                            sem_post(s_started);
                                            fflush(vystup);
                                    }
                                    else
                                    {
                                            sem_wait(s_started);
                                            (*cislo_procesu)++;
                                            (*pocitadlo_waiting_O)++;
                                            fprintf(vystup,"%d     : O %d    : waiting\n", (*cislo_procesu), I);
                                            
                                            sem_post(s_started);
                                            fflush(vystup);
                                    }    
                                    sem_post(s_kyslik);
                                    exit(0);                       
                             }

                    }
            }
            fclose(vystup);
            exit(0);
        } 
        }
    return 0;
}
/* ******************** PROCESS - KONIEC ******************** */
void bond(void)
{
        (*cislo_procesu)++;
        fprintf(vystup,"%d     : H %d    : begin bonding\n", (*cislo_procesu), 1);
        (*cislo_procesu)++;
        fprintf(vystup,"%d     : H %d    : begin bonding\n", (*cislo_procesu), 2);
        (*cislo_procesu)++;
        fprintf(vystup,"%d     : O %d    : begin bonding\n", (*cislo_procesu), 1);
        (*cislo_procesu)++;
        fprintf(vystup,"%d     : H %d    : bonded\n", (*cislo_procesu), 1);
        (*cislo_procesu)++;
        fprintf(vystup,"%d     : H %d    : bonded\n", (*cislo_procesu), 2);
        (*cislo_procesu)++;
        fprintf(vystup,"%d     : O %d    : bonded\n", (*cislo_procesu), 1);
}

/* ******************** Spracovanie - VSTUP - ZACIATOK ******************** */
bool Spracovanie_Argumentov( int argc, char *argv[], Params *Z)
{
        if ( argc != 5 )
        {
                fprintf(stderr, "CHYBA: Program nespavne implementovany\n");
                printf("%s\n", HELPMSG);
                return false;
        }
                
        else if ( argc == 5 )
        {
                if( isdigit(*argv[1]) )
                {
                        Z->N_Kyslik = strtol(argv[1], NULL, 10);
                }
                else
                {
                        fprintf(stderr, "CHYBA: Program nespavne implementovany\n");
                        return false;
                }
                if( isdigit(*argv[2]) )
                {
                        Z->GH_preVodik = strtol(argv[2], NULL, 10);
                }
                else
                {
                        fprintf(stderr, "CHYBA: Program nespavne implementovany\n");
                        return false;
                }
                if( isdigit(*argv[3]) )
                {
                        Z->GO_preKyslik = strtol(argv[3], NULL, 10);
                }
                else
                {
                        fprintf(stderr, "CHYBA: Program nespavne implementovany\n");
                        return false;
                }
                if( isdigit(*argv[4]) )
                {
                        Z->B_bond = strtol(argv[4], NULL, 10);
                }
                else
                {
                        fprintf(stderr, "CHYBA: Program nespavne implementovany\n");
                        return false;
                }
        }        
        if ( Z->N_Kyslik > 0 || ( Z->GH_preVodik >= 0 && Z->GH_preVodik < 5001 ) || ( Z->GO_preKyslik >= 0 && Z->GO_preKyslik < 5001 ) || ( Z->B_bond >= 0 && Z->B_bond < 5001 ) )
        {
                if ((vystup = fopen("h2o.out", "w+" )) == NULL ) 
                {
                        fprintf(stderr, "Nepodarilo sa otvorit vystupny subor. Program bude ukoncen.\n");
                        return EXIT_FAILURE;
                }
                else 
                {
                        vystup = stdout;
                }    


        } 
        if ( Z->N_Kyslik <= 0 || Z->GH_preVodik < 0 || Z->GH_preVodik >= 5001 || Z->GO_preKyslik < 0 || Z->GO_preKyslik >= 5001 || Z->B_bond < 0 || Z->B_bond >= 5001 )
        {
                fprintf(stderr, "CHYBA: Program nespavne implementovany\n");
                return false;
        }
        return true;            
}
/* ******************** Spracovanie - VSTUP - KONIEC ******************** */

int nacitaj_zdroje(void)
{
        int error = E_OK;
        // -------------------- inicializacia semaforov -----------------------
        if((s_kyslik = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_vodik = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_H2o = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_cekarna = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_citac = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_citac_z = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_started = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
        if((s_zapis_2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { error = E_SEM; }
   
        if(error == E_OK) 
        {   
                if(sem_init(s_kyslik, 1, 1) == -1) { error = E_SEM; }
                if(sem_init(s_vodik, 1, 1) == -1) { error = E_SEM; }
                if(sem_init(s_H2o, 1, 0) == -1) { error = E_SEM; }
                if(sem_init(s_cekarna, 1, 1) == -1) { error = E_SEM; }
                if(sem_init(s_citac, 1, 1) == -1) { error = E_SEM; }
                if(sem_init(s_citac_z, 1, 1) == -1) { error = E_SEM; }
                if(sem_init(s_started, 1, 1) == -1) { error = E_SEM; }
                if(sem_init(s_zapis_2, 1, 0) == -1) { error = E_SEM; }
        }

    if(error == E_OK) 
    {// -------------------- alokace pameti ------------------------------
            if((shm_H_finished = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { error = E_SHM; }
            if((started_H = (int *) shmat(shm_H_finished, NULL, 0)) == NULL) { error = E_SHM; }

            if((shm_O_finished = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { error = E_SHM; }
            if((pocitadlo_waiting_O = (int *) shmat(shm_O_finished, NULL, 0)) == NULL) { error = E_SHM; }

            if((shm_begin_bonding = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { error = E_SHM; }
            if((pocitadlo_waiting_H = (int *) shmat(shm_begin_bonding, NULL, 0)) == NULL) { error = E_SHM; }

            if((shm_bonded = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { error = E_SHM; }
            if((cislo_bonded = (int *) shmat(shm_bonded, NULL, 0)) == NULL) { error = E_SHM; }

            if((shm_procesu = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { error = E_SHM; }
            if((cislo_procesu = (int *) shmat(shm_procesu, NULL, 0)) == NULL) { error = E_SHM; }
    // -------------------- konec alokace pameti ------------------------
    }
    return error;
} 
void uvolni_zdroje(void)    
{
        fclose(vystup);
        int error = E_OK;
 
        if(sem_destroy(s_kyslik) == -1) { error = E_SEM; }
        if(sem_destroy(s_vodik) == -1) { error = E_SEM; }
        if(sem_destroy(s_H2o) == -1) { error = E_SEM; }
        if(sem_destroy(s_cekarna) == -1) { error = E_SEM; }
        if(sem_destroy(s_citac) == -1) { error = E_SEM; }
        if(sem_destroy(s_citac_z) == -1) { error = E_SEM; }
        if(sem_destroy(s_started) == -1) { error = E_SEM; }
        if(sem_destroy(s_zapis_2) == -1) { error = E_SEM; }
 
        if(error == E_SEM) 
        { 
                fprintf(stderr, "Nepovedlo se smazat semafor.\n"); 
        }
} 
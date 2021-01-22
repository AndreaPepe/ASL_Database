#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

#include "defines.h"

//signals handling
static volatile sig_atomic_t signo;
typedef struct sigaction sigaction_t;
static void handler(int s);

/* to acquire inputs from stdin */
char *getInput(unsigned int lung, char *stringa, bool hide){

  char c;
  unsigned int i;

  // variables needed to hide input
  sigaction_t sa, savealrm, saveint, savehup, savequit, saveterm;
  sigaction_t savetstp, savettin, savettou;
  struct termios term, oterm;

  if(hide){
    // svuota buffer
    (void) fflush(stdout);
    // Cattura i segnali che altrimenti potrebbero far terminare il programma, lasciando l'utente senza output sulla shell
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_INTERRUPT; // Per non resettare le system call
		sa.sa_handler = handler;
    (void) sigaction(SIGALRM, &sa, &savealrm);
		(void) sigaction(SIGINT, &sa, &saveint);
		(void) sigaction(SIGHUP, &sa, &savehup);
		(void) sigaction(SIGQUIT, &sa, &savequit);
		(void) sigaction(SIGTERM, &sa, &saveterm);
		(void) sigaction(SIGTSTP, &sa, &savetstp);
		(void) sigaction(SIGTTIN, &sa, &savettin);
		(void) sigaction(SIGTTOU, &sa, &savettou);

    // disattiva output su schermo
    if (tcgetattr(fileno(stdin), &oterm) == 0){
      (void) memcpy(&term, &oterm, sizeof(struct termios));
      term.c_lflag &= ~(ECHO|ECHONL);
      (void) tcsetattr(fileno(stdin), TCSAFLUSH, &term);
    }else{
      (void) memset(&term, 0, sizeof(struct termios));
      (void) memset(&oterm, 0, sizeof(struct termios));
    }
  }

  // legge al più lung-1 caratteri
  for(i=0; i<lung; i++){
    (void) fread(&c, sizeof(char), 1, stdin);
    if (c == '\n'){
      stringa[i] = '\0';
      break;
    }else
      stringa[i] = c;

    //gestione asterischi '*'
    if(hide){
      if(c == '\b') //backspace
        (void) write(fileno(stdout), &c, sizeof(char));
      else
        (void) write(fileno(stdout), "*", sizeof(char));
    }
  }

  // controllo terminatore di stringa '\0'
  if(i == lung -1)
    stringa[i] = '\0';

  // se sono stati digitati più di lung-1 caratteri, svuota buffer tastiera
  if(strlen(stringa) >= lung){
    do{
      c = getchar();
    }while (c != '\n');
  }

  if (hide){
    // '\n' dopo l'input
    (void) write (fileno(stdout), "\n", 1);

    // ripristina impostazioni precedenti schermo
    (void) tcsetattr(fileno(stdin), TCSAFLUSH, &oterm);

    // ripristina gestione segnali
    (void) sigaction(SIGALRM, &savealrm, NULL);
		(void) sigaction(SIGINT, &saveint, NULL);
		(void) sigaction(SIGHUP, &savehup, NULL);
		(void) sigaction(SIGQUIT, &savequit, NULL);
		(void) sigaction(SIGTERM, &saveterm, NULL);
		(void) sigaction(SIGTSTP, &savetstp, NULL);
		(void) sigaction(SIGTTIN, &savettin, NULL);
		(void) sigaction(SIGTTOU, &savettou, NULL);

    // se era stato ricevuto un segnale, viene rilanciato al processo stesso
    if (signo)
      (void) raise(signo);
  }

  return stringa;
}


// to handle signals
static void handler (int s){
  // salvo il codice del segnale nella variabile statica per gestirlo in seguito
  signo = s;
}



/* genera domanda con multi-opzione di risposta */
char multiChoice(char *domanda, char choices[], int num){
  //genera stringa delle possibilità
  char *possib = malloc(2 * num * sizeof(char));
  int i, j = 0;

  for(i = 0; i < num; i++){
    possib[j++] = choices[i];
    possib[j++] = '/';
  }
  possib[j-1] = '\0';   // per eliminare l'ultimo '/' inserito alla fine

  // chiede la risposta
  while(true){
    //mostra la domanda
    printf("%s [%s]: ", domanda, possib);

    char c;
    getInput(1, &c, false); //read 1 char

    // controlla se è un carattere valido
    for(i = 0; i < num; i++){
      if(c == choices[i]){
        free(possib);
        return c;
      }
    }
  } //else repeat while loop
}


// yes or no input function
bool yesOrNo(char *domanda, char yes, char no, bool predef, bool insensitive){

  // i caratteri 'yes' e 'no' devono essere lower_case
  yes = tolower(yes);
  no = tolower(no);

  // decide quale delle due lettere mostrare come predefinita
  char s, n;
  if (predef){
    s = toupper(yes);
    n = no;
  }else{
    s = yes;
    n = toupper(no);
  }

  // richiede la risposta
  while(true){
    // mostra la domanda
    printf("%s [%c/%c]: ", domanda, s, n);

    char c;
    getInput(1, &c ,false);

    // controlla quale risposta ha ricevuto
    if (c == '\0'){ // getInput() non può restituire '\n' !!
      return predef;
    } else if (c == yes){
        return true;
    } else if (c == no){
        return false;
    } else if(c == toupper(yes)) {
			  if(predef || insensitive)
          return true;
		} else if(c == toupper(yes)) {
			  if(!predef || insensitive)
          return false;
		}
  }
}

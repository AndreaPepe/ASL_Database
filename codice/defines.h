#include <stdbool.h>
#include <mysql.h>

struct configuration{
  char *host;
  char *db_username;
  char *db_password;
  unsigned int port;
  char *database;

  char username[128];
  char password[128];
};

extern struct configuration conf;

/* used to parse json files into a configuration struct */
extern int parse_config(char *path, struct configuration *conf);

/* to handle I/O interaction */
extern char *getInput (unsigned int lung, char *stringa, bool hide);
extern char multiChoice (char *domanda, char choices[], int num);
extern bool yesOrNo (char *domanda, char yes, char no, bool predef, bool insensitive);

/* connection error */
extern void print_error(MYSQL *conn, char *message);

/* statement error */
extern void print_stmt_error(MYSQL_STMT *stmt, char *message);

/* exit with error */
extern void finish_with_error(MYSQL *conn, char *message);

/* exit with stmt error */
extern void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt);

/* prepare statement for execution */
extern bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn);


/* run with different configurations */
extern void run_as_amministratore(MYSQL *conn);
extern void run_as_personaleCUP(MYSQL *conn);

/* prepare output tables to be printed well */
extern void dump_result_set(MYSQL *conn, MYSQL_STMT *stmt, char *title);

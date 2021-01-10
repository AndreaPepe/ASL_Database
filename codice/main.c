#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

static char *opt_host_name = NULL;
static char *opt_user_name = "root";
static char *opt_password = "05ago1999";
static unsigned int port_number = 0;
static char *opt_socket_name = NULL;
static char *opt_db_name = "ASL_db";
static unsigned int opt_flags = 0;

static MYSQL *conn;

int main(char* argc, char** argv){

  // initialize connection handler
  conn = mysql_init(NULL);
  if(conn == NULL){
    fprintf(stderr, "mysql_init() failed at line 17\n");
    exit(EXIT_FAILURE);
  }

  // real CONNECTION to database server
  if (mysql_real_connect(conn, opt_host_name, opt_user_name, opt_password,
    opt_db_name, port_number, opt_socket_name, opt_flags) == NULL){
      fprintf(stderr, "mysql_real_connect() failed at line 23\n");
      mysql_close(conn);
      exit(EXIT_FAILURE);
    }
  else{
    printf("Successful connection to database %s\n", opt_db_name);
  }

  // disconnecting from the server
  mysql_close(conn);
  printf("\nConnection closed with success\n");
  return 0;
}

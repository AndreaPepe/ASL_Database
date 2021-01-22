#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

/*this function is used to print errors verified during the execution of some stmt */
void print_stmt_error (MYSQL_STMT *stmt, char *message){

  fprintf(stderr, "%s\n", message);
  if (stmt != NULL){
    fprintf(stderr, "Error %u (%s): %s\n",
          mysql_stmt_errno(stmt),
          mysql_stmt_sqlstate(stmt),
          mysql_stmt_error(stmt));
  }
}

/* to print connection errors */
void print_error (MYSQL *conn, char *message){

  fprintf (stderr, "%s\n", message);
  if (conn!=NULL){
    #if MYSQL_VERSION_ID >=40101
    fprintf(stderr, "Error %u (%s): %s\n",
      mysql_errno(conn), mysql_sqlstate(conn), mysql_error(conn));
    #else
    fprintf(stderr, "Error %u: %s\n",
    mysql_errno(conn), mysql_error(conn));
    #endif
  }
}

/* prepare statement*/
bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn){

  bool update_length = true;

  *stmt = mysql_stmt_init(conn);
  if(*stmt == NULL){
    print_error(conn, "Could not init stmt handler");
    return false;
  }

  if (mysql_stmt_prepare (*stmt, statement, strlen(statement)) != 0){
    print_stmt_error(*stmt, "Could not prepare stmt");
    return false;
  }

  mysql_stmt_attr_set (*stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update_length);
  return true;
}


// exit with error
void finish_with_error(MYSQL *conn, char *message){

  print_error(conn, message);
  mysql_close(conn);
  exit(EXIT_FAILURE);
}


// exit with stmt error
void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt){

  print_stmt_error(stmt, message);
  if(close_stmt)
    mysql_stmt_close(stmt);
  mysql_close(conn);
  exit(EXIT_FAILURE);
}

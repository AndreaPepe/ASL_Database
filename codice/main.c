#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string.h>

#include "defines.h"

// protocol between client and DMBS
typedef enum {
  AMMINISTRATORE = 1,
  PERSONALECUP,
  FAILED_LOGIN
} role_t;

struct configuration conf;

static MYSQL *conn;


static role_t attempt_login(MYSQL *conn, char *username, char *password){
  MYSQL_STMT *login_procedure;

  MYSQL_BIND param[3];
  int role = 0;

  if (!setup_prepared_stmt(&login_procedure, "call login(?, ?, ?)", conn)){
    print_stmt_error(login_procedure, "Unable to init login stmt\n");
    goto err2;
  }

  // prepare params; clean memory to avoid unexpected errors!!!
  memset(param, 0, sizeof(param));

  param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
  param[0].buffer = username;
  param[0].buffer_length = strlen(username);

  param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
  param[1].buffer = password;
  param[1].buffer_length = strlen(password);

  param[2].buffer_type = MYSQL_TYPE_LONG; //out; mysql_long --> int (C_lang)
  param[2].buffer = &role;
  param[2].buffer_length = sizeof(role);

  // PARAM binding!
  if (mysql_stmt_bind_param(login_procedure, param) != 0){
    print_stmt_error (login_procedure, "Could not bind parameters for login procedure");
    goto err1;
  }

  // Run procedure
  if(mysql_stmt_execute(login_procedure) != 0){
    print_stmt_error(login_procedure, "Could not execute login procedure");
    goto err1;
  }

  // prepare output params
  memset(param, 0, sizeof(param));
  param[0].buffer_type = MYSQL_TYPE_LONG;
  param[0].buffer = &role;
  param[0].buffer_length = sizeof(role);

  // RESULT binding!
  if (mysql_stmt_bind_result(login_procedure, param)){
    print_stmt_error(login_procedure, "Could not retrieve output parameters");
    goto err1;
  }

  // retrieve output params
  if (mysql_stmt_fetch(login_procedure)){
    print_stmt_error(login_procedure, "Could not buffer results");
    goto err1;
  }

  // close stmt and return result
  mysql_stmt_close(login_procedure);
  return role;

  err1:
    mysql_stmt_close(login_procedure);
  err2:
    return FAILED_LOGIN;
}


int main(void){

  role_t role;

  if (!parse_config("Users/login.json", &conf)){
    fprintf(stderr, "Unable to load login configuration\n");
    exit(EXIT_FAILURE);
  }

  // initialize connection handler
  conn = mysql_init(NULL);
  if(conn == NULL){
    fprintf(stderr, "mysql_init() failed\n");
    exit(EXIT_FAILURE);
  }

  // real CONNECTION to database server
  if (mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password,
  conf.database, conf.port, NULL,
  CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL){
      fprintf(stderr, "mysql_real_connect() failed\n");
      mysql_close(conn);
      exit(EXIT_FAILURE);
    }
  else{
    printf("Successful connection to database %s\n\n", conf.database);
  }

/*  if (mysql_real_connect(conn, "localhost", "login", "login",
  "ASL_db", 3306, NULL,
  CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL){
      fprintf(stderr, "mysql_real_connect() failed: Error: %s\n", mysql_error(conn));
      mysql_close(conn);
      exit(EXIT_FAILURE);
    }
  else{
    printf("Successful connection to database %s\n\n", conf.database);
  }*/

  printf("Username: ");
  getInput(128, conf.username, false);
  printf("Password: ");
  getInput(128, conf.password, true); //read with hide option enabled

  role = attempt_login(conn, conf.username, conf.password);

  switch(role){
    case AMMINISTRATORE:
      run_as_amministratore(conn);
      break;

    case PERSONALECUP:
      run_as_personaleCUP(conn);
      break;

    case FAILED_LOGIN:
      fprintf(stderr, "Invalid credentials\n");
      mysql_close(conn);
      exit(EXIT_FAILURE);
      break;

    default:
      fprintf(stderr, "Invalid condition at %s: %d\n", __FILE__, __LINE__);
      abort();
  }

  // disconnecting from the server
  printf ("Bye\n");
  mysql_close(conn);
  return 0;
}

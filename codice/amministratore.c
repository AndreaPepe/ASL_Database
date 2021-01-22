#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"


static void registration(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[3];
	char local_options[2] = {'1', '2'}; // possible roles
	char r;

	// input
	char username[46];
	char password[46];
	char var_ruolo[46];

	// get required info
	printf("\nUsername: ");
	getInput(46, username, false);
	printf("Password: ");
	getInput(46, password, true);

	printf("Assign a role. Possible options are:\n");
	printf("\t1) Amministratore\n");
	printf("\t2) PersonaleCUP\n");

	r = multiChoice("Select role", local_options, 2);
	// convert role into enum value
	switch(r){
		case '1':
			strcpy(var_ruolo, "amministratore");
			break;

		case '2':
			strcpy(var_ruolo, "personaleCUP");
			break;

		default:
			fprintf(stderr, "Invalid condition at %s: %d\n", __FILE__, __LINE__);
			abort();
	}

	// prepare stmt
	if(!setup_prepared_stmt(&prep_stmt, "call registration(?, ?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init registration stmt\n", false);
	}

	// prepare input params
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = var_ruolo;
	param[2].buffer_length = strlen(var_ruolo);

	// bind params
	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for registration procedure\n", true);
	}

	// run procedure
	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred during registration");
	} else{
		printf("The new user has been created successfully\n");
	}

	// closing stmt
	mysql_stmt_close(prep_stmt);
}

void run_as_amministratore(MYSQL *conn){

	char options[2] = {'1', '2'};
	char op;

	printf ("Switching to administrative role ...\n");

	if(!parse_config("Users/amministratore.json", &conf)){
		fprintf(stderr, "Unable to load 'amministratore' configuration\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)){
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	// showing multiChoice after cleaning screen
	while(true){	// while quit option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** What do you want to do? ***\n\n");
		printf("1) Create new user\n");
		printf("2) Quit\n");

		op = multiChoice("Select an option", options, 2);
		switch(op){
			case '1':
				registration(conn);
				break;

			case '2':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar(); //cleaning '\n'
	}
}

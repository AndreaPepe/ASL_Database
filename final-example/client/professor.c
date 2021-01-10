#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"


static void register_exam(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[5];

	// Input for the registration routine
	char matricola[46];
	int matricola_int;
	char esame[46];
	char corso_laurea[46];
	char voto[5];
	int voto_int;

	if(!setup_prepared_stmt(&prepared_stmt, "call lista_esami_docente(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize exam list statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for exam list\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve exam list\n", true);
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nList of exams assigned to you");
	mysql_stmt_close(prepared_stmt);

	// Get the required information
	printf("\nStudent ID: ");
	getInput(46, matricola, false);
	printf("Exam: ");
	getInput(46, esame, false);
	printf("Degree course: ");
	getInput(46, corso_laurea, false);
	printf("Mark: ");
	getInput(5, voto, false);

	// Apply proper type conversions
	matricola_int = atoi(matricola);
	voto_int = atoi(voto);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call verbalizza_esame(?, ?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize exam list statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &matricola_int;
	param[0].buffer_length = sizeof(matricola_int);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = esame;
	param[1].buffer_length = strlen(esame);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = corso_laurea;
	param[2].buffer_length = strlen(corso_laurea);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = conf.username;
	param[3].buffer_length = strlen(conf.username);

	param[4].buffer_type = MYSQL_TYPE_LONG;
	param[4].buffer = &voto_int;
	param[4].buffer_length = sizeof(voto_int);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for exam list\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while registering the exam.");
	} else {
		printf("Registration correctly completed...\n");
	}

	mysql_stmt_close(prepared_stmt);
}


void run_as_professor(MYSQL *conn)
{
	char options[2] = {'1','2'};
	char op;
	
	printf("Switching to professor role...\n");

	if(!parse_config("users/docente.json", &conf)) {
		fprintf(stderr, "Unable to load professor configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Register exam\n");
		printf("2) Quit\n");

		op = multiChoice("Select an option", options, 2);

		switch(op) {
			case '1':
				register_exam(conn);
				break;
				
			case '2':
				return;
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}

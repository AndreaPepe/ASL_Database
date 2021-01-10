#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

static void add_exam(MYSQL *conn)
{
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[3];

	// Input for the registration routine
	char esame[46];
	char corso_laurea[46];
	char docente[46];

	// Get the required information
	printf("\nExam name: ");
	getInput(46, esame, false);
	printf("Degree course: ");
	getInput(46, corso_laurea, false);
	printf("Professor name: ");
	getInput(46, docente, false);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_esame(?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize exam insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = esame;
	param[0].buffer_length = strlen(esame);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = corso_laurea;
	param[1].buffer_length = strlen(corso_laurea);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = docente;
	param[2].buffer_length = strlen(docente);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for exam insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the exam.");
	} else {
		printf("Exam correctly added...\n");
	}

	mysql_stmt_close(prepared_stmt);
}

static void add_student(MYSQL *conn)
{
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[4];

	// Input for the registration routine
	char name[46];
	char surname[46];
	char username[46];
	int matricola;

	// Get the required information
	printf("\nStudent name: ");
	getInput(46, name, false);
	printf("Student surname: ");
	getInput(46, surname, false);
	printf("Student username: ");
	getInput(46, username, false);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_studente(?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize student insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = name;
	param[0].buffer_length = strlen(name);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = surname;
	param[1].buffer_length = strlen(surname);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = username;
	param[2].buffer_length = strlen(username);

	param[3].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[3].buffer = &matricola;
	param[3].buffer_length = sizeof(matricola);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for student insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while adding the student.");
		goto out;
	}

	// Get back the ID of the newly-added student
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[0].buffer = &matricola;
	param[0].buffer_length = sizeof(matricola);
	
	if(mysql_stmt_bind_result(prepared_stmt, param)) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
	}
	
	// Retrieve output parameter
	if(mysql_stmt_fetch(prepared_stmt)) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
	}

	printf("Student correctly added with ID %d...\n", matricola);

    out:
	mysql_stmt_close(prepared_stmt);
}

static void create_user(MYSQL *conn)
{
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[3];
	char options[5] = {'1','2', '3'};
	char r;

	// Input for the registration routine
	char username[46];
	char password[46];
	char ruolo[46];

	// Get the required information
	printf("\nUsername: ");
	getInput(46, username, false);
	printf("password: ");
	getInput(46, password, true);
	printf("Assign a possible role:\n");
	printf("\t1) Studente\n");
	printf("\t2) Docente\n");
	printf("\t3) Amministratore\n");
	r = multiChoice("Select role", options, 3);

	// Convert role into enum value
	switch(r) {
		case '1':
			strcpy(ruolo, "studente");
			break;
		case '2':
			strcpy(ruolo, "docente");
			break;
		case '3':
			strcpy(ruolo, "amminstratore");
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call crea_utente(?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize user insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = ruolo;
	param[2].buffer_length = strlen(ruolo);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for user insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the user.");
	} else {
		printf("User correctly added...\n");
	}

	mysql_stmt_close(prepared_stmt);
}

static void subscribe_to_degree(MYSQL *conn)
{
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for the registration routine
	int matricola_int;
	char matricola[46];
	char corso[46];

	// Get the required information
	printf("\nStudent ID: ");
	getInput(46, matricola, false);
	printf("Degree course: ");
	getInput(46, corso, false);

	// Convert values
	matricola_int = atoi(matricola);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call iscrivi_studente(?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize student registration statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &matricola_int;
	param[0].buffer_length = sizeof(matricola_int);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = corso;
	param[1].buffer_length = strlen(corso);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for student registration\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while registering the student.");
	} else {
		printf("Student correctly registered...\n");
	}

	mysql_stmt_close(prepared_stmt);
}

void run_as_administrator(MYSQL *conn)
{
	char options[5] = {'1','2', '3', '4', '5'};
	char op;
	
	printf("Switching to administrative role...\n");

	if(!parse_config("users/amministratore.json", &conf)) {
		fprintf(stderr, "Unable to load administrator configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Add new exam\n");
		printf("2) Add new student\n");
		printf("3) Create new user\n");
		printf("4) Subscribe student to degree course\n");
		printf("5) Quit\n");

		op = multiChoice("Select an option", options, 5);

		switch(op) {
			case '1':
				add_exam(conn);
				break;
			case '2':
				add_student(conn);
				break;
			case '3':
				create_user(conn);
				break;
			case '4':
				subscribe_to_degree(conn);
				break;
			case '5':
				return;
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#define FLOAT_SIZE 10

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


static void print_dashes(MYSQL_RES *res_set)
{
	MYSQL_FIELD *field;
	unsigned int i, j, num;

	mysql_field_seek(res_set, 0);
	putchar('+');
	for (i = 0; i < mysql_num_fields(res_set); i++) {
		field = mysql_fetch_field(res_set);
    if (field->type == MYSQL_TYPE_FLOAT){
      num = FLOAT_SIZE;
    }else{
      num = field->max_length + 2;
    }
		for (j = 0; j < num; j++)
			putchar('-');
		putchar('+');
	}
	putchar('\n');
}



static void dump_result_set_header(MYSQL_RES *res_set)
{
	MYSQL_FIELD *field;
	unsigned long col_len;
	unsigned int i;

	/* determine column display widths -- requires result set to be */
	/* generated with mysql_store_result(), not mysql_use_result() */

	mysql_field_seek (res_set, 0);

	for (i = 0; i < mysql_num_fields (res_set); i++) {
		field = mysql_fetch_field (res_set);
		col_len = strlen(field->name);

		if (col_len < field->max_length)
			col_len = field->max_length;
		if (col_len < 4 && !IS_NOT_NULL(field->flags))
			col_len = 4; /* 4 = length of the word "NULL" */
		field->max_length = col_len; /* reset column info */
	}

	print_dashes(res_set);
	putchar('|');
	mysql_field_seek (res_set, 0);
	for (i = 0; i < mysql_num_fields(res_set); i++) {
		field = mysql_fetch_field(res_set);
    if (field->type == MYSQL_TYPE_FLOAT){
      printf(" %-*s |", FLOAT_SIZE - 2, field->name);
    }else{
		printf(" %-*s |", (int)field->max_length, field->name);
    }
	}
	putchar('\n');

	print_dashes(res_set);
}


void dump_result_set(MYSQL *conn, MYSQL_STMT *stmt, char *title)
{
	int i;
	int status;
	int num_fields;       /* number of columns in result */
	MYSQL_FIELD *fields;  /* for result set metadata */
	MYSQL_BIND *rs_bind;  /* for output buffers */
	MYSQL_RES *rs_metadata;
	MYSQL_TIME *date;
	size_t attr_size;


	/* Prefetch the whole result set. This in conjunction with
	 * STMT_ATTR_UPDATE_MAX_LENGTH set in `setup_prepared_stmt`
	 * updates the result set metadata which are fetched in this
	 * function, to allow to compute the actual max length of
	 * the columns.
	 */
	if (mysql_stmt_store_result(stmt)) {
		fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(0);
	}


  bool is_null[mysql_stmt_num_rows(stmt)];      // added to print NULL values !!!!

	/* the column count is > 0 if there is a result set */
	/* 0 if the result is only the final status packet */
	num_fields = mysql_stmt_field_count(stmt);

	if (num_fields > 0) {
		/* there is a result set to fetch */
		printf("%s\n", title);

		if((rs_metadata = mysql_stmt_result_metadata(stmt)) == NULL) {
			finish_with_stmt_error(conn, stmt, "Unable to retrieve result metadata\n", true);
		}

		dump_result_set_header(rs_metadata);

		fields = mysql_fetch_fields(rs_metadata);

		rs_bind = (MYSQL_BIND *)malloc(sizeof (MYSQL_BIND) * num_fields);
		if (!rs_bind) {
			finish_with_stmt_error(conn, stmt, "Cannot allocate output buffers\n", true);
		}
		memset(rs_bind, 0, sizeof (MYSQL_BIND) * num_fields);

		/* set up and bind result set output buffers */
		for (i = 0; i < num_fields; ++i) {

			// Properly size the parameter buffer
			switch(fields[i].type) {
				case MYSQL_TYPE_DATE:
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_DATETIME:
				case MYSQL_TYPE_TIME:
					attr_size = sizeof(MYSQL_TIME);
					break;
				case MYSQL_TYPE_FLOAT:
					attr_size = sizeof(float);
					break;
				case MYSQL_TYPE_DOUBLE:
					attr_size = sizeof(double);
					break;
				case MYSQL_TYPE_TINY:
					attr_size = sizeof(signed char);
					break;
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_YEAR:
					attr_size = sizeof(short int);
					break;
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_INT24:
					attr_size = sizeof(int);
					break;
				case MYSQL_TYPE_LONGLONG:
					attr_size = sizeof(int);
					break;
				default:
					attr_size = fields[i].max_length;
					break;
			}

			// Setup the binding for the current parameter
			rs_bind[i].buffer_type = fields[i].type;
			rs_bind[i].buffer = malloc(attr_size + 1);
			rs_bind[i].buffer_length = attr_size + 1;
      rs_bind[i].is_null = &(is_null[i]);

			if(rs_bind[i].buffer == NULL) {
				finish_with_stmt_error(conn, stmt, "Cannot allocate output buffers\n", true);
			}
		}

		if(mysql_stmt_bind_result(stmt, rs_bind)) {
			finish_with_stmt_error(conn, stmt, "Unable to bind output parameters\n", true);
		}

		/* fetch and display result set rows */
		while (true) {
			status = mysql_stmt_fetch(stmt);

			if (status == 1 || status == MYSQL_NO_DATA)
				break;

			putchar('|');

			for (i = 0; i < num_fields; i++) {

				if (is_null[i]){//rs_bind[i].is_null_value) {
					printf (" %-*s |", (int)fields[i].max_length, "NULL");
					continue;
				}

				switch (rs_bind[i].buffer_type) {

					case MYSQL_TYPE_VAR_STRING:
					case MYSQL_TYPE_DATETIME:
						printf(" %-*s |", (int)fields[i].max_length, (char*)rs_bind[i].buffer);
						break;

          case MYSQL_TYPE_TIME:         //added
            date = (MYSQL_TIME *)rs_bind[i].buffer;
          //printf(" %02d:%-*.02d |", date->hour, (int)fields[i].max_length - 3, date->minute);
            char time[6];
            sprintf(time, "%02d:%02d", date->hour, date->minute);
            printf(" %-*s |", (int)fields[i].max_length, time);
            break;

					case MYSQL_TYPE_DATE:
					case MYSQL_TYPE_TIMESTAMP:
						date = (MYSQL_TIME *)rs_bind[i].buffer;
						printf(" %02d-%02d-%04d |", date->day, date->month, date->year);  // modified to [DD-MM-YYYY] format
						break;

					case MYSQL_TYPE_STRING:
						printf(" %-*s |", (int)fields[i].max_length, (char *)rs_bind[i].buffer);
						break;

					case MYSQL_TYPE_FLOAT:
					case MYSQL_TYPE_DOUBLE:
						printf(" %-*.02f |", FLOAT_SIZE -2, *(float *)rs_bind[i].buffer);   //modified (float: max 10 digits)
						break;

					case MYSQL_TYPE_LONG:
					case MYSQL_TYPE_SHORT:
            printf(" %-*d |", (int)fields[i].max_length, *(int*)rs_bind[i].buffer); //modified from int* to signed char*
            break;
					case MYSQL_TYPE_TINY:
						printf(" %-*d |", (int)fields[i].max_length, *(signed char *)rs_bind[i].buffer); //modified from int* to signed char*
            break;

					case MYSQL_TYPE_NEWDECIMAL:
						printf(" %-*.02lf |", (int)fields[i].max_length, *(float*) rs_bind[i].buffer);
						break;

					default:
					    printf("ERROR: Unhandled type (%d)\n", rs_bind[i].buffer_type);
					    abort();
				}
			}
			putchar('\n');
			print_dashes(rs_metadata);
		}

		mysql_free_result(rs_metadata); /* free metadata */

		/* free output buffers */
		for (i = 0; i < num_fields; i++) {
			free(rs_bind[i].buffer);
		}
		free(rs_bind);
	}
}

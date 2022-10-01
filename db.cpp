#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
#include <mysql/my_global.h>
#include <mysql/mysql.h>

int finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    if (con != NULL) mysql_close(con);
    return -1;
}

int finish_with_correct(MYSQL *con)
{
    fprintf(stdout, "-------------------------------\n");
    fprintf(stdout, "mysql client version: %s\n", mysql_get_client_info());
    fprintf(stdout, "%s\n", mysql_get_server_info(con));
    fprintf(stdout, "%s\n", mysql_get_host_info(con));
    fprintf(stdout, "-------------------------------\n\n");
    mysql_close(con);
    return 0;
}

int create_database()
{
    MYSQL *con = mysql_init(NULL);
    if (NULL == con)
        return finish_with_error(con);

    if (NULL == mysql_real_connect(con, "localhost", "bjqe", "456789", NULL, 0, NULL, 0))
        return finish_with_error(con);

    if (mysql_query(con, "CREATE DATABASE IF NOT EXISTS mr_z_db character set utf8"))
        return finish_with_error(con);

    return finish_with_correct(con);
}

int test_query()
{
    MYSQL *con = mysql_init(NULL);
    if (NULL == con)
        return finish_with_error(con);

    if (NULL == mysql_real_connect(con, "localhost", "bjqe", "456789", "mr_z_db", 0, NULL, 0))
        return finish_with_error(con);

    char query[1024] = "CREATE TABLE IF NOT EXISTS person(\
                 id int primary key auto_increment, \
                 name varchar(20) not null, \
                 password varchar(20) not null, \
                 create_date timestamp)";
    if (mysql_query(con, query))
        return finish_with_error(con);

    sprintf(query, "select count(*) from person");
    if (mysql_query(con, query))
        return finish_with_error(con);

    if (mysql_field_count(con) <= 0)
        return finish_with_error(con);

    MYSQL_RES *res;
    MYSQL_ROW row;
    int num_fields;
    int num_datarow;

    if (!(res = mysql_store_result(con)))
        return finish_with_error(con);
    num_fields = mysql_num_fields(res);
    while (row = mysql_fetch_row(res))
        for (int i = 0; i < num_fields; ++i)
            num_datarow = atoi(row[i]);

    sprintf(query, "INSERT INTO person(id, name, password) \
            values(%d,%s,%s)", ++num_datarow, "'hunhun'", "'hunhun_pw123'");
    if (mysql_query(con, query))
       return finish_with_error(con);

    return finish_with_correct(con);
}

int drop_database()
{
    MYSQL *con = mysql_init(NULL);
    if (NULL == con)
        return finish_with_error(con);

    if (NULL == mysql_real_connect(con, "localhost", "bjqe", "456789", NULL, 0, NULL, 0))
        return finish_with_error(con);

    if (mysql_query(con, "DROP DATABASE IF EXISTS mr_z_db"))
        return finish_with_error(con);

    return finish_with_correct(con);
}
*/
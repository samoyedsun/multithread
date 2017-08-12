#include <stdlib.h>
#include <string.h>

#include <mysql/my_global.h>
#include <mysql/mysql.h>

#include <iostream>

int finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    if (con != NULL) mysql_close(con);
    return -1;
}

int finish_with_correct(MYSQL *con)
{
    std::cout << "mysql client version: " << mysql_get_client_info() << std::endl;
    std::cout << mysql_get_server_info(con) << std::endl;
    std::cout << mysql_get_host_info(con) << std::endl;
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

    if (mysql_query(con, "CREATE TABLE IF NOT EXISTS person(name TEXT, password TEXT)"))
        return finish_with_error(con);

    if (mysql_query(con, "INSERT INTO person values(\"hunhun\", \"hunhun_pw\")"))
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

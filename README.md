# Команды

```make``` - компилирует **src/server.c** и **src/client.c**

```make clean``` - удаляет исполняемые файлы **server** и **client**

```make runclient``` - запуск клиента

- Можно также передавать аргументы, по дефолту HOST=127.0.0.1 и PORT=9999. Пример ```make runserver HOST=192.168.0.1 PORT=1889```

```make runserver``` - запуск сервера

- Можно также передавать аргумент, по дефолту PORT=9999. Пример ```make runserver PORT=1889```



# operating-systems-project
gcc -o app2 main.c -Wall \`pkg-config --cflags --libs gtk+-3.0\` -export-dynamic

gcc -o server server.c -lpthread \`mysql_config --cflags --libs\`

### MYSQL
https://technomanor.wordpress.com/2012/07/01/connect-c-with-mysql-database/

sudo apt-get install -y libjson-c-dev
А компилить просто с линкингом -l json-c

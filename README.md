# operating-systems-project
gcc -o app2 main.c -Wall \`pkg-config --cflags --libs gtk+-3.0\` -export-dynamic

gcc -o server server.c -lpthread \`mysql_config --cflags --libs\`

### MYSQL
https://technomanor.wordpress.com/2012/07/01/connect-c-with-mysql-database/

sudo apt-get install -y libjson-c-dev
А компилить просто с линкингом -l json-c

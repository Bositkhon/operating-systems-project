# operating-systems-project
gcc -o app2 main.c -Wall \`pkg-config --cflags --libs gtk+-3.0\` -export-dynamic

gcc -o server server.c -lpthread

*This project has been created as part of the 42 curriculum by nseon & lcesbron*

# Webserv

## Description
Webserv is a lightweight and robust HTTP/1.1 server written entirely in C++98 from scratch. The primary goal of this project is to understand the underlying mechanics of the World Wide Web by recreating a server architecture inspired by NGINX. 

It handles multiple concurrent client connections through non-blocking I/O multiplexing. The server is capable of parsing raw HTTP requests, serving static files, managing custom routing via a configuration file, and executing dynamic scripts (like Python or PHP) through a Common Gateway Interface (CGI).

## Instructions

### Compilation
To compile the server, run the standard Makefile command at the root of the repository: 
```
make
```

Removes object files:
```
make clean
```

Removes objects and the executable:
```
make fclean
```

Recompiles the entire project.
```
make re
```
### Execution & Configuration Example
To start the server, you must provide a valid configuration file as an argument:
```
./webserv serv.conf
```
Concrete Example:
If you create a configuration file named tester.conf at the root of the project with this structure:
```
server {
    listen 8080;
    server_name localhost;
    host 127.0.0.1;
    client_max_body_size 10M;

    location / {
        limit_except GET POST;
        root /var/www/html;
        index index.html;
        autoindex on;
    }
}
```

Once running, you can test it directly in your browser at http://localhost:8080 or via curl in another terminal:
$ curl -i http://localhost:8080

## Ressources

>AI was used to improve understanding of certains concept and resolve some issues

* Beej's Guide to Network Programming: https://beej.us/guide/bgnet/
* NGINX Documentation: https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/
* RFC HTTP: https://www.rfc-editor.org/rfc/rfc9110.html
* RFC CGI: https://datatracker.ietf.org/doc/html/rfc3875

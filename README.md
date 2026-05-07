*This project has been created as part of the 42 curriculum by sgavrilo, and sgaspari*

# webserv

<img src="assets/webserv-banner.png" alt="webserv project banner" width="100%">

## Description

HTTP web server built from scratch in C++98.

It reads its configuration from a configuration that resembles Nginx's configuration files.

It handles multiple connections using `epoll`.

`epoll` is a Linux-only system call, which means that it won't work on other operating systems. For this reason the program is containerized with Docker.

## Configuration file

The server requires a configuration file with extension `.conf` in order to run.

Its structure is inspired by **nginx** configuration file and defines the directives, which can be of two types:

- **block directive**: consists of a name and set of instructions surrounded my curly braces:

```conf
<block_directive_name> {
  <instructions>
}
```
- **simple directive**: consists of a name and a value separated by one space and followed by a semicolon:

```conf
<simple_directive_name> <value>;
```

### Block directives

- `global`

  It represent the top-level context. It is implicit, which means that the configuration file would not have `global {}`.

- `server`

  Can be defined only in `global`.

  At least one `server` directive must be defined.

- `location`

  Can be defined only in `server`. Between the name and the opening curly brace, an endpoint must be specified:

  ```conf
  location / {
  }
  ```

  In this example, `/` is the endpoint of the `location` block directive.

In order to be valid, the closing curly brace must be in a different line than the opening curly brace.

This is invalid:

```conf
server {}
```

This is valid:

```conf
server {
}
```

### Simple directives

- `root`

  It determines the directory from which static files will be served. 

  It can be defined at any scope. 

  If not defined, it is set to `www`.

  If multiple definition are present in the same block directive, the last one is applied.

- `index`

  It determines the default file to serve.

  It can be defined at any scope.

  If not defined, it is set to `index.html`.

  If multiple definition are present in the same block directive, the last one is applied.

- `listen`

  It determines a port that a server listens from.

  It can be defined only inside a `server` block.

  If not defined, the program will exit with `[ERROR] No port specified`.

  Multiple definitions are allowed in the same block directive.

### Simple web server

The configuration file of a simple web server would like this:

```conf
server {
  listen 1024;

  location / {
  }
}
```

This will start the server with a single `server` block listening at port `1024`. `root` is set to `www` and `index` to `index.html`.

## How to run

Clone the repository:

```bash
git clone https://github.com/s-gas/webserv
```

Change to the project directory:

```bash
cd webserv
```

Create the executable via the `Makefile`:

```bash
make
```

### Through binary file

Run the program passing as argument the configuration file:

```bash
./webserv <configuration_file>
```

### Through Docker

Create the Docker image:

```bash
docker build -t webserv:1 .
```

Run the container:

```bash
docker run --rm -d -p 8080:8080 --name webserv webserv:1
```

This will run the program with `conf-files/webserv.conf` as argument.

### Flags used

| Flag   | Description                                        |
|--------|----------------------------------------------------|
| --rm   | Automatically remove the container when it exits   |
| -d     | Run container in background and print container ID |
| -p     | Publish a container's port(s) to the host          |
| --name | Assign a name to the container                     |

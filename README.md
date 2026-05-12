*This project has been created as part of the 42 curriculum by sgavrilo, and sgaspari*

# webserv

<img src="assets/webserv-banner.png" alt="webserv project banner" width="100%">

## Table of contents

- [Description](#description)
- [Configuration file](#configuration-file)
- [How to run](#how-to-run)

## Description

HTTP web server built from scratch in C++98.

It handles multiple connections using `epoll`.

`epoll` is a Linux-only system call, which means that it won't work on other operating systems. For this reason the program is containerized with Docker.

## Configuration file

The server requires a configuration file with extension `.conf` in order to run.

Its structure is inspired by **nginx** configuration file and defines the directives, which are instructions that tell the server how to handle incoming requests.

There are two types of directives:

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
  location /about/ {
  }
  ```

  In this example, `/about/` is the endpoint of the `location` block directive.

  In order to be valid, the location must include a trailing `/`:

  - `location /about`  -> Invalid
  - `location /about/` -> Valid
  
  **Fallback location**: if `location /` is defined, it acts as fallback, which means that if a request endpoint does not match any other `location` block, the server will default to the `location /` block.

  ```conf
  location /about/ {
  }

  location / {
  }
  ```

  With the above configuration:
  - `http://<hostname>/home`   -> `location /` block
  - `http://<hostname>/about/` -> `location /about/` block

In order to be valid, the closing curly brace of a block directive must be in a different line than the opening curly brace.

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

- `cgi_script`

  It defines the script that will handle the request.

  It can be defined only inside a `location` block.

### Simple web server

The configuration file of a simple web server would look like this:

```conf
server {
  listen 1024;

  location / {
  }
}
```

This will start the server with a single `server` block listening at port `1024`. `root` is set to `www` and `index` to `index.html`. The fallback `location /` allows the server to handle requests with any `endpoint`, which will be appended to `root`.

Some examples:

| Request                          | File served           |
|----------------------------------|-----------------------|
| `http://localhost:1024`          | `www/index.html`      |
| `http://localhost:1024/about/`   | `www/about/index.html`|
| `http://localhost:1024/file.txt` | `www/file.txt`        |


## HTTP request endpoints

HTTP request endpoints are normalized by appending `/` if the endpoint does not contain any dot (`.`).

For this reason, endpoints with directory names that contain a dot (`.`) in their name must also include the trailing `/`.

The following HTTP request would not be interpreted correctly:

```bash
http://<ip>:<port>/directory.name
```

The endpoint would be interpreted as a file name and, if not found, the server will return `404`.

To solve this, simply add a trailing `/`:

```bash
http://<ip>:<port>/directory.name/
```

## How to run

Clone the repository:

```bash
git clone https://github.com/s-gas/webserv
```

Change to the project directory:

```bash
cd webserv
```

### Through binary file

Create the executable via the `Makefile`:

```bash
make
```

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
docker run --rm -d -p <host_port>:<container_port> --name webserv webserv:1
```

This will run the program with `conf-files/webserv.conf` as argument.

### Flags used

| Flag   | Description                                        |
|--------|----------------------------------------------------|
| --rm   | Automatically remove the container when it exits   |
| -d     | Run container in background and print container ID |
| -p     | Publish a container's port(s) to the host          |
| --name | Assign a name to the container                     |

[Back to top](#webserv)

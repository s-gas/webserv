*This project has been created as part of the 42 curriculum by sgavrilo, and sgaspari*

# webserv

HTTP web server built from scratch in C++98.

It reads its configuration from a configuration that resembles Nginx's configuration files.

It handles multiple connections using `epoll`.

`epoll` is a Linux-only system call, which means that it won't work on other operating systems. For this reason the program is containerized with Docker.

## Todo list

- parse HTTP request
- serve static files
- support CGI

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

Create the Docker image:

```bash
docker build -t webserv:1 .
```

Run the container:

```bash
docker run --rm -d -p 8080:8080 --name webserv webserv:1
```

### Flags used

| Flag   | Description                                        |
|--------|----------------------------------------------------|
| --rm   | Automatically remove the container when it exits   |
| -d     | Run container in background and print container ID |
| -p     | Publish a container's port(s) to the host          |
| --name | Assign a name to the container                     |

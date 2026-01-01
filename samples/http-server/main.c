#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pool_day.h"
#include "task.h"

#define MAX_VERB_SIZE          7
#define MAX_BUFFER_SIZE        4096

#define DEFAULT_ROOT_DIR       "./www"
#define DEFAULT_PORT           8080
#define DEFAULT_MAX_CLIENTS    100

#define STR(x) #x
#define MAKE_ERROR_BODY(status_code, msg) \
  "<html><body><h1>" STR(status_code) " " #msg "</h1></body></html>"

typedef struct {
  uint32_t max_clients;
  uint16_t port;
  char root_dir[PATH_MAX];
} server_cfg_t;

typedef struct {
  char verb[MAX_VERB_SIZE];
  char resource[PATH_MAX];
} request_t;

typedef struct {
  uint32_t fd;
  struct in_addr addr;
} client_t;

// TODO:
// - select
// - graceful server exit
// - const and restrict

static void on_client_connected(uint32_t tid, const void *param) {
  const client_t *cli = (client_t *)param;

  printf("[+] task[%u]: client connected, ip=%s\n", tid, inet_ntoa(cli->addr));
}

static void on_client_disconnected(uint32_t tid, const void *param,
                                   void *ret_val) {
  const client_t *cli = (client_t *)param;

  printf("[+] task[%u]: client disconnected ip=%s, result=%u\n", tid,
         inet_ntoa(cli->addr), *((uint16_t *)ret_val));
  free(ret_val);
}

static void parse_request(const char *restrict buffer,
                          request_t *restrict req) {
  // simple parsing logic for demonstration purposes
  sscanf(buffer, "%s %s", req->verb, req->resource);
}

static void assemble_reply(char *buffer, size_t buffer_size, int status_code,
                           const char *status_str, const char *body) {
  const char *reply_fmt =
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: text/html\r\n"
    "\r\n%s";

  snprintf(buffer, buffer_size, reply_fmt, status_code, status_str, body);
}

static char *get_resource(const char *res_name) {
  struct stat st;

  FILE *file = fopen(res_name, "r");
  if (!file) {
    printf("[-] file to open the requested resource: %s\n", strerror(errno));
    return NULL;
  }

  stat(res_name, &st);

  char *content = calloc(1, st.st_size + 1);
  if (!content) {
    printf(
      "[-] no memory available to put the requested resource's content on\n");
    fclose(file);
    return NULL;
  }

  if (!fread(content, 1, st.st_size, file)) {
    printf("[-] file to read the requested resource\n");
    free(content);
    fclose(file);
    return NULL;
  }

  fclose(file);

  return content;
}

static int handle_get_request(char *reply_buffer, size_t buffer_size,
                              const char *resource) {
  char *res = get_resource(resource);
  int status_code;

  if (res) {
    status_code = 200;
    assemble_reply(reply_buffer, buffer_size, status_code, "OK", res);
    free(res);
  } else {
    status_code = 404;
    assemble_reply(reply_buffer, buffer_size, status_code, "Not Found",
                   MAKE_ERROR_BODY(404, Not Found));
  }

  return status_code;
}

static void *handle_client(void *param) {
  char buffer[MAX_BUFFER_SIZE] = {0};
  client_t *cli = (client_t *)param;
  int *ret = calloc(1, sizeof(int));
  request_t req;

  memset(&req, 0, sizeof(request_t));

  ssize_t received = recv(cli->fd, buffer, sizeof(buffer) - 1, 0);
  if (received > 0) {
    parse_request(buffer, &req);

    printf("[+] received request: %s %s\n", req.verb, req.resource);

    memset(buffer, 0, sizeof(buffer));
    if (!strcmp(req.verb, "GET")) {
      *ret = handle_get_request(buffer, MAX_BUFFER_SIZE, &req.resource[1]);

      send(cli->fd, buffer, strlen(buffer), 0);
    }
  }

  close(cli->fd);

  return (void *)ret;
}

static void parse_args(int argc, char **argv, server_cfg_t *restrict cfg) {
  int opt;

  while ((opt = getopt(argc, argv, "m:p:r:")) != -1) {
    switch (opt) {
    case 'm':
      cfg->max_clients = (uint32_t)atoi(optarg);
      break;
    case 'p':
      cfg->port = (uint16_t)atoi(optarg);
      break;
    case 'r':
      memcpy(cfg->root_dir, optarg, strlen(optarg) + 1);
      break;
    }
  }

  if (!cfg->max_clients) {
    cfg->max_clients = DEFAULT_MAX_CLIENTS;
  }

  if (!cfg->port) {
    cfg->port = DEFAULT_PORT;
  }

  if (cfg->root_dir[0] == '\0') {
    memcpy(cfg->root_dir, DEFAULT_ROOT_DIR, strlen(DEFAULT_ROOT_DIR) + 1);
  }
}

static int setup_socket(int *restrict sock_fd, const server_cfg_t *cfg) {
  struct sockaddr_in addr;

  *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (*sock_fd < 0) {
    printf("[-] fail to create the server socket: %s\n", strerror(errno));
    return 1;
  }

  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(cfg->port);

  if (bind(*sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("[-] fail to create the server socket: %s\n", strerror(errno));
    close(*sock_fd);
    return 1;
  }

  if (listen(*sock_fd, cfg->max_clients) < 0) {
    printf("[-] fail to create the server socket: %s\n", strerror(errno));
    close(*sock_fd);
    return 1;
  }

  return 0;
}

static int run_server(const server_cfg_t *restrict cfg) {
  int server_fd;
  pool_day_t pool;
  struct sockaddr_in cli_addr;
  socklen_t cli_len = sizeof(cli_addr);

  if (setup_socket(&server_fd, cfg) != 0) {
    printf("[-] failed to setup server socket\n");
    return 1;
  }

  if (!(pool = create_pool(cfg->max_clients))) {
    printf("[-] fail to setup the server pool\n");
    close(server_fd);
    return 1;
  }

  printf("[+] starting server with max_clients=%u, port=%u, root_dir=%s\n",
         cfg->max_clients, cfg->port, cfg->root_dir);

  if (chdir(cfg->root_dir)) {
    printf("[-] fail to run the server: %s\n", strerror(errno));
    return 1;
  }

  while (1) {
    int client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len);

    if (client_fd == -1) {
      printf("[-] failed to accept the incoming client: %s\n", strerror(errno));
      continue;
    }

    client_t *cli_ptr = calloc(1, sizeof(client_t));

    cli_ptr->fd = (uint32_t)client_fd;
    memcpy(&cli_ptr->addr, &cli_addr.sin_addr, sizeof(struct in_addr));

    task_t task = create_async_task(client_fd, handle_client, (void *)cli_ptr,
                                    sizeof(client_t), true, on_client_connected,
                                    on_client_disconnected);

    if (enqueue_task(pool, task) != POOL_DAY_SUCCESS) {
      printf("[-] failed to enqueue the request task\n");
      destroy_task(task);
      close(client_fd);
      free(cli_ptr);
    }
  }

  close(server_fd);
  destroy_pool(&pool);

  return 0;
}

int main(int argc, char **argv) {
  server_cfg_t cfg;

  memset(&cfg, 0, sizeof(server_cfg_t));
  parse_args(argc, argv, &cfg);
  return run_server(&cfg);
}

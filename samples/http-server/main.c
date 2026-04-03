#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "pool_day.h"
#include "task.h"

#define MAX_METHOD_SIZE        7
#define MAX_BUFFER_SIZE        10 * 1024

#define DEFAULT_ROOT_DIR       "./www"
#define DEFAULT_PORT           8080
#define DEFAULT_MAX_CLIENTS    100

#define CONTENT_TYPE_TEXT_HTML  "text/html"
#define CONTENT_TYPE_IMAGE_PNG  "image/png"
#define CONTENT_TYPE_IMAGE_JPEG "image/jpeg"
#define CONTENT_TYPE_IMAGE_GIF  "image/gif"
#define CONTENT_TYPE_TEXT_CSS   "text/css"
#define CONTENT_TYPE_APP_JS     "application/javascript"
#define CONTENT_TYPE_APP_OCTET  "application/octet-stream"

#define MAKE_ERROR_BODY(code, msg) \
  "<html><body><h1>" #code " " #msg "</h1></body></html>"

typedef struct {
  uint32_t max_clients;
  uint16_t port;
  char root_dir[PATH_MAX];
} server_cfg_t;

typedef struct {
  char method[MAX_METHOD_SIZE];
  char resource[PATH_MAX];
} request_t;

typedef struct {
  uint32_t fd;
  struct in_addr addr;
} client_t;

static void sig_handler(int signum) {
  (void)signum;
}

static void on_client_connected(uint32_t tid, const void *param) {
  const client_t *cli = (client_t *)param;

  fprintf(stdout, "[+] task[%u]: client connected, ip=%s\n", tid,
          inet_ntoa(cli->addr));
}

static void on_client_disconnected(uint32_t tid, const void *param,
                                   void *ret_val) {
  const client_t *cli = (client_t *)param;
  int ret = ret_val ? *((uint16_t *)ret_val) : -1;

  fprintf(stdout, "[+] task[%u]: client disconnected ip=%s, result=%u\n", tid,
          inet_ntoa(cli->addr), ret);

  if (ret_val) {
    free(ret_val);
  }
}

static const char *get_content_type(const char *path) {
  const char *ext = strrchr(path, '.');

  if (!ext) {
    return CONTENT_TYPE_APP_OCTET;
  }

  if (!strcmp(ext, ".html") || !strcmp(ext, ".htm")) {
    return CONTENT_TYPE_TEXT_HTML;
  }

  if (!strcmp(ext, ".png")) {
    return CONTENT_TYPE_IMAGE_PNG;
  }

  if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg")) {
    return CONTENT_TYPE_IMAGE_JPEG;
  }

  if (!strcmp(ext, ".gif")) {
    return CONTENT_TYPE_IMAGE_GIF;
  }

  if (!strcmp(ext, ".css")) {
    return CONTENT_TYPE_TEXT_CSS;
  }

  if (!strcmp(ext, ".js")) {
    return CONTENT_TYPE_APP_JS;
  }

  return CONTENT_TYPE_APP_OCTET;
}

static void parse_request(const char *buffer, request_t *req) {
  sscanf(buffer, "%s %s", req->method, req->resource);
}

static char *build_http_header(int status_code, const char *status_str,
                               const char *content_type, size_t content_length,
                               size_t *header_len) {

  const char *fmt =
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n"
    "\r\n";

  *header_len = snprintf(NULL, 0, fmt, status_code, status_str, content_type,
                         content_length);

  char *header = malloc(*header_len + 1);
  if (!header) {
    return NULL;
  }

  snprintf(header, *header_len + 1, fmt, status_code, status_str, content_type,
           content_length);

  return header;
}

static int assemble_reply(char **buffer, const char *header, size_t header_len,
                          const char *body, size_t body_len) {
  size_t total = header_len + body_len;

  *buffer = malloc(total);
  if (!(*buffer)) {
    return 0;
  }

  memcpy(*buffer, header, header_len);
  memcpy(*buffer + header_len, body, body_len);

  return total;
}

static char *get_resource(const char *path, size_t *len) {
  struct stat st;

  FILE *file = fopen(path, "rb");
  if (!file) {
    return NULL;
  }

  if (stat(path, &st) < 0) {
    fclose(file);
    return NULL;
  }

  char *data = malloc(st.st_size);
  if (!data) {
    fclose(file);
    return NULL;
  }

  size_t read_bytes = fread(data, 1, st.st_size, file);
  fclose(file);

  if (read_bytes != (size_t)st.st_size) {
    free(data);
    return NULL;
  }

  *len = read_bytes;
  return data;
}

static int handle_get_request(char **reply_buffer, size_t *reply_buffer_size,
                              const char *resource) {
  size_t header_len;

  if (strstr(resource, "..")) {
    return 400;
  }

  size_t res_len;
  char *res = get_resource(resource, &res_len);

  if (res) {
    const char *content_type = get_content_type(resource);
    char *header = build_http_header(200, "OK", content_type, res_len,
                                     &header_len);

    *reply_buffer_size = assemble_reply(reply_buffer, header, header_len, res,
                                        res_len);

    free(header);
    free(res);

    return 200;
  }

  const char *body = MAKE_ERROR_BODY(404, Not Found);
  size_t body_len = strlen(body);
  char *header = build_http_header(404, "Not Found", CONTENT_TYPE_TEXT_HTML,
                                   body_len, &header_len);

  *reply_buffer_size = assemble_reply(reply_buffer, header, header_len, body,
                                      body_len);

  free(header);

  return 404;
}

static int handle_request(const char *req_buffer,
                          char **reply_buffer,
                          size_t *reply_buffer_len) {

  request_t req;

  memset(&req, 0, sizeof(request_t));

  parse_request(req_buffer, &req);
  if (strcmp(req.method, "GET") != 0) {
    return 501;
  }

  fprintf(stdout, "[+] %s %s\n", req.method, req.resource);

  const char *resource = req.resource[0] == '/' ?
                         req.resource + 1 :
                         req.resource;

  return handle_get_request(reply_buffer, reply_buffer_len, resource);
}

static void send_all(int fd, const char *buffer, size_t len) {
  size_t total = 0;

  while (total < len) {
    ssize_t sent = send(fd, buffer + total, len - total, 0);

    if (sent <= 0) {
      break;
    }

    total += sent;
  }
}

static void *handle_new_connection(void *param) {
  client_t *cli = (client_t *)param;
  int *ret = NULL;

  char *req_buffer = calloc(1, MAX_BUFFER_SIZE);
  if (!req_buffer) {
    close(cli->fd);
    return NULL;
  }

  ssize_t received = recv(cli->fd, req_buffer, MAX_BUFFER_SIZE - 1, 0);

  if (received > 0) {
    char *reply_buffer = NULL;
    size_t reply_len = 0;

    ret = calloc(1, sizeof(int));
    *ret = handle_request(req_buffer, &reply_buffer, &reply_len);

    if (reply_buffer && reply_len > 0) {
      send_all(cli->fd, reply_buffer, reply_len);
      free(reply_buffer);
    }
  }

  free(req_buffer);
  close(cli->fd);

  return (void *)ret;
}

static void fill_cfg(int argc, char **argv, server_cfg_t *cfg) {
  int opt;

  while ((opt = getopt(argc, argv, "m:p:r:")) != -1) {
    switch (opt) {
    case 'm':
      cfg->max_clients = atoi(optarg);
      break;
    case 'p':
      cfg->port = atoi(optarg);
      break;
    case 'r':
      strcpy(cfg->root_dir, optarg);
      break;
    }
  }

  if (!cfg->max_clients) {
    cfg->max_clients = DEFAULT_MAX_CLIENTS;
  }

  if (!cfg->port) {
    cfg->port = DEFAULT_PORT;
  }

  if (!cfg->root_dir[0]) {
    strcpy(cfg->root_dir, DEFAULT_ROOT_DIR);
  }
}

static int setup_socket(int *sock_fd, const server_cfg_t *cfg) {
  struct sockaddr_in addr;

  *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (*sock_fd < 0) {
    return 1;
  }

  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(cfg->port);

  if (bind(*sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(*sock_fd);
    return 1;
  }

  if (listen(*sock_fd, cfg->max_clients) < 0) {
    close(*sock_fd);
    return 1;
  }

  return 0;
}

static int setup_server(int *sock_fd, const server_cfg_t *cfg,
                        pool_day_t *pool) {
  if (setup_socket(sock_fd, cfg) != 0) {
    return 1;
  }

  if (!(*pool = create_pool(cfg->max_clients))) {
    close(*sock_fd);
    return 1;
  }

  return 0;
}

static int server_mainloop(int server_fd, pool_day_t pool) {
  fd_set set;

  while (1) {
    FD_ZERO(&set);
    FD_SET(server_fd, &set);

    if (select(server_fd + 1, &set, NULL, NULL, NULL) < 0) {
      break;
    }

    if (FD_ISSET(server_fd, &set)) {
      struct sockaddr_in cli_addr;
      socklen_t len = sizeof(cli_addr);

      int client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &len);
      if (client_fd < 0) {
        continue;
      }

      task_t task = create_async_task(client_fd, handle_new_connection,
                                      (void *)&((client_t) {
                                        .fd = (uint32_t)client_fd,
                                        .addr = cli_addr.sin_addr }),
                                      sizeof(client_t), true,
                                      on_client_connected,
                                      on_client_disconnected);

      enqueue_task(pool, task);
    }
  }

  close(server_fd);
  destroy_pool(&pool);

  return 0;
}

static int run_server(const server_cfg_t *cfg) {
  int server_fd;
  pool_day_t pool;

  if (setup_server(&server_fd, cfg, &pool)) {
    fprintf(stderr, "[-] fail to setup the server\n");
    return 1;
  }

  if (chdir(cfg->root_dir)) {
    fprintf(stderr, "[-] fail to run the server on '%s': %s\n", cfg->root_dir,
            strerror(errno));
    return 1;
  }

  fprintf(stdout,
          "[+] starting server with max_clients=%u, port=%u, root_dir=%s\n",
          cfg->max_clients, cfg->port, cfg->root_dir);

  return server_mainloop(server_fd, pool);
}

int main(int argc, char **argv) {
  server_cfg_t cfg;

  signal(SIGINT, sig_handler);

  memset(&cfg, 0, sizeof(server_cfg_t));
  fill_cfg(argc, argv, &cfg);

  return run_server(&cfg);
}

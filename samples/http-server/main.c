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

#define STR(x) #x
#define MAKE_ERROR_BODY(status_code, msg) \
  "<html><body><h1>" STR(status_code) " " #msg "</h1></body></html>"

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

typedef enum {
  SUCCESS,
  NOT_FOUND,
  INTERNAL_ERROR
} http_status_id_t;

typedef struct {
  http_status_id_t id;
  int code;
  char *msg;
} http_status_t;

//static http_status_t status_table[] = {
//  { SUCCESS, 200, "OK" },
//  { NOT_FOUND, 404, "Not Found" },
//  { INTERNAL_ERROR, 505, "Internal Error" }
//};

// TODO: lookup table for http errors
// TODO: 501 error?

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

static void parse_request(const char *buffer, request_t *req) {
  // simple parsing logic for demonstration purposes
  sscanf(buffer, "%s %s", req->method, req->resource);
}

static char *build_http_header(int status_code, const char *status_str,
                               size_t *header_len) {
  const char *crlf = "\r\n";
  const char *version = "HTTP/1.1";
  const char *content_type = "Content-Type: text/html"; // TODO: and how about images?
  const char *header_fmt =
    "%s %d %s\r\n"
    "%s\r\n"
    "\r\n";

  //const char *reply_fmt =
  //  "HTTP/1.1 %d %s\r\n"
  //  "Content-Type: text/html\r\n"
  //  "\r\n%s";

  *header_len = strlen(version) + sizeof(int) + strlen(status_str) + 2
    + 3 * strlen(crlf) + strlen(content_type);

  char *header = calloc(1, *header_len);
  if (!header) {
    return NULL;
  }

  snprintf(header, *header_len, header_fmt, version, status_code, status_str,
           content_type);

  return header;
}

static int assemble_reply(char **buffer, char *header, size_t header_len,
                          const char *body, size_t body_len) {
  size_t buffer_len = header_len + body_len + 1;

  *buffer = calloc(1, buffer_len);
  if (!(*buffer)) {
    return 0;
  }

  memcpy(*buffer, header, header_len);
  memcpy(*buffer + header_len - 1, body, body_len);

  return buffer_len;
}

static char *get_resource(const char *res_name, size_t *resource_len) {
  struct stat st;

  FILE *file = fopen(res_name, "r"); // TODO: and how about the image files?
  if (!file) {
    fprintf(stderr, "[-] fail to open the requested resource: %s\n",
            strerror(errno));
    return NULL;
  }

  stat(res_name, &st);

  char *content = calloc(1, st.st_size + 1);
  if (!content) {
    fprintf(stderr, "[-] fail to allocate memory for the requested resource\n");
    fclose(file);
    return NULL;
  }

  *resource_len = fread(content, 1, st.st_size, file);
  if (!(*resource_len)) {
    fprintf(stderr, "[-] fail to read the requested resource\n");
    free(content);
    fclose(file);
    return NULL;
  }

  fclose(file);

  return content;
}

static int handle_get_request(char **reply_buffer, size_t *reply_buffer_size,
                              const char *resource) {
  int status_code;
  size_t res_len;
  char *header;
  size_t header_len;
  char *res = get_resource(resource, &res_len);

  if (res) {
    status_code = 200;
    header = build_http_header(status_code, "OK", &header_len);
    *reply_buffer_size = assemble_reply(reply_buffer, header, header_len, res,
                                        res_len);
    free(res);
  } else {
    status_code = 404;
    header = build_http_header(status_code, "Not Found", &header_len);
    char *body = MAKE_ERROR_BODY(404, Not Found);
    *reply_buffer_size = assemble_reply(reply_buffer, header, header_len,
                                        body, strlen(body));
  }

  free(header);

  return status_code;
}

static int handle_request(const char *req_buffer, char **reply_buffer,
                          size_t *reply_buffer_len) {
  request_t req;

  memset(&req, 0, sizeof(request_t));

  parse_request(req_buffer, &req);

  fprintf(stdout, "[+] received request: %s %s\n", req.method, req.resource);
  if (strcmp(req.method, "GET")) {
    fprintf(stderr, "[-] unsupported request\n");
    // TODO: assemble 501 reply
    return 501;
  }

  return handle_get_request(reply_buffer, reply_buffer_len, &req.resource[1]);
}

static void *handle_new_connection(void *param) {
  client_t *cli = (client_t *)param;
  int *ret = NULL;
  char *req_buffer;
  char *reply_buffer;
  size_t reply_buffer_len;

  req_buffer = calloc(1, MAX_BUFFER_SIZE);
  if (!req_buffer) {
    fprintf(stderr, "[-] fail to allocate memory for request buffer\n");
    close(cli->fd);
    return NULL;
  }

  ssize_t received = recv(cli->fd, req_buffer, MAX_BUFFER_SIZE - 1, 0);
  if (received > 0) {
    ret = calloc(1, sizeof(int));
    if (ret) {
      *ret = handle_request(req_buffer, &reply_buffer, &reply_buffer_len);

      printf("reply_len = %lu\n", reply_buffer_len);
      printf("reply = %s\n", reply_buffer);

      //for (size_t i = 0; i < reply_buffer_len; i++) {
      //  printf("%c\n", reply_buffer[i]);
      //}

      if (reply_buffer_len) {
        send(cli->fd, reply_buffer, reply_buffer_len - 2, 0); // FIXME: tem coisa amais sendo copiado: GDB!!!!!
        free(reply_buffer);
      }
    }
  }

  close(cli->fd);
  free(req_buffer);

  return (void *)ret;
}

static void fill_cfg(int argc, char **argv, server_cfg_t *cfg) {
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

static int setup_server(const server_cfg_t *cfg, int *server_fd,
                        pool_day_t *pool) {
  if (setup_socket(server_fd, cfg) != 0) {
    fprintf(stderr, "[-] fail to setup the server socket: %s\n",
            strerror(errno));
    return 1;
  }

  if (!(*pool = create_pool(cfg->max_clients))) {
    fprintf(stderr, "[-] fail to setup the server pool\n");
    close(*server_fd);
    return 1;
  }

  return 0;
}

static int server_mainloop(int server_fd, pool_day_t pool) {
  int ret;
  fd_set set;
  struct sockaddr_in cli_addr;
  socklen_t cli_len = sizeof(cli_addr);

  while (1) {
    FD_ZERO(&set);
    FD_SET(server_fd, &set);

    ret = select(server_fd + 1, &set, NULL, NULL, NULL);
    if ((ret == -1) && (errno == EINTR)) {
      fprintf(stdout, "[+] exiting server...\n");
      break;
    }

    if (FD_ISSET(server_fd, &set)) {
      int client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len);

      if (client_fd == -1) {
        fprintf(stderr, "[-] fail to accept the incoming client: %s\n",
                strerror(errno));
        continue;
      }

      task_t task = create_async_task(client_fd, handle_new_connection,
                                      (void *)&((client_t) {
                                        .fd = (uint32_t)client_fd,
                                        .addr = cli_addr.sin_addr }),
                                      sizeof(client_t), true,
                                      on_client_connected,
                                      on_client_disconnected);

      if (enqueue_task(pool, task) != POOL_DAY_SUCCESS) {
        fprintf(stderr, "[-] fail to enqueue the request task\n");
        destroy_task(task);
        close(client_fd);
      }
    }
  }

  close(server_fd);
  destroy_pool(&pool);

  return EXIT_SUCCESS;
}

static int run_server(const server_cfg_t *cfg) {
  int server_fd;
  pool_day_t pool;

  if (setup_server(cfg, &server_fd, &pool)) {
    fprintf(stderr, "[-] fail to setup the server\n");
    return EXIT_FAILURE;
  }

  if (chdir(cfg->root_dir)) {
    fprintf(stderr, "[-] fail to run the server: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  fprintf(stdout,
          "[+] starting server with max_clients=%u, port=%u, root_dir=%s\n",
          cfg->max_clients, cfg->port, cfg->root_dir);

  return server_mainloop(server_fd, pool);
}

int main(int argc, char **argv) {
  server_cfg_t cfg;

  memset(&cfg, 0, sizeof(server_cfg_t));

  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  fill_cfg(argc, argv, &cfg);
  int ret = run_server(&cfg);

  exit(ret);
}

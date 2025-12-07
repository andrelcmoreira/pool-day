#include <assert.h>
#include <sys/stat.h>
#include <getopt.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "pool_day.h"

#define HANDLE_REQUEST_TASK_ID 0
#define MAX_VERB_SIZE          7
#define MAX_BUFFER_SIZE        4096

#define DEFAULT_ROOT_DIR       "./www"
#define DEFAULT_PORT           8080
#define DEFAULT_MAX_CLIENTS    10

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

static server_cfg_t cfg;

// TODO: what parameters should be useful here?
void task_start_callback(uint32_t tid) {
  printf("[+] task '%u' starting...\n", tid);
}

void task_end_callback(uint32_t tid, void *ret_val) {
  printf("[+] task '%u' ended with return value: %s\n", tid, (char *)ret_val);
}

void parse_request(const char *request_buffer, request_t *req) {
  // Simple parsing logic for demonstration purposes
  sscanf(request_buffer, "%s %s", req->verb, req->resource);
}

void assemble_reply(char *buffer, int status_code, const char *status_str,
                    const char *body) {
  const char *reply_fmt =
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "%s\r\n";

  sprintf(buffer, reply_fmt, status_code, status_str, body);
}

char *get_resource(const char *res) {
  char resource[PATH_MAX * 2] = {0}; // FIXME
  struct stat st;

  snprintf(resource, sizeof(resource), "%s/%s", cfg.root_dir, res);

  FILE *file = fopen(resource, "r");
  if (!file) {
    return NULL;
  }

  stat(resource, &st);

  char *content = calloc(1, st.st_size + 1);
  if (!content) {
    fclose(file);
    return NULL;
  }

  fread(content, 1, st.st_size, file);
  fclose(file);

  return content;
}

void *handle_client(void *param) {
  char buffer[MAX_BUFFER_SIZE] = {0};
  int client_fd = *(int *)(param);
  request_t req;

  memset(&req, 0, sizeof(request_t));

  ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (received > 0) {
    parse_request(buffer, &req);

    printf("[+] received request: %s %s\n", req.verb, req.resource);

    if (!strcmp(req.verb, "GET")) {
      char *res = get_resource(req.resource);

      memset(buffer, 0, sizeof(buffer));
      if (res) {
        assemble_reply(buffer, 200, "OK", res);
        free(res);
      } else {
        assemble_reply(buffer, 404, "Not Found",
                       MAKE_ERROR_BODY(404, Not Found));
      }

      send(client_fd, buffer, strlen(buffer), 0);
    }
  }

  close(client_fd);
  free(param);

  return NULL;
}

void parse_args(int argc, char **argv) {
  int opt;

  // Set default values
  cfg.max_clients = DEFAULT_MAX_CLIENTS;
  cfg.port = DEFAULT_PORT;
  memcpy(cfg.root_dir, DEFAULT_ROOT_DIR, strlen(DEFAULT_ROOT_DIR) + 1);

  while ((opt = getopt(argc, argv, "m:p:r:")) != -1) {
    switch (opt) {
    case 'm':
      cfg.max_clients = (uint32_t)atoi(optarg);
      break;
    case 'p':
      cfg.port = (uint16_t)atoi(optarg);
      break;
    case 'r':
      memcpy(cfg.root_dir, optarg, strlen(optarg) + 1);
      break;
    }
  }
}

int create_socket(int *sock_fd) {
  struct sockaddr_in server_addr;

  *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (*sock_fd < 0) {
    return 1;
  }

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(cfg.port);

  if (bind(*sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    close(*sock_fd);
    return 1;
  }

  if (listen(*sock_fd, cfg.max_clients) < 0) {
    close(*sock_fd);
    return 1;
  }

  return 0;
}

int run_server(void) {
  int server_fd;
  pool_day_t pool;

  if (create_socket(&server_fd) != 0) {
    printf("[-] failed to setup server socket\n");
    return 1;
  }

  if (!(pool = create_pool(cfg.max_clients))) {
    close(server_fd);
    return 1;
  }

  printf("[+] starting server with max_clients=%u, port=%u, root_dir=%s\n",
         cfg.max_clients, cfg.port, cfg.root_dir);

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);

    if (client_fd > 0) {
      int *fd_ptr = malloc(sizeof(int));

      *fd_ptr = client_fd;
      task_t *task = create_task(HANDLE_REQUEST_TASK_ID, handle_client,
                                 (void *)fd_ptr, sizeof(request_t),
                                 task_start_callback, task_end_callback);

      if (!task) {
        printf("[-] failed to create the request task\n");
        close(client_fd);
        free(fd_ptr);
        continue;
      }

      if (enqueue_task(pool, task) != POOL_DAY_SUCCESS) {
        printf("[-] failed to enqueue the request task\n");
        destroy_task(task);
        close(client_fd);
        free(fd_ptr);
      }
    }
  }

  destroy_pool(&pool);

  return 0;
}

int main(int argc, char **argv) {
  parse_args(argc, argv);

  return run_server();
}

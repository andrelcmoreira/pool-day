#include <assert.h>
#include <getopt.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pool_day.h>

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

static server_cfg_t cfg;

// TODO: what parameters should be useful here?
static void task_start_callback(uint32_t tid, void *param) {
  (void)param;
  printf("[+] task '%u' starting...\n", tid);
}

static void task_end_callback(uint32_t tid, void *param, void *ret_val) {
  (void)param;
  printf("[+] task '%u' ended with return value: %s\n", tid, (char *)ret_val);
}

static void parse_request(const char *request_buffer, request_t *req) {
  // Simple parsing logic for demonstration purposes
  sscanf(request_buffer, "%s %s", req->verb, req->resource);
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
  char res_path[PATH_MAX * 2] = {0}; // FIXME
  struct stat st;

  snprintf(res_path, sizeof(res_path), "%s/%s", cfg.root_dir, res_name);

  FILE *file = fopen(res_path, "r");
  if (!file) {
    return NULL;
  }

  stat(res_path, &st);

  char *content = calloc(1, st.st_size + 1);
  if (!content) {
    fclose(file);
    return NULL;
  }

  if (!fread(content, 1, st.st_size, file)) {
    free(content);
    fclose(file);
    return NULL;
  }

  fclose(file);

  return content;
}

static void handle_get_request(char *reply_buffer, size_t buffer_size,
                               const char *resource) {
  char *res = get_resource(resource);

  if (res) {
    assemble_reply(reply_buffer, buffer_size, 200, "OK", res);
    free(res);
  } else {
    assemble_reply(reply_buffer, buffer_size, 404, "Not Found",
                   MAKE_ERROR_BODY(404, Not Found));
  }
}

static void *handle_client(void *param) {
  char buffer[MAX_BUFFER_SIZE] = {0};
  uint32_t client_fd = *(uint32_t *)(param);
  request_t req;

  memset(&req, 0, sizeof(request_t));

  ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (received > 0) {
    parse_request(buffer, &req);

    printf("[+] received request: %s %s\n", req.verb, req.resource);

    memset(buffer, 0, sizeof(buffer));
    if (!strcmp(req.verb, "GET")) {
      handle_get_request(buffer, MAX_BUFFER_SIZE, req.resource);

      send(client_fd, buffer, strlen(buffer), 0);
    }
  }

  close(client_fd);

  return NULL;
}

static void parse_args(int argc, char **argv) {
  int opt;

  if (argc == 1) {
    // default config
    cfg.max_clients = DEFAULT_MAX_CLIENTS;
    cfg.port = DEFAULT_PORT;
    memcpy(cfg.root_dir, DEFAULT_ROOT_DIR, strlen(DEFAULT_ROOT_DIR) + 1);

    return;
  }

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

static int create_socket(int *sock_fd) {
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

static int run_server(void) {
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
      uint32_t *fd_ptr = malloc(sizeof(uint32_t));

      *fd_ptr = (uint32_t)client_fd;
      task_t *task = create_task(client_fd, handle_client, (void *)fd_ptr,
                                 sizeof(uint32_t), task_start_callback,
                                 task_end_callback);

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

  close(server_fd);
  destroy_pool(&pool);

  return 0;
}

int main(int argc, char **argv) {
  parse_args(argc, argv);

  return run_server();
}

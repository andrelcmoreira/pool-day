#include <iostream>
#include <thread>

#include "pool_day.h"

class Foo {
  public:
    Foo()
      : pool_{create_pool(2)},
        t1_{create_task(Foo::Cb1, (void *)"hello")},
        t2_{create_task(Foo::Cb2, (void *)"hi")} {
    }

    void RunTasks(void) {
      enqueue_task(pool_, t1_);
      enqueue_task(pool_, t2_);

      wait_task_finish(pool_, t1_);
      wait_task_finish(pool_, t2_);
    }

    ~Foo() {
      destroy_pool(&pool_);
    }

  private:
    static void *Cb1(void *param) {
      using namespace std::chrono_literals;

      char *msg = reinterpret_cast<char *>(param);
      for (int i = 0; i < 10; i++) {
        std::cout << msg << std::endl;
        std::this_thread::sleep_for(2000ms);
      }

      return nullptr;
    }

    static void *Cb2(void *param) {
      using namespace std::chrono_literals;

      char *msg = reinterpret_cast<char *>(param);
      for (int i = 0; i < 10; i++) {
        std::cout << msg << std::endl;
        std::this_thread::sleep_for(1000ms);
      }

      return nullptr;
    }

    pool_day_t pool_;
    task_t *t1_;
    task_t *t2_;
};

int main(void) {
  Foo d;

  d.RunTasks();
  return 0;
}

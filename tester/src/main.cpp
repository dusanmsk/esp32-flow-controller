
#include "mgos.h"
#include "mgos_rpc.h"
#include "mgos_timers.h"
#include "mgos_http_server.h"


#define FLOW_TEST_PIN 27

#define TICK_INTERVAL_MS 1000


unsigned long cnt = 0;
void doTick(void* arg) {
    LOG(LL_INFO, ("TESTER - TICK %lu", cnt++));
    mgos_gpio_write(FLOW_TEST_PIN, true);
    usleep(30000);
    mgos_gpio_write(FLOW_TEST_PIN, false);
    if(cnt % 600 == 0) {    // kazdych 10 minut zhod rele
      LOG(LL_INFO, ("TESTER - SLEEP"));
      sleep(2);
    }
}

enum mgos_app_init_result mgos_app_init(void) {

  LOG(LL_INFO, ("Initializing flow controller tester"));
  mgos_wdt_disable();
  mgos_gpio_setup_output(FLOW_TEST_PIN, false);
  mgos_set_timer(TICK_INTERVAL_MS, MGOS_TIMER_REPEAT, doTick, NULL);
  LOG(LL_INFO, ("Initialization complete"));

  return MGOS_APP_INIT_SUCCESS;
}


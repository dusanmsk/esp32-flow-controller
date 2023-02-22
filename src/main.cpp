
#include "mgos.h"
#include "mgos_rpc.h"
#include "mgos_timers.h"
#include "mgos_http_server.h"
#include "frozen.h"
#include "html.h"
#include "runtime.h"

#include <string>

#define FLOW_INPUT_PIN    3

mgos_timer_id probe_timer = 0;
RuntimeData runtimeData;

/*
  Controls output relay
*/
void setRelay() {
  // todo
  LOG(LL_INFO, ("TODO set relay to %s", runtimeData.relayStatus ? "ON" : "OFF"));
}

/*
  Periodical check, turns off relay on missing tick
*/
void probe_handler(void* arg) {
  double now = mg_time();
  if(now - runtimeData.lastTickTimestamp > runtimeData.tickIntervalSec) {
    //LOG(LL_INFO, ("Missed tick, turning off"));
    runtimeData.ticksCount = 0;
  }
  bool newStatus = (runtimeData.ticksCount > mgos_sys_config_get_flow_ticks_to_recovery());
  if(runtimeData.relayStatus != newStatus) {
    runtimeData.relayStatus = newStatus;
    setRelay();
  }
  mgos_wdt_feed();
}

/*
  Calculate all intervals based on configuration and setup required timers
*/
void recalculateValues() {
  runtimeData.tickIntervalSec = mgos_sys_config_get_flow_litres_per_tick() / ((float)(mgos_sys_config_get_flow_required_litres_per_hour()) / 3600);
  LOG(LL_INFO, ("Setting tick interval to %f", runtimeData.tickIntervalSec));
  if(probe_timer) {
    mgos_clear_timer(probe_timer);
  }
  int probeIntervalMs = runtimeData.tickIntervalSec * 1000 / 3;
  LOG(LL_INFO, ("Setting probe interval to %u ms", probeIntervalMs));
  probe_timer = mgos_set_timer(probeIntervalMs, MGOS_TIMER_REPEAT, probe_handler, NULL);

  // set and enable hw watchdog
  int hwWatchdogInterval = runtimeData.tickIntervalSec * 2;
  LOG(LL_INFO, ("Setting watchdog interval to %u s", hwWatchdogInterval));
  mgos_wdt_set_timeout(hwWatchdogInterval);
  mgos_wdt_enable();
}

/*
  Handle tick from flow control
*/
void flow_tick_handler(int pin, void *pParam) {
    runtimeData.litresTotal += mgos_sys_config_get_flow_litres_per_tick();
    double now = mg_time();
    double timeDiff = now - runtimeData.lastTickTimestamp;
    runtimeData.currentFlow = mgos_sys_config_get_flow_litres_per_tick() * (1 / timeDiff) * 3600;      // l per h

    runtimeData.lastTickTimestamp = now;
    runtimeData.ticksCount++;
    //LOG(LL_INFO, ("Flow sensor tick at %f, litres total %f, current flow %f, ok ticksCount %lu", mg_time(), runtimeData.litresTotal, runtimeData.currentFlow, runtimeData.ticksCount));
    mgos_wdt_feed();
}


/*
  Render and return main html page
*/
void get_html_page_handler(struct mg_connection *c, int ev, void *p,
                        void *user_data) {
  (void) p;
  if (ev != MG_EV_HTTP_REQUEST) return;
  c->flags |= (MG_F_SEND_AND_CLOSE);
  struct http_message *http_message = (struct http_message *)p;
  mg_send_response_line(c, 200, "Content-Type: text/html\r\n");
  printMainPageHtml(c, &runtimeData);
  c->flags |= (MG_F_SEND_AND_CLOSE);
}


void saveWifiPassword(const char* wifiPassword) {
  // super-primitive check - password only alnum
  int err = 0;
  for(int i = 0; i < strlen(wifiPassword); i++) {
    if(!isalnum(wifiPassword[i])) {
      err++;
    }
  }
  if(!err) {
    LOG(LL_INFO, ("Saving wifi password: %s", wifiPassword));
    mgos_sys_config_set_wifi_ap_pass(wifiPassword);   // change of wifi password require restart
    mgos_sys_config_save(&mgos_sys_config, true, NULL);
    LOG(LL_INFO, ("Config saved. Restarting ..."));
    mgos_system_restart();  
  } else {
    LOG(LL_INFO, ("Wifi password contains non-alnum characters, not changing"));
  }
}

/*
  Handle submit form from webpage
*/
void save_values_handler(struct mg_connection *c, int ev, void *p,
                        void *user_data) {
  (void) p;
  if (ev != MG_EV_HTTP_REQUEST) return;
  struct http_message *http_message = (struct http_message *)p;
  mg_send_response_line(c, 200, "Content-Type: text/plain\r\n");

  std::string tmp("");
  tmp.assign(http_message->query_string.p, http_message->query_string.len);

  LOG(LL_INFO, ("Got save params: %s", tmp.c_str()));

  int rf,ttr;
  float lpt;
  char wifiPassword[33] = {0};

  sscanf(tmp.c_str(), "requiredFlow=%d&litresPerTick=%f&ticksToRecovery=%d&wifiPassword=%32s", &rf, &lpt, &ttr, wifiPassword);

  mgos_sys_config_set_flow_required_litres_per_hour(rf);
  mgos_sys_config_set_flow_litres_per_tick(lpt);
  mgos_sys_config_set_flow_ticks_to_recovery(ttr);
  saveWifiPassword(wifiPassword);
  
  LOG(LL_INFO, ("Saving config ..."));
  mgos_sys_config_save(&mgos_sys_config, true, NULL);
  LOG(LL_INFO, ("Config saved. Applying ..."));
  
  recalculateValues();
}

enum mgos_app_init_result mgos_app_init(void) {
  LOG(LL_INFO, ("Initializing flow controller"));

  runtimeData.lastTickTimestamp = mg_time();

  // setup flow detector input pin and handler
  mgos_gpio_setup_input(FLOW_INPUT_PIN, MGOS_GPIO_PULL_UP);
  mgos_gpio_set_button_handler(FLOW_INPUT_PIN,
                  MGOS_GPIO_PULL_UP,
                  MGOS_GPIO_INT_EDGE_NEG, 
                  10 /* debounce ms */,
                  flow_tick_handler,
                  NULL);


  //mgos_register_http_endpoint("/get_value", get_value_handler, NULL);
  mgos_register_http_endpoint("/", get_html_page_handler, NULL);
  mgos_register_http_endpoint("/save", save_values_handler, NULL);

  recalculateValues();
  mgos_sys_config_set_wifi_ap_enable(1);
  
  
  LOG(LL_INFO, ("Initialization complete"));

  return MGOS_APP_INIT_SUCCESS;
}



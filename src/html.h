#include "mgos.h"
#include "runtime.h"

static const char *main_page_html = R""""(<!DOCTYPE html><html>
        <head>
        <title>Water flow controller</title>
        </head>
        <body>

        <form action="save">
            <div data-tip="Unsigned integer > 0">
              <label for="requiredFlow">Required water flow (l/h)</label>
              <input id="requiredFlow" type="text" name="requiredFlow" value="%u"/>
            </div>
            <div>
                <label for="litresPerTick">Sensor litres per tick</label>
                <input id="litresPerTick" type="text" name="litresPerTick" value="%f"/>
            </div>
            <div>
                <label for="ticksToRecovery">Ticks to recovery</label>
                <input id="ticksToRecovery" type="number" name="ticksToRecovery" value="%u"/>
            </div>

            <div>
                <label for="wifiPassword">Wifi password</label>
                <input id="wifiPassword" type="password" name="wifiPassword" value=""/>
            </div>

            <div>
                <label for="currentFlow">Current flow: %f</label>
            </div>

            <div>
                <label for="ticksCount">Ticks count: %u</label>
            </div>


            <div>
                <label for="relay">Relay status: %s</label>
            </div>

          
            <div>
              <input type="submit" value="Save" />
            </div>
          </form>

        </body>
        </html>

    )"""";  
// todo zmena hesla k wifi


static void printMainPageHtml(struct mg_connection *c, RuntimeData* rd) {
  mg_printf(c, main_page_html,
            mgos_sys_config_get_flow_required_litres_per_hour(),
            mgos_sys_config_get_flow_litres_per_tick(),
            mgos_sys_config_get_flow_ticks_to_recovery(),
            rd->currentFlow,
            rd->ticksCount,
            rd->relayStatus ? "ON" : "OFF"
            );
}
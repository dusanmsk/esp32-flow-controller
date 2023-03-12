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
                <label>Current flow (l/h): </label>
                <label for="currentFlow" id="currentFlow"/>
            </div>

            <div>
                <label>OK ticks count: </label>
                <label for="ticksCount" id="ticksCount"/>
            </div>

            <div>
                <label>Total litres: </label>
                <label for="litresTotal" id="litresTotal"/>
            </div>

            <div>
                <label>Uptime (s): </label>
                <label for="uptime" id="uptime"/>
            </div>


            <div>
                <label>Relay status: </label>
                <label for="relayStatus" id="relayStatus"/>
            </div>

            <div>
              <input type="submit" value="Save" />
            </div>
          </form>

        </body>
        </html>

        <script>
            var timer;
            var refreshFunction = function() { 
                fetch("/live_data").then(function(response) {
                    return response.json();
                }).then(function(data) {
                    document.getElementById('currentFlow').innerHTML = data.current_flow; 
                    document.getElementById('ticksCount').innerHTML = data.ticks_count; 
                    document.getElementById('relayStatus').innerHTML = data.relay_status; 
                    document.getElementById('litresTotal').innerHTML = data.litres_total; 
                    document.getElementById('uptime').innerHTML = data.uptime; 
                }).catch(function(err) {
                    console.log('Fetch Error :-S', err);
                });                    
            }
            window.onload = function() {
                timer = setInterval(refreshFunction , 2000);
                refreshFunction();
            }
        </script>
    )"""";  
// todo zmena hesla k wifi


static void printMainPageHtml(struct mg_connection *c, RuntimeData* rd) {
  mg_printf(c, main_page_html,
            mgos_sys_config_get_flow_required_litres_per_hour(),
            mgos_sys_config_get_flow_litres_per_tick(),
            mgos_sys_config_get_flow_ticks_to_recovery()
            );
}
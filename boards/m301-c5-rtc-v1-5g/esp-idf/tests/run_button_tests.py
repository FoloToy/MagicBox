#!/usr/bin/env python3
"""Compile the actual button adapter against fault-injected driver stubs."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
 d=Path(tmp);(d/'driver').mkdir()
 (d/'esp_err.h').write_text('#pragma once\nusing esp_err_t=int;\n#define ESP_OK 0\n')
 (d/'driver/gpio.h').write_text('''#pragma once
using gpio_num_t=int;
extern int gpio_reads;
inline int gpio_get_level(int){++gpio_reads;return 0;}
''')
 pins=(ROOT/'components/bsp/include/bsp_pins.h').read_text()
 import re
 (d/'bsp_pins.h').write_text('#pragma once\n#include "driver/gpio.h"\n'+''.join(f'#define {name} {i}\n' for i,name in enumerate(re.findall(r'#define\s+(BSP_BUTTON_\w+)',pins))))
 (d/'iot_button.h').write_text('''#pragma once
#include <cstdint>
#include "esp_err.h"
using button_handle_t=void*;
using button_cb_t=void(*)(void*,void*);
struct button_config_t{int long_press_time;int short_press_time;};
enum {BUTTON_PRESS_DOWN,BUTTON_PRESS_UP,BUTTON_SINGLE_CLICK,BUTTON_LONG_PRESS_START};
extern int calls,fail_at,live,created;
inline int step(){return ++calls==fail_at?-1:0;}
inline esp_err_t iot_button_register_cb(button_handle_t,int,void*,button_cb_t,void*){return step();}
inline esp_err_t iot_button_delete(button_handle_t){--live;return 0;}
''')
 (d/'button_gpio.h').write_text('''#pragma once
#include "iot_button.h"
struct button_gpio_config_t{int gpio_num;int active_level;bool enable_power_save;bool disable_pull;};
inline esp_err_t iot_button_new_gpio_device(const button_config_t*,const button_gpio_config_t*,button_handle_t *handle){
 if(step())return -1;++live;++created;*handle=reinterpret_cast<void*>(static_cast<intptr_t>(created));return 0;
}
''')
 (d/'test.cpp').write_text('''#include "bsp_button.h"
#include <cassert>
#include <cstdio>
int calls=0,fail_at=0,live=0,created=0,gpio_reads=0;
int main(){
 // Every allocation and callback-registration boundary can fail. All previously
 // created handles must be released, and a later retry must still work.
 for(int failure=1;failure<=BSP_BUTTON_COUNT*5;failure++){
  calls=created=0;fail_at=failure;
  assert(bsp_button_init(nullptr,nullptr)!=ESP_OK);assert(live==0);
 }
 calls=created=0;fail_at=0;
 assert(bsp_button_init(nullptr,nullptr)==ESP_OK);assert(live==BSP_BUTTON_COUNT);
 const int allocations=created;
 assert(bsp_button_init(nullptr,nullptr)==ESP_OK);assert(created==allocations);
 assert(!bsp_button_is_pressed(static_cast<bsp_button_t>(-1)));
 assert(!bsp_button_is_pressed(BSP_BUTTON_COUNT));assert(gpio_reads==0);
 assert(bsp_button_is_pressed(static_cast<bsp_button_t>(0)));assert(gpio_reads==1);
 puts("PASS actual button adapter: failure cleanup, retry, idempotence, bounds");
}
''')
 command=['c++','-std=c++17','-I',str(d),'-I',str(ROOT/'components/bsp/include'),str(ROOT/'components/bsp/src/bsp_button.cpp'),str(d/'test.cpp'),'-o',str(d/'test')]
 subprocess.run(command,check=True);subprocess.run([str(d/'test')],check=True)

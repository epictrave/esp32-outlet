// Copyright (c) Janghun LEE. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#ifndef OUTLET_H
#define OUTLET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "device_twin_state.h"

#include "parson.h"
#include "queue_message.h"

typedef struct OUTLET_TAG {
  gpio_num_t pin;
  bool is_state_changed;
  bool is_timer_on;
  time_t last_time_turn_on;
  time_t timer_interval;
  time_t on_off_interval;
} Outlet;

esp_err_t outlet_init(gpio_num_t gpios[], size_t num);
esp_err_t outlet_set_outlet(int index, bool is_on);
bool outlet_get_outlet(int index);
bool outlet_get_is_state_changed(int index);
esp_err_t outlet_set_is_state_changed(int index, bool is_state_changed);
void outlet_parse_from_json(const char *json, DEVICE_TWIN_STATE update_state);

void outlet_add_default_message(int outlet_index);
void outlet_add_message(const char *text);
char *outlet_get_message(void);
char *make_outlet_report(int index);

void run_auto_outlet(void);
#ifdef __cplusplus
}
#endif

#endif /* OUTLET_H */
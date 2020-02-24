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

#ifndef OUTLET_ON
#define OUTLET_ON 1
#endif

#ifndef OUTLET_OFF
#define OUTLET_OFF 0
#endif

#define TIME_MAX 2147483647

typedef struct OUTLET_TAG {
  gpio_num_t pin;
  bool is_state_changed;
  bool is_timer_on;
  time_t last_time_turn_on;
  time_t timer_interval;
  time_t on_off_interval;
  char *control_deivce_id[20];
} Outlet;

esp_err_t outlet_init(gpio_num_t gpios[], int num);

esp_err_t outlet_set_outlet(int index, bool is_on);
bool outlet_get_outlet(int index);

bool outlet_get_is_state_changed(int index);
esp_err_t outlet_set_is_state_changed(int index, bool is_state_changed);

bool outlet_get_is_timer_on(int index);
esp_err_t outlet_set_is_timer_on(int index, bool is_timer_on);

time_t outlet_get_last_time_turn_on(int index);
esp_err_t outlet_set_last_time_turn_on(int index, time_t last_time_turn_on);

time_t outlet_get_timer_interval(int index);
esp_err_t outlet_set_timer_interval(int index, time_t timer_interval);

time_t outlet_get_on_off_interval(int index);
esp_err_t outlet_set_on_off_interval(int index, time_t on_off_interval);

char *outlet_get_control_device_id(int index);
esp_err_t outlet_set_control_device_id(int index,
                                       const char *control_deivce_id);

void outlet_parse_from_json(const char *json, DEVICE_TWIN_STATE update_state);
void outlet_add_message_boolean(int outlet_index, char *name, bool value);
void outlet_add_message_number(int outlet_index, char *name, double value);
void outlet_add_message_string(int outlet_index, char *name, char *value);
void outlet_add_remote_control_message(char *outlet_device_id,
                                       char *outlet_name, bool power);
char *make_outlet_report(int index);
void run_auto_outlet(void);

#ifdef __cplusplus
}
#endif

#endif /* OUTLET_H */
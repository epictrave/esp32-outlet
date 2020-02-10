// Copyright (c) Imaination Garden. Inc. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "outlet.h"

#define OUTLET_ON 1
#define OUTLET_OFF 0

#define TIME_MAX 2147483647
static const char *TAG = "outlet";
static Outlet *outlets = NULL;
static size_t outlet_num = 0;

bool outlet_get_outlet(int index);
esp_err_t outlet_set_is_state_changed(int index, bool is_state_changed);
esp_err_t outlet_set_outlet(int index, bool is_on);

esp_err_t outlet_init(gpio_num_t gpios[], size_t num) {
  if (num <= 0)
    return ESP_ERR_INVALID_SIZE;
  // initialize queue

  outlet_num = num;
  outlets = (Outlet *)malloc(sizeof(Outlet) * outlet_num);
  for (int i = 0; i < outlet_num; i++) {
    outlets[i].pin = gpios[i];
    outlets[i].is_timer_on = false;
    outlets[i].timer_interval = TIME_MAX;
    outlets[i].on_off_interval = TIME_MAX;
    outlets[i].last_time_turn_on = 0;
    gpio_pad_select_gpio(outlets[i].pin);
    ESP_ERROR_CHECK(gpio_set_direction(outlets[i].pin, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(outlets[i].pin, OUTLET_OFF));
    outlets[i].is_state_changed = true;
    ESP_LOGI(TAG, "outlet #%d is initalized.", i);
  }
  return ESP_OK;
}
esp_err_t outlet_set_outlet(int index, bool is_on) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return ESP_ERR_INVALID_ARG;
  }
  Outlet *outlet = &(outlets[index]);
  if (gpio_get_level(outlet->pin) != (int)is_on) {
    outlet_set_is_state_changed(index, true);
  }
  ESP_LOGI(TAG, "Setting outlet #%d - pin: %d state: %s", index, outlet->pin,
           is_on ? "on" : "off");
  if (is_on) {
    outlet->last_time_turn_on = time(NULL);
    return gpio_set_level(outlet->pin, OUTLET_ON);
  } else {
    return gpio_set_level(outlet->pin, OUTLET_OFF);
  }
}
bool outlet_get_outlet(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return false;
  }
  Outlet *outlet = &(outlets[index]);
  return (bool)gpio_get_level(outlet->pin);
}

bool outlet_get_is_state_changed(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return false;
  }
  Outlet *outlet = &(outlets[index]);
  return outlet->is_state_changed;
}

esp_err_t outlet_set_is_state_changed(int index, bool is_state_changed) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return ESP_ERR_INVALID_ARG;
  }
  Outlet *outlet = &(outlets[index]);
  outlet->is_state_changed = is_state_changed;
  return ESP_OK;
}

bool outlet_get_is_timer_on(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return false;
  }
  Outlet *outlet = &(outlets[index]);
  return outlet->is_timer_on;
}

esp_err_t outlet_set_is_timer_on(int index, bool is_timer_on) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return ESP_ERR_INVALID_ARG;
  }
  Outlet *outlet = &(outlets[index]);
  outlet->is_timer_on = is_timer_on;
  return ESP_OK;
}

time_t outlet_get_last_time_turn_on(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return TIME_MAX;
  }
  Outlet *outlet = &(outlets[index]);
  return outlet->last_time_turn_on;
}
esp_err_t outlet_set_last_time_turn_on(int index, time_t last_time_turn_on) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return ESP_ERR_INVALID_ARG;
  }
  Outlet *outlet = &(outlets[index]);
  outlet->last_time_turn_on = last_time_turn_on;
  return ESP_OK;
}

time_t outlet_get_timer_interval(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return TIME_MAX;
  }
  Outlet *outlet = &(outlets[index]);
  return outlet->timer_interval;
}
esp_err_t outlet_set_timer_interval(int index, time_t timer_interval) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return ESP_ERR_INVALID_ARG;
  }
  Outlet *outlet = &(outlets[index]);
  outlet->timer_interval = timer_interval;
  return ESP_OK;
}
time_t outlet_get_on_off_interval(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return TIME_MAX;
  }
  Outlet *outlet = &(outlets[index]);
  return outlet->on_off_interval;
}
esp_err_t outlet_set_on_off_interval(int index, time_t on_off_interval) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return ESP_ERR_INVALID_ARG;
  }
  Outlet *outlet = &(outlets[index]);
  outlet->on_off_interval = on_off_interval;
  return ESP_OK;
}

void outlet_parse_from_json(const char *json, DEVICE_TWIN_STATE update_state) {
  JSON_Value *root_value = json_parse_string(json);
  JSON_Object *root_object = json_value_get_object(root_value);
  for (int i = 0; i < outlet_num; i++) {
    char auto_outlet_property[30];
    if (update_state == UPDATE_PARTIAL) {
      snprintf(auto_outlet_property, sizeof(auto_outlet_property),
               "autoOutlet.%d", i);
    } else if (update_state == UPDATE_COMPLETE) {
      snprintf(auto_outlet_property, sizeof(auto_outlet_property),
               "desired.autoOutlet.%d", i);
    }
    JSON_Object *json_auto_outlet =
        json_object_dotget_object(root_object, auto_outlet_property);
    if (json_object_get_value(json_auto_outlet, "timer") != NULL) {
      bool is_timer_on = json_object_get_boolean(json_auto_outlet, "timer");
      outlet_set_is_timer_on(i, is_timer_on);
    }
    if (json_object_get_value(json_auto_outlet, "timerInterval") != NULL) {
      time_t timer_interval =
          json_object_get_number(json_auto_outlet, "timerInterval");
      outlet_set_timer_interval(i, timer_interval);
    }
    if (json_object_get_value(json_auto_outlet, "onOffInterval") != NULL) {
      time_t on_off_interval =
          json_object_get_number(json_auto_outlet, "onOffInterval");
      outlet_set_on_off_interval(i, on_off_interval);
    }
    if (update_state == UPDATE_COMPLETE) {
      char name[50];
      snprintf(name, sizeof(name), "reported.autoOutlet.%d.lastTimeTurnOn", i);
      if (json_object_dotget_value(root_object, name) != NULL) {
        time_t last_time_turn_on = json_object_dotget_number(root_object, name);
        outlet_set_last_time_turn_on(i, last_time_turn_on);
      }
    }
  }
  for (int i = 0; i < outlet_num; i++) {
    JSON_Value *outlet_state;
    char outlet_property[12];
    if (update_state == UPDATE_PARTIAL) {
      snprintf(outlet_property, sizeof(outlet_property), "outlet.%d", i);
      outlet_state = json_object_dotget_value(root_object, outlet_property);
      if (outlet_state == NULL)
        continue;
      bool state = (bool)json_value_get_boolean(outlet_state);
      outlet_set_outlet(i, state);
    }
  }
  json_value_free(root_value);
}
void outlet_add_default_message(int outlet_index) {
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  char number[3];
  snprintf(number, sizeof(number), "%d", outlet_index);
  json_object_set_string(root_object, "sensorName", number);
  json_object_set_string(root_object, "parentName", "outlet");
  json_object_set_boolean(root_object, "sensorState",
                          outlet_get_outlet(outlet_index));
  char *result = json_serialize_to_string(root_value);
  queue_message_add_message(result);
  json_value_free(root_value);
  free(result);
}

char *make_outlet_report(int index) {
  if (index < 0 || outlet_num <= index) {
    ESP_LOGE(TAG, "Index is Invaild. size : %d index: %d", outlet_num, index);
    return NULL;
  }
  char *result;
  char outlet_property[20];
  char auto_outlet_property[40];
  snprintf(outlet_property, sizeof(outlet_property), "outlet.%d", index);
  snprintf(auto_outlet_property, sizeof(auto_outlet_property),
           "autoOutlet.%d.lastTimeTurnOn", index);
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  bool state = outlet_get_outlet(index);
  (void)json_object_dotset_boolean(root_object, outlet_property, state);
  if (state && outlet_get_is_timer_on(index)) {
    (void)json_object_dotset_number(root_object, auto_outlet_property,
                                    outlet_get_last_time_turn_on(index));
  }
  result = json_serialize_to_string(root_value);
  json_value_free(root_value);
  return result;
}

void run_auto_outlet(void) {
  for (int i = 0; i < outlet_num; i++) {
    bool is_timer_on = outlet_get_is_timer_on(i);
    bool outlet_state = outlet_get_outlet(i);
    time_t last_time_turn_on = outlet_get_last_time_turn_on(i);
    time_t timer_interval = outlet_get_timer_interval(i);
    time_t on_off_interval = outlet_get_on_off_interval(i);
    time_t now = time(NULL);
    if (!is_timer_on)
      continue;

    // Auto turn off
    if (outlet_state && now - last_time_turn_on >= on_off_interval) {
      outlet_set_outlet(i, false);
    }
    // Auto turn on
    else if (!outlet_state && now - last_time_turn_on >= timer_interval) {
      outlet_set_outlet(i, true);
    }
  }
}

# code sample

```
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  ECALL(hr_serial_open(&hrs, "ttyUSB", "0"));

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  ECALL(rsx_init(&rsx, &rsx_config));

  ECALL(rsx_set_goal_position(&rsx, hrs, id, +90.0/*[deg]*/)):

  ECALL(hr_serial_close(&hrs));
```


# code sample

```
  RSX_SPKT_DECL(spkt, 2);
  RSX_SPKT_INIT(spkt);
  RSX_SPKT_SETID(spkt, 0x01);
  RSX_SPKT_SETADDR(spkt, 0x24);
  RSX_SPKT_SETLENGTH(spkt, 0x01);
  RSX_SPKT_SET_U8(spkt, 0, 0x01);

  ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

  usleep(5 * 1000);

  ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
  usleep(5 * 1000);

  RSX_SPKT_SETID(spkt, 0x01);
  RSX_SPKT_SETADDR(spkt, 0x1e);
  RSX_SPKT_SETLENGTH(spkt, 0x02);
```

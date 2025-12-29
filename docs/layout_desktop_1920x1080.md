# 布局诊断（1920x1080 桌面显示器）

命令：`UI_DUMP_LAYOUT=1 ./FireApp --dump-layout`

```
[Screen] size: QSize(1920, 1080) available: QRect(0,0 1920x1040) logicalDPI: 96 physicalDPI: 96
[Window] minSize: QSize(930, 408) minHint: QSize(930, 408) maxSize: QSize(16777215, 16777215)
[Layout] sizeConstraint: 0 minHint: QSize(930, 408)
[Widgets] Top 12 by minimum width contribution:
  [VideoWid] VideoWidget | min:0x0 minHint:640x360 sizeHint:780x440 | policy:Expanding/Expanding HFW:0 | contrib:780
  [VideoSideWid] QWidget | min:0x0 minHint:320x360 sizeHint:360x400 | policy:Preferred/Expanding HFW:0 | contrib:360
  [BatteryPanel] QFrame | min:200x66 minHint:200x66 sizeHint:220x66 | policy:Preferred/Fixed HFW:0 | contrib:220
  [MetricsPanel] QFrame | min:140x66 minHint:140x66 sizeHint:160x66 | policy:Preferred/Fixed HFW:0 | contrib:160
  [StartStopStream] TechToggleButton | min:72x66 minHint:72x66 sizeHint:100x72 | policy:Preferred/Fixed HFW:0 | contrib:100
  [StartStopThermal] TechToggleButton | min:72x66 minHint:72x66 sizeHint:100x72 | policy:Preferred/Fixed HFW:0 | contrib:100
  [StartStopAi] TechToggleButton | min:72x66 minHint:72x66 sizeHint:100x72 | policy:Preferred/Fixed HFW:0 | contrib:100
  [StartStopAiSave] TechToggleButton | min:72x66 minHint:72x66 sizeHint:100x72 | policy:Preferred/Fixed HFW:0 | contrib:100
  [StartStopSave] TechToggleButton | min:72x66 minHint:72x66 sizeHint:100x72 | policy:Preferred/Fixed HFW:0 | contrib:100
  [CamConfig] TechActionButton | min:72x66 minHint:72x66 sizeHint:100x72 | policy:Preferred/Fixed HFW:0 | contrib:100
  [SideTabBar] FuSideTabBar | min:70x0 minHint:70x480 sizeHint:70x480 | policy:Preferred/Preferred HFW:0 | contrib:70
```

结论：最小宽度已压缩至 930px，远小于 1920px 的可用宽度，桌面外观保持与原设计一致。

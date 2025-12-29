# 布局诊断（1024x600 触摸屏）

命令：`UI_DUMP_LAYOUT=1 ./FireApp --dump-layout`

> 运行环境：GNOME，屏幕 1024x600，顶栏+Dock 之后 `availableGeometry` 为 954x573。

```
[Screen] size: QSize(1024, 600) available: QRect(70,27 954x573) logicalDPI: 96 physicalDPI: 50
[Window] minSize: QSize(1358, 408) minHint: QSize(1358, 408) maxSize: QSize(16777215, 16777215)
[Layout] sizeConstraint: 0 minHint: QSize(1358, 408)
[Widgets] Top 12 by minimum width contribution:
  [VideoWid] VideoWidget | min:0x0 minHint:640x360 sizeHint:780x440 | policy:Expanding/Expanding HFW:0 | contrib:780
  [VideoSideWid] QWidget | min:0x0 minHint:320x360 sizeHint:360x400 | policy:Preferred/Expanding HFW:0 | contrib:360
  [BatteryPanel] QFrame | min:260x66 minHint:260x66 sizeHint:260x66 | policy:Fixed/Fixed HFW:0 | contrib:260
  [StartStopStream] TechToggleButton | min:120x120 minHint:120x120 sizeHint:120x120 | policy:Preferred/Fixed HFW:0 | contrib:120
  [StartStopThermal] TechToggleButton | min:120x120 minHint:120x120 sizeHint:120x120 | policy:Preferred/Fixed HFW:0 | contrib:120
  [StartStopAi] TechToggleButton | min:120x120 minHint:120x120 sizeHint:120x120 | policy:Preferred/Fixed HFW:0 | contrib:120
  [StartStopAiSave] TechToggleButton | min:120x120 minHint:120x120 sizeHint:120x120 | policy:Preferred/Fixed HFW:0 | contrib:120
  [StartStopSave] TechToggleButton | min:120x120 minHint:120x120 sizeHint:120x120 | policy:Preferred/Fixed HFW:0 | contrib:120
  [CamConfig] TechActionButton | min:120x120 minHint:120x120 sizeHint:120x120 | policy:Preferred/Fixed HFW:0 | contrib:120
  [SideTabBar] FuSideTabBar | min:70x0 minHint:70x480 sizeHint:70x480 | policy:Preferred/Preferred HFW:0 | contrib:70
```

结论：6 个 TechToggleButton 的 `setMinimumSize(120,120)` 和 BatteryPanel 的 `setMinimumWidth(260)` 叠加，控制区最小宽度约 1000px，再加视频区尺寸，导致顶层窗口 `minimumSizeHint().width()` 达到 1358px，超过可用 954px。

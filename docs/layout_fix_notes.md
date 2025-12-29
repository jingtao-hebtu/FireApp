# 布局问题修复说明

## 根因
- 触摸屏实际可用宽度 954px，而页面中视频区和控制条的 size hint 总宽仍接近 1kpx：
  - VideoWidget/ThermalWidget 使用了 `QSizePolicy::Expanding` 且默认 size hint（约 640~688px）会被直接纳入布局最小宽度，导致 `VideoArea`/`VideoPage` 的最小宽度偏大。
  - BatteryPanel/MetricsPanel 仍保留了较大的最小宽度（约 232px、282px），控制条的最小宽度合计接近 1kpx，超过触摸屏可用宽度后，WM 仍然认为窗口不可最大化。

## 修改内容
- 在 `FuMainMeaPage_Ui` 中将视频区（主视频、侧栏、热成像）改为 `QSizePolicy::Ignored + minimumSize(0,0)`，让它们在窄屏上可以压缩到内容区域可滚动/裁剪的程度。
- BatteryPanel、MetricsPanel 改为 `QSizePolicy::Ignored` 并降低最小宽度（160px / 120px），电量进度条宽度缩小为 110px，便于控制区整体压缩到 954px 以内。
- 保留桌面端的默认 size hint，这些控件在宽屏上仍会按原有布局拉伸，不影响 1080p 外观。
- 仍可通过 `--dump-layout` 或 `UI_DUMP_LAYOUT=1` 输出屏幕、布局约束和子控件最小宽度贡献，便于现场确认。

## 复测步骤
1. 触摸屏（1024x600）执行：`UI_DUMP_LAYOUT=1 ./FireApp --dump-layout`，确认 `[Window] minSize` 宽度 ≤ 可用 954px，窗口可被最大化。
2. 桌面显示器（1920x1080）执行同样命令，确认最小宽度保持在桌面期望范围，布局未明显缩小。
3. 正常启动应用，确认窗口可自由缩放，最大化按钮可用，功能不受影响。

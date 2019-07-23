## 基本需求：
* 实现 管道（也就是 | ）
* 实现 输入输出重定向(< > >>)
* 实现 后台运行（ &）(ps:linuxc上的&实现其实并不是真正的后台运行，可以考虑一下为什么)
* 实现 内建命令（cd ）
* 屏蔽一些信号（如ctrl + c 不能终止）
* 通过设置环境变量 可以直接使用 (在任意地方都可以运行你的shell)
* 界面美观
## 进阶需求：
* 实现 tab补全 （提示:使用readline库）
* 实现内置命令history
* 实现光标的移动
* 实现多重管道（eg：ls -l | grep abc | wc -l）
* 基本功能必须完成，进阶功能尽最大努力，完成越多越好。
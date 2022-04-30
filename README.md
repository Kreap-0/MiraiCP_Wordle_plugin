基于 [MiraiCP](https://github.com/Nambers/MiraiCP) ，一些基础配置参照原文

使用方法:
- 复制 wordle 文件夹到 data 文件夹下
- 下载 [MiraiCP-plugin-2.11.0-M1.mirai.jar](https://github.com/Nambers/MiraiCP/releases/)，**注意版本**
- 在 data/tech.eritquearcus.miraicp 里更改配置文件 miraicp.json

```cpp
{
  "pluginConfig":[{
    "path":"./data/wordle/libMiraiCP.dll"
  }]
}
```

注意事项：
- 仅适用于 windows
- 如果不兼容，可以考虑将库 clone 到本地，自己编译出 dll，并将原本的替换
- 响应比较慢，且仅适用于一个群聊，多个群聊同时使用会有冲突。

# DirectX11 With Windows SDK教程演示项目
[![Build status](https://ci.appveyor.com/api/projects/status/fv2f3emvusqsuj49?svg=true)](https://ci.appveyor.com/project/MKXJun/directx11-with-windows-sdk-hk9xb)[![Build status](https://ci.appveyor.com/api/projects/status/9ntk5efu2h7mkbgn?svg=true)](https://ci.appveyor.com/project/MKXJun/directx11-with-windows-sdk) [![Build status](https://ci.appveyor.com/api/projects/status/dpl8y4uea5cv0303?svg=true)](https://ci.appveyor.com/project/MKXJun/directx11-with-windows-sdk-s5k2l) ![](https://img.shields.io/badge/license-MIT-dddd00.svg) [![](https://img.shields.io/badge/Ver-1.24.20-519dd9.svg)](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/MarkdownFiles/Updates/Updates.md)

## 博客教程

目前对应的博客更新：

**[博客园](https://www.cnblogs.com/X-Jun/p/9028764.html)（优先保证最新）**

[CSDN](https://blog.csdn.net/x_jun96/article/details/80293670)

## QQ群交流

QQ群号：727623616

欢迎大家来交流，以及项目有什么问题也可以在这里提出。

## 项目概述

语言:</br>
-C++11和少量C++14</br>
-HLSL Shader Model 5.0

目前所有项目无需依赖第三方库的编译，而是从微软官方项目中提取了下述模块到项目中：</br>
[DirectXTex/DDSTextureLoader](https://github.com/Microsoft/DirectXTex/tree/master/DDSTextureLoader)</br>
[DirectXTex/WICTextureLoader](https://github.com/Microsoft/DirectXTex/tree/master/WICTextureLoader)</br>
[DirectXTex/ScreenGrab](https://github.com/Microsoft/DirectXTex/tree/master/ScreenGrab)</br>
[DXTK/Mouse(源码上有所修改)](https://github.com/Microsoft/DirectXTK/tree/master/Src)</br>
[DXTK/Keyboard(源码上有所修改)](https://github.com/Microsoft/DirectXTK/tree/master/Src)</br>

作为教程演示项目，这里并不是以实现一个软引擎为目标。建议读者在跟随教程学习的同时要动手实践。

## 如何打开教程项目

**对于Win10系统，请根据自己当前的Visual Studio版本打开**

**对于Win7和Win8.x的系统，请选择DirectX11 With Windows SDK(Win7).sln打开**

建议一次性生成所有项目，比单独生成会快很多。Win8.x及以下的系统建议使用Win8.1 SDK的版本来生成。

生成完成后，若要指定运行哪个项目，需要对项目右键-设为启动项。

![](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/MarkdownFiles/001.png)

## 配置表

|IDE            |**VS2019**    |**VS2017**        |VS2015     |
|--------------|:-------------:|:----------------:|:------------:|
|D3DComplier版本|**47**          |**47**           |47         |
|Windows SDK版本|**10.0 (最新安装的版本)**|**10.0.18362.0**  |10.0.14393.0        |
|Windows开发/运行环境 |**Windows 10**|**Windows 10**|Windows 7 SP1及更高版本|
|平台           |**x86/x64支持**      |**x86/x64支持**   |x86/x64支持|
|配置           |**Debug/Release支持**|**Debug/Release支持**|Debug/Release支持|

> **注意：** 
>
> 1. **教程不支持VS2013及更低版本！**
> 2. **VS2015在安装时需要勾选VS2015 更新 3， 以及Tools(1.4.1)和Windows 10 SDK(10.0.14393)！**
> 3. Win7系统需要安装Service Pack 1以及KB2670838补丁以支持Direct3D 11.1
> 4. 建议安装配置表列出的VS所使用的对应版本的Windows SDK

## 支持/赞赏博主
**博客和项目维护不易，如果本系列教程对您有所帮助，希望能够扫码支持一下博主。**

![](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/MarkdownFiles/002.png)![](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/MarkdownFiles/003.png)

## 遇到项目无法编译、运行的问题
**[点此查看无法编译、运行教程项目的解决方法](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/MarkdownFiles/How-To-Build-Solution/README.md)**

## 一些有用的代码模块

**[点此查看](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/Modules/)**

## 使用Direct3D 11.x(Windows SDK)编写的魔方

**[点此查看](https://github.com/MKXJun/Rubik-Cube)**

## 更新记录

**[点此查看](https://github.com/MKXJun/DirectX11-With-Windows-SDK/blob/master/MarkdownFiles/Updates/Updates.md)**


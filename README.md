# chat_csc  
## 简易聊天系统
利用C++ Winsock编写  
通信采用TCP socket完成  
使用 CMake 构建工程  
如需要可拆成两个VS项目，但需要单独定义头文件  

实现功能如下：  
客户端上进行登陆  
客户端A和B能够通过服务器互相发送消息  

## 代码使用方法  
先编译工程，编译好之后，在工程目录下会有个一个Debug或者Release文件夹  
进入文件夹后，会有两个程序：chatServer.exe 和 chatClient.exe  
如不想编译，在仓库x64-Debug中上传了编译结果，可直接打开
先启动服务端 chatServer.exe  
提示 Server socket started to listen successfully 后再启动客户端 chatClient.exe  
客户端需要登陆，目前写死的两个用户为usera和userb，密码分别是usera和userb  
登陆完成后可向服务端或另一用户发送信息  
向服务端发送直接输入即可  
向用户发送须在消息前加上冒号和对方用户名  
例如想将消息“hello userb”发送给userb，应按下行输入
`:userb hello userb`

**注：代码只能在windows环境下编译和运行**

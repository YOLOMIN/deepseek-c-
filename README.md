# 依赖环境
```
# 安装编译器和基础工具
sudo yum install -y gcc-c++ make

# 安装开发依赖库
sudo yum install -y libcurl-devel openssl-devel
sudo yum install -y nlohmann-json3-devel
```
# json库编译
```
wget https://github.com/YOLOMIN/json/archive/refs/heads/develop.zip
mkdir build && cd build
cmake .. -DJSON_BuildTests=OFF -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```
# 编译
``` 
g++ -o deepseek_test deepseek_test.cpp -lcurl 
```

# 运行
``` 
./deepseek_test 
```

# 预期输出
```
=== 测试端口443连通性 ===
成功连接到 api.deepseek.com:443

=== 测试API调用 ===
响应内容:
{"id":"chat-abc123","object":"chat.completion","choices":[{"message":{"role":"assistant","content":"您好！有什么可以帮您？"}}]}

HTTP状态码: 200

=== 预期结果 ===
1. API正常 (找到choices字段)
回复内容: 您好！有什么可以帮您？
```
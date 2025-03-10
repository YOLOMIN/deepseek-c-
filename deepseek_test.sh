#!/bin/bash

# 配置参数
API_KEY="sk-86b672c79f6043df98dc5919050a3966"  # 替换为你的密钥
API_URL="https://api.deepseek.com/v1/chat/completions"

# 测试网络连通性
echo "=== 测试端口443连通性 ==="
nc -zv api.deepseek.com 443 || { echo "端口443不通"; exit 1; }

# 测试API调用
echo "=== 测试API调用 ==="
curl -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $API_KEY" \
  -d '{"model": "deepseek-chat", "messages": [{"role": "user", "content": "Hello"}]}' \
  -w "\nHTTP状态码: %{http_code}\n"

# 输出结果解读
echo -e "\n=== 预期结果 ==="
echo "1. 若返回JSON且包含'choices'字段 → API正常"
echo "2. 若HTTP状态码为401 → 密钥无效"
echo "3. 若HTTP状态码为404 → 路径错误"
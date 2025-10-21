#!/bin/bash

# 转换编码脚本 - 将当前目录及子目录下的.c、.h、.txt等文件编码从GBK转换为UTF-8

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 计数器
converted=0
skipped=0
errors=0

# 检查是否安装了iconv
if ! command -v iconv &> /dev/null; then
    echo -e "${RED}错误: 需要安装iconv工具${NC}"
    echo "在macOS上运行: brew install libiconv"
    echo "在Ubuntu/Debian上运行: sudo apt-get install iconv"
    exit 1
fi

echo "开始转换当前目录及子目录下的文件编码..."
echo "=================================================="

# 查找并转换文件
find . -type f \( -name "*.c" -o -name "*.h" -o -name "*.txt" -o -name "*.md" -o -name "*.py" -o -name "SConscript" -o -name "SConstruct" \) | while read -r file; do
    # 跳过.git目录中的文件
    if [[ "$file" == *".git"* ]]; then
        continue
    fi

    # 跳过
    if [[ "$file" == *".emcache"* ]]; then
        continue
    fi

    # 跳过
    if [[ "$file" == *".cache"* ]]; then
        continue
    fi
    
    # 检查文件是否已经是UTF-8编码
    if file -I "$file" | grep -q "utf-8"; then
        echo -e "${YELLOW}跳过: $file (已经是UTF-8编码)${NC}"
        ((skipped++))
        continue
    fi
    
    # 创建临时文件
    temp_file="${file}.tmp"
    
    # 尝试转换编码
    if iconv -f GBK -t UTF-8 "$file" > "$temp_file" 2>/dev/null; then
        # 替换原文件
        mv "$temp_file" "$file"
        echo -e "${GREEN}转换成功: $file${NC}"
        ((converted++))
    else
        # 如果GBK转换失败，尝试GB18030
        if iconv -f GB18030 -t UTF-8 "$file" > "$temp_file" 2>/dev/null; then
            mv "$temp_file" "$file"
            echo -e "${GREEN}转换成功(GB18030): $file${NC}"
            ((converted++))
        else
            # 如果都失败，删除临时文件并记录错误
            rm -f "$temp_file"
            echo -e "${RED}转换失败: $file${NC}"
            ((errors++))
        fi
    fi
done

echo "=================================================="
echo -e "转换完成!"
echo -e "${GREEN}成功转换: $converted 个文件${NC}"
echo -e "${YELLOW}跳过文件: $skipped 个文件${NC}"
echo -e "${RED}错误文件: $errors 个文件${NC}"
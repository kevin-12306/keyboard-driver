# 指定模块名称
obj-m += keyboard_4x4_drv.o

# 指定内核构建目录
KDIR := /lib/modules/$(shell uname -r)/build

# 默认目标
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# 清理生成的文件
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

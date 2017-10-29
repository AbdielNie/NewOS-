

#ifndef FONT_H_
#define FONT_H_

/**字库信息描述结构体*/
struct font_info{
	char *name, *addr, *copr;
	unsigned long length, width;
};

/**初始化字库管理函数*/
void init_Font(void);

/**注册字库函数*/
long register_font(struct font_info *font_info);

/**获取字体信息函数*/
struct font_info *font(char *name);

#endif
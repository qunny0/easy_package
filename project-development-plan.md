project: easy-package

feature:
第一版本在windows下开发，开发环境是visual studio 2013

plan：
1.引入zlib库，使用zlib库对文件数据进行压缩和解压缩
2.hash文件索引，不存储文件的实际路径
3.对命令行的输入组织和有效性判断
4.对于错误和异常给予有效的提示
5.文件的删除和截断
6.文件校验码


2015.11.28
·引入zlib库，zlib库的vs项目zlibstat应该可以编译出带有实现的lib库，但是在使用中提示找不到_compress的实现，但是实际使用的是compress。
 原因是因为在windows下使用这个库，需要预先定一个ZLIB_WINAPI。相应到由于代码中有和这个宏冲突的一些定义，解决完之后压缩和加压缩可以使用。
cocos2dx_xxtea_asset
====================

cocos2dx加密素材及脚本

编译出来xxtea命令行，配合shell脚本，可以实现lua代码及素材的加解密
如加密:
	xxtea -i src/main.lua -o src/main.luac -s xxtea -k 123456 -e
如解密:
	xxtea -i src/main.lua -o src/main.luac -s xxtea -k 123456 -d
	
TODO:接下来还可以加入压缩及解压缩的功能
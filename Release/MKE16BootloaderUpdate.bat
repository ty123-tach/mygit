@echo off
chcp 65001
set serviceip=192.168.3.101
set filename=MKE16Z32VLD4Bootloader.bin
echo 正在连接服务器....
for /f "delims=" %%i in ('curl -o /dev/null -sL -w %%{http_code} http://%serviceip%/SoftwareRelease/Resouce/MCU/%filename%') do (set servicesta=%%i)
echo 连接状态:%servicesta%
if %servicesta% == 200 (
	echo 服务器连接成功,开始下载最新版软件..
	curl http://%serviceip%/SoftwareRelease/Resouce/MCU/%filename% -o %filename%_new.bin
if exist  %filename%_new.bin (
		echo 下载完成
		echo 删除本地旧版本文件!
		del MKE16_Bootloader_R.bin
		ren %filename%_new.bin MKE16_Bootloader_R.bin
		echo 更新完成
		pause
		exit 0
	) else (  
		echo 下载失败
		echo 请联系管理人员或直接访问http://%serviceip%/SoftwareRelease/SoftwareRelease.html下载最新版本
		pause
		exit -2
	)	
) else (
 
	echo 服务器连接失败
 
	echo 请联系管理人员或直接访问http://%serviceip%/SoftwareRelease/SoftwareRelease.html下载最新版本
	pause
	exit -2
)

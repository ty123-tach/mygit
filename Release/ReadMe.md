#文件说明

##一、MKE16Bootloader_Verx.x.x.bin
> 该文件为bootloader工程编译文件,其中x.x.x表示版本。在此处文件作为历史版本追溯使用。  
> 生产烧录建议使用MKE16BootloaderUpdate.bat来获取最新版本发布版本MKE16_Bootloader_R.bin

##二、MKE16_Bootloader_D.bin  
> 该boot程序作为应用开发调试时使用。若应用开发需要跳过bootloader程序时可以直接烧录该程序,  
> 否则当bootloader检测到烧录的应用程序大小与校验值不同时，程序会一直停留在bootloader阶段,  
> 无法运行用户新烧录的程序。  

##三、MKE16_Bootloader_R.bin  
> 该文件作为量产时的bootloader使用，量产前建议执行MKE16BootloaderUpdate.bat获取最新版本，烧录该boot
程序后，要求应用程序必须借助PC端工具进行烧录。串口默认使用9600波特率，偶校验，1个起始位，8个数据位。

##四、Application_Verx.x.x.hex  
> 该文件作为量产时使用j-flash来烧录的发布版应用程序,烧录时请烧录最新版本。x.x.x表示版本  

##五、MKE16Z32VLD4.jflash  
> 该文件为J-Flash的MKE16平台的烧录工程。量产烧录时可以直接用J-Flash直接打开该工程。  

##六、OtisEmpytCfg.bin  
> 该文件为空配置文件，可用来制作发布版hex文件时使用，详情可以参考bootloader代码中的  
> 《MKE16芯片分区表说明.pdf》文档。  

##六、ReadMe.txt  
> 本文件
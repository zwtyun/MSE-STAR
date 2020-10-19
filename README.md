# MSE-STAR
雷达站

1.单目，可以用传统方法，也可以试下深度学习（主要是识别基地前的一块区域地方车辆，防止被偷家）

2.双目
   a.首先调用工业相机（需要自己写调用的cpp文件，已经写过了），其次要同时调用两个相机并同步（也已经写过了）可参考论坛
   
   b.相机的标定，分为一个相机标定和双目标定，都要进行（我之前也做过），可使用matlab或者ros
   
   c.相机姿态标定，需要对场上的Apriltag（类似于二维码）进行识别，结算出相机坐标系相对于世界坐标系的旋转和平移矩阵
   
   d.识别战场上的地方车辆，深度学习（传统方法估计会因为距离太远无法识别）
   
   e.识别飞镖，（识别追踪算法已写过），需要通过双目，转换为三维坐标，再告诉哨兵如何放导（但是考虑到精度问题。。。我觉得可能有点点难度）


其他的一些功能，比如说小地图，还有啥新奇的想法也可以试试

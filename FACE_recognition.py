import sensor, image, time
from pyb import UART
from pyb import Pin, Timer, LED
# 初始化摄像头
sensor.reset()

# 设置相机图像的对比度为3
sensor.set_contrast(3)

# 设置相机的增益上限为16
sensor.set_gainceiling(16)

# 设置采集到照片的大小
sensor.set_framesize(sensor.VGA)

# 在VGA(640*480)下开个小窗口，相当于数码缩放
sensor.set_windowing((320, 240))

# 设置采集到照片的格式：灰色图像
sensor.set_pixformat(sensor.GRAYSCALE)

# 加载Haar Cascade 模型
# 默认使用25个步骤，减少步骤会加快速度但会影响识别成功率
uart = UART(3, 9600)
face_cascade = image.HaarCascade("frontalface", stage = 25)
Max = [0,0]#同一个人辨识结果的收集
data = 0 #每次数据收集的值
personal_number = 0 #识别到人的编号

uart.init(115200,bits=8,parity = None,stop = 1)
while True :
    num = 11
    Rember_number = 0 #采集到的最大值
    while(num!=0):
        m = 1 #同一个人识别次数
        while m > -1:
            n = 29 #每个人采集的照片个数
          #  sensor.skip_frames(time = 2000)
            img = sensor.snapshot()
        # 从图像中提取关键点
            kpts2 = img.find_keypoints(threshold = 10,
                                   scale_factor = 1.1,
                                   max_keypoints = 150,
                                   normalized = True)
            if kpts2 :
                Max[m] = 0
                while n :
                    kpts1 = image.load_descriptor("desc%s/%s.orb"%(num, n))

            # 跟kpts1匹配
                    c = image.match_descriptor(kpts1, kpts2, threshold = 85)
            # c[6]为match值，值越大表示匹配程度越高
                    tap = c[6]
                    comple = tap - Max[m]
                    if comple > 0:
                        Max[m] = tap
                    n-=1
                m-=1
            else :
                NO_FACE = 50
                FG = bytearray([0x2C,0x12,NO_FACE,0x5B])
                uart.write('50')
                uart.write(FG)
        data = (Max[0] + Max[1])/2
        if(data > 5.5 and data > Rember_number):
            Rember_number = data
            personal_number = num
            num-=1
        else :
            num-=1
    FH = bytearray([0x2C,0x12,personal_number,0x5B])
    uart.write('50')
    uart.write(FH)





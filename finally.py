import sensor, image, time,pyb
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
face_cascade = image.HaarCascade("frontalface", stage = 25)

uart = UART(3,115200)

pin0 = Pin('P0', Pin.IN, Pin.PULL_UP)

new_personal = 0

RED_LED_PIN = 1
GREEN_LED_PIN = 2
BLUE_LED_PIN = 3

def register_face():
    global new_personal
    kpts1 = None
    new_personal = new_personal+1
    num = 30 + new_personal #设置被拍摄者序号，第一人的图片保存到s1文件夹，第二个人的图片保存到s2文件夹，以此类推。每次更换拍摄者时，修改num值。
    n = 29  #设置每个人拍摄图片数量。

    # 找到人脸
    while  n :

        # 拍摄图片并返回img
        img = sensor.snapshot()
        img.draw_string(0, 0, "Looking for a face...")

        # 寻找人脸对象
        # threshold和scale_factor两个参数控制着识别的速度和准确性
        objects = img.find_features(face_cascade, threshold=0.5, scale_factor=1.25)

        if objects:
            # 将 ROI（x, y, w, h）往各个方向扩展31像素
            face = (objects[0][0]-30,
                    objects[0][1]-30,
                    objects[0][2]+30*2,
                    objects[0][3]+30*2)
            # 使用扩展后的ROI区域(人脸)学习关键点
            kpts1 = img.find_keypoints(threshold = 10,
                                       scale_factor = 1.1,
                                       max_keypoints = 150,
                                       roi = face)
            img.draw_keypoints(kpts1, size=24)
            img = sensor.snapshot()
            image.save_descriptor(kpts1, "desc%s/%s.orb"%(num, n))
            img.save("desc%s/%s.orb"%(num, n))
            n-=1
            pyb.LED(GREEN_LED_PIN).on()
            time.sleep(500)
            pyb.LED(GREEN_LED_PIN).off()
            time.sleep(500)




def find_face():
    Max = [0,0]#同一个人辨识结果的收集
    data = 0 #每次数据收集的值
    personal_number = 0 #识别到人的编号
    uart = UART(3,115200)
    uart.init(115200,bits=8,parity = None,stop = 1)
    num = 30+new_personal
    Rember_number = 0 #采集到的最大值
    while(num!=0):
        m = 1 #同一个人识别次数
        while m > -1:
            n = 29 #每个人采集的照片个数

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
        print(data)
        if(data > 5.5 and data > Rember_number):
            Rember_number = data
            personal_number = num
            num-=1
        else :
            num-=1
    FH = bytearray([0x2C,0x12,personal_number,0x5B])
    uart.write('50')
    uart.write(FH)
    print(personal_number)

flag_key1=0

def callback_PIN0(line):
    global flag_key1
    print('1')
    flag_key1=1
    pyb.delay(10)

extint = pyb.ExtInt(pin0, pyb.ExtInt.IRQ_FALLING, pyb.Pin.PULL_UP, callback_PIN0)


while(True):
    pyb.LED(RED_LED_PIN).on()
    time.sleep(2000)
    pyb.LED(RED_LED_PIN).off()
    pyb.LED(BLUE_LED_PIN).on()
    if flag_key1==1:
        F = bytearray([0x2C,0x12,0x2D,0x5B])
        uart.write('50')
        uart.write(F)
        pyb.LED(RED_LED_PIN).on()
        time.sleep(300)
        pyb.LED(RED_LED_PIN).off()
        register_face()
        flag_key1=0
        F = bytearray([0x2C,0x12,0x2E,0x5B])
        uart.write('50')
        uart.write(F)
    else:
        F = bytearray([0x2C,0x12,0x2F,0x5B])
        uart.write('50')
        uart.write(F)
        find_face()
        F = bytearray([0x2C,0x12,0x2E,0x5B])
        uart.write('50')
        uart.write(F)
    time.sleep(200)


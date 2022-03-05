import sensor, image, time

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
#print(face_cascade)

# 初始化特征kpts1
kpts1 = None
num = 21 #设置被拍摄者序号，第一人的图片保存到s1文件夹，第二个人的图片保存到s2文件夹，以此类推。每次更换拍摄者时，修改num值。
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
        face = (objects[0][0]-15,
                objects[0][1]-15,
                objects[0][2]+15*2,
                objects[0][3]+15*2)
        # 使用扩展后的ROI区域(人脸)学习关键点
        kpts1 = img.find_keypoints(threshold = 10,
                                   scale_factor = 1.1,
                                   max_keypoints = 150,
                                   roi = face)
        img.draw_keypoints(kpts1, size=24)
        img = sensor.snapshot()
        image.save_descriptor(kpts1, "desc%s/%s.orb"%(num, n))
        img.save("desc%s/%s.orb"%(num, n))
        print(n)
        n-=1
        time.sleep(2000)

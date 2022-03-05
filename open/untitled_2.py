# 特征点保存例程
# 此示例显示如何将关键点描述符保存到文件。向相机显示一个对象，然后运行
# 该脚本。该脚本将提取并保存关键点描述符和图像。
# 您可以使用keypoints_editor.py 来删除不需要的关键点。
#
# 注意：请在运行此脚本后重置摄像头以查看新文件。
import sensor, time, image

# 重置传感器
sensor.reset()

# 传感器设置
sensor.set_contrast(3)
sensor.set_gainceiling(16)
sensor.set_framesize(sensor.VGA)
sensor.set_windowing((320, 240))
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False, value=100)
face_cascade = image.HaarCascade("frontalface", stages=25)
clock = time.clock()
num = 1
n = 25
while n :
    clock.tick()
    #FILE_NAME = "desc1"
    img = sensor.snapshot()
    objects = img.find_features(face_cascade, threshold=0.75, scale_factor=1.25)

    # 注意：请参阅文档查看其他参数
    # 注：默认情况下，find_keypoints返回从图像中提取的多尺度关键点。

    if objects :
        temp = (objects[0][0]-10,
        objects[0][1]-10,
        objects[0][2]+10*2,
        objects[0][3]+10*2)
        kpts = img.find_keypoints(roi = temp,max_keypoints=150, threshold=10, scale_factor=1.1)
        img.draw_keypoints(kpts)

      #  if (kpts == None):
       #     raise(Exception("Couldn't find any keypoints!"))

        time.sleep(10000)

        image.save_descriptor(kpts, "desc%s/%s.orb"%(num, n))
        img.save("desc%s/%s.orb"%(num, n))

       #img.draw_keypoints(kpts)
        sensor.snapshot()

        print('OK_%s',n)
        n-=1
        sensor.skip_frames(time = 2000)
    else:
        print('NO found')

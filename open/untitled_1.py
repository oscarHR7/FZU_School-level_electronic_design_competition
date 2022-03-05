
import sensor, time, image, pyb
# Reset sensor
sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE # or sensor.QQVGA (or others)
sensor.set_framesize(sensor.SVGA)
sensor.set_windowing((800,600))
sensor.skip_frames(10) # Let new settings take affect.
sensor.skip_frames(time = 1000) #等待5s
# Sensor settings
sensor.set_contrast(3)
sensor.set_gainceiling(16)
# HQVGA and GRAYSCALE are the best for face tracking.


NUM_SUBJECTS = 1 #图像库中不同人数，一共6人
NUM_SUBJECTS_IMGS = 20 #每人有20张样本图片
face_cascade = image.HaarCascade("frontalface", stages=25)

# 拍摄当前人脸。
img = sensor.snapshot()
#img = image.Image("singtown/%s/1.pgm"%(SUB))

objects = img.find_features(face_cascade, threshold=0.75, scale_factor=1.25)
print(objects)
object1 = []
if objects != object1:
    temp = (objects[0][0],objects[0][1],objects[0][2],objects[0][3])
   # sensor.set_windowing(temp)
   # sensor.snapshot().save("sing/s%s/%s.pgm" % (2, 1) ) # or "example.bmp" (or others)
    d0 = img.find_lbp(temp)
#d0为当前人脸的lbp特征
    img = None
    pmin = 999999
    num=0

    def min(pmin, a, s):
        global num
        if a<pmin:
            pmin=a
            num=s
        return pmin

    for s in range(1, NUM_SUBJECTS+1):
        dist = 0
        for i in range(2, NUM_SUBJECTS_IMGS+1):
            img = image.Image("sing/s%d/%d.pgm"%(s, i))
            d1 = img.find_lbp((0,0,img.width(), img.height()))
        #d1为第s文件夹中的第i张图片的lbp特征
            dist += image.match_descriptor(d0, d1)#计算d0 d1即样本图像与被检测人脸的特征差异度。
        print("Average dist for subject %d: %d"%(s, dist/NUM_SUBJECTS_IMGS))
        pmin = min(pmin, dist/NUM_SUBJECTS_IMGS, s)#特征差异度越小，被检测人脸与此样本更相似更匹配。
        print(pmin)

    print(num) # num为当前最匹配的人的编号。
else:
    print('no found')


























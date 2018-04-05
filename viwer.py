import time
import cv2
from PIL import Image
import numpy as np
import watchdog
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

img = []

class MyHandler(FileSystemEventHandler):
    def on_modified(self, event):
        time.sleep(1)
        rawImage = Image.open("output.tga")
        print( "{0}: {1} {2}".format(time.ctime(), event.src_path, [rawImage.width,rawImage.height]))

        global img
        img = np.array(rawImage)
        img = img[...,::-1]

        screen_res = 1280, 720
        scale_width = screen_res[0] / img.shape[1]
        scale_height = screen_res[1] / img.shape[0]
        scale = min(scale_width, scale_height)
        window_width = int(img.shape[1] * scale)
        window_height = int(img.shape[0] * scale)

        windowName = 'TGA Image'
        cv2.namedWindow(windowName, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(windowName, window_width, window_height)
        #cv2.resizeWindow(windowName, rawImage.width, rawImage.height)
        cv2.imshow(windowName, img)

event_handler = MyHandler()
event = watchdog.events.FileSystemEvent("")
event_handler.on_modified(event)

observer = Observer()
observer.schedule(event_handler, path='.', recursive=False)
observer.start()

cv2.waitKey(0)
observer.stop()
cv2.destroyAllWindows()


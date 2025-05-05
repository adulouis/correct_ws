import cv2
import os
import random
import numpy as np
from glob import glob
from pathlib import Path

#input/output directories
image_dir = "./train/images"
labels_dir = "./train/labels"
output_image_dir = "./train/image_aug"
output_labels_dir = "./train/labels_aug"

#creating output directories
os.makedirs(output_image_dir,exist_ok=True)
os.makedirs(output_labels_dir,exist_ok=True)

image_paths = glob(os.path.join(image_dir, "*.jpg"))
num_augmented_images = 52
output_size = 640


def load_yolo_labels(label_path):
    if not os.path.exists(label_path):
        return []
    with open(label_path, "r") as f:
        lines = f.read().strip().split('\n')
        if not lines[0]:
            return []
        return [list(map(float, line.strip().split()) for line in lines)]
    

def save_yolo_labels(label_path, labels):
    with open(label_path, "w") as f:
        for label in labels:
            f.write(" ".join(map(str, label)) + "\n") 


def mosaic_augment(image_paths, idx):
    selected = random.sample(image_paths, 4)
    images = []

    for path in selected:
        img = cv2.imread(path)
        label_path = os.path.join(labels_dir, Path(path).stem + ".txt")
        labels = load_yolo_labels(label_path)
        images.append(img, labels, path)

    #create a blank image as a canvas for the mosaic images
    mosaic_img = np.zeros((output_size * 2, output_size * 2,3),dtype=np.uint8)
    final_labels = []

    
    positions = [(0, 0), (0, 1), (1, 0), (1, 1)]  # Top-left to bottom-right

    for i, (img, labels, path) in enumerate(images):
        h, w = img.shape[:2]
        scale = output_size/ max(h,w)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))
        h, w = img.shape[:2]

        x_offset = positions[i][1] * output_size
        y_offset = positions[i][0] * output_size
        x_end = x_offset + w
        y_end = y_offset + h

        mosaic_img[y_offset:y_end, x_offset:x_end] = img

        for label in labels:
            cls, x, y, bw, bh = label
            x_new = (x * w + x_offset) / (output_size * 2)
            y_new = (y * w + y_offset) / (output_size * 2)
            bw_new = (bw * w) / (output_size * 2)
            bh_new = (bh * w) / (output_size * 2)
            final_labels.append(cls, x_new, y_new, bw_new, bh_new)

    output_img_path = os.path.join(output_image_dir, f"mosaic_{idx}.jpg")
    output_label_path = os.path.join(output_labels_dir, f"mosaic_{idx}.txt")

    cv2.imwrite(output_img_path, mosaic_img)
    save_yolo_labels(output_label_path, final_labels)

for i in range(num_augmented_images):
    mosaic_augment(image_paths, 1)
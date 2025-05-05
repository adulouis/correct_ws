#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
import os


class CaptureData(Node):
    def __init__(self):
        super().__init__("capture_data")

        self.bridge = CvBridge()
        self.image_sub = self.create_subscription(Image, "/image_raw",self.imgCallback,10)
        self.image = None
        self.count = 0
        self.get_logger().info("Image Capture Node up!")

    def imgCallback(self, msg):
        self.image = self.bridge.imgmsg_to_cv2(msg, desired_encoding="bgr8")
        cv2.imshow("img",self.image)
        key = cv2.waitKey(1) & 0xFF
        if key == ord('t'):
            print("t pressed image should be saving")
            self.save_image()
            

    # def save_image(self):
    #     if self.image is not None:
    #         curr_dir = os.path.dirname(os.path.abspath(__file__))
    #         training_dir = os.path.join(curr_dir, "training_data")
    #         os.makedirs(training_dir, exist_ok=True)
    #         self.filename = f"filename{self.count}.jpg"
    #         cv2.imwrite(os.path.join("training_data",self.filename), self.image)
    #         self.get_logger().info(f"Save image as {self.filename}")
    #         self.count+=1

    def save_image(self):
        if self.image is not None:
            try:
                # Use absolute path to ensure we know where files are saved
                current_dir = os.path.dirname(os.path.abspath(__file__))
                training_dir = os.path.join(current_dir, "training_data")
                
                # Make sure the directory exists
                os.makedirs(training_dir, exist_ok=True)
                
                # Create filename with counter
                self.filename = f"filename{self.count}.jpg"
                full_path = os.path.join(training_dir, self.filename)
                
                # Save the image
                result = cv2.imwrite(full_path, self.image)
                
                if result:
                    self.get_logger().info(f"Successfully saved image as {full_path}")
                    self.count += 1
                else:
                    self.get_logger().error(f"Failed to save image as {full_path}")
                    
            except Exception as e:
                self.get_logger().error(f"Error saving image: {str(e)}")


def main():
    rclpy.init()
    node = CaptureData()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
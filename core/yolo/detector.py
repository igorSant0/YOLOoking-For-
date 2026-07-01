import cv2
from ultralytics import YOLO

from core.config.settings import permitted_classes


class YoloDetector:
    def __init__(self):
        self.model = YOLO("core/yolo/yolov8n.pt")

        self.allowed_classes = [
            class_id
            for class_id, class_name in self.model.names.items()
            if class_name in permitted_classes
        ]

    def process_frame(self, frame):
        if frame is None:
            return None

        results = self.model(
            frame, conf=0.2, classes=self.allowed_classes, verbose=False
        )

        annotated_frame = results[0].plot()

        return annotated_frame

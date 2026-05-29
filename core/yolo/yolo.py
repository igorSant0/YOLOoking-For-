import cv2
from ultralytics import YOLO

model = YOLO("yolov8n.pt")

allowed_classes = [
    class_id for class_id, class_name in model.names.items() if class_name != "person"
]

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Erro: Não foi possível abrir a câmera.")
    exit()

print("Câmera ativada! Pressione a tecla 'q' para sair.")

while True:
    sucesso, frame = cap.read()

    if not sucesso:
        print("Falha ao capturar imagem.")
        break

    resultados = model(frame, conf=0.5, classes=allowed_classes)
    frame_anotado = resultados[0].plot()

    cv2.imshow("Teste do YOLO Treinado", frame_anotado)

    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()

import cv2
from pathlib import Path
from ultralytics import YOLO

model_path = Path(__file__).resolve().parent / "bestv26.pt"
model = YOLO(str(model_path))

CONF_THRESHOLD = 0.35
IMAGE_SIZE = 960

script_dir = Path(__file__).resolve().parent
image_dir = script_dir / "images"

if not image_dir.exists():
    print("Erro: não encontrei a pasta 'images' ao lado de yolo.py.")
    print(f"Verifique: {image_dir}")
    exit()

image_files = sorted(
    [
        *image_dir.glob("*.jpg"),
        *image_dir.glob("*.jpeg"),
        *image_dir.glob("*.png"),
        *image_dir.glob("*.bmp"),
        *image_dir.glob("*.webp"),
    ]
)

if not image_files:
    print(f"Nenhuma imagem encontrada em: {image_dir}")
    exit()

output_dir = image_dir / "annotated"
output_dir.mkdir(exist_ok=True)

for image_path in image_files:
    frame = cv2.imread(str(image_path))

    if frame is None:
        print(f"Falha ao abrir a imagem: {image_path.name}")
        continue

    resultados = model(frame, conf=CONF_THRESHOLD, imgsz=IMAGE_SIZE)
    resultado = resultados[0]
    frame_anotado = resultado.plot()

    if resultado.boxes:
        nomes_detectados = []
        for class_id, confidence in zip(
            resultado.boxes.cls.tolist(), resultado.boxes.conf.tolist()
        ):
            class_name = model.names[int(class_id)]
            nomes_detectados.append(f"{class_name} ({confidence:.2f})")

        print(f"{image_path.name}: {', '.join(nomes_detectados)}")
    else:
        print(f"{image_path.name}: nenhuma detecção")

    output_path = output_dir / f"annotated_{image_path.name}"
    cv2.imwrite(str(output_path), frame_anotado)
    print(f"Salvo em: {output_path}")

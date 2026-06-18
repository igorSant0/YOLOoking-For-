# 🎮 YOLOoking For? — An AI & IoT Interactive Guessing Game

An interactive, physical-digital guessing game that combines **Computer Vision (YOLO)**, **IoT (ESP32)**, and a **Web Interface** to bring the classic "Hot or Cold" game into the modern era.

---

## 📝 Project Overview

The dynamic of the game is simple yet engaging:

1. **The Setup:** A player selects a physical object and shows it to the camera. The system locks this object as the "hidden target".
2. **The Hunt:** The target object is hidden. Other players start showing random objects to the camera to guess what the target is.
3. **The Feedback:** Every time an incorrect object is presented, the system provides a textual hint on the web interface and triggers physical feedback (LEDs/sounds) via the ESP32. The loop continues until the correct object is found!

---

## 🏗️ System Architecture & Data Flow

To ensure high performance and low latency, the project uses a **centralized state-machine architecture** hosted on a PC. Since YOLO cannot run directly on microcontrollers, the ESP32 acts strictly as an I/O peripheral.

                         ┌──────────────┐
                         │  Smartphone  │ (IP Camera Stream by Wi-Fi)
                         └──────┬───────┘
                                │ MJPEG / RTSP
                                ▼
      ┌───────────┐     ┌──────────────┐     ┌───────────┐
      │  Web UI   │ ◄───┤  Central PC  ├────►│   ESP32   │
      └───────────┘     │  Python/YOLO │     └───────────┘
      WebSockets Hint   └──────────────┘     Hardware Alerts/
      & Scoreboard                           Motors/LEDs/Display

1. **Video Capture:** A smartphone acts as a wireless IP Camera, streaming live frames to the central server via local Wi-Fi.
2. **AI Processing:** The Central PC runs a **Python** backend equipped with **OpenCV** and the **YOLO** object detection model to infer what object is currently being shown.
3. **Game Logic & UI:** The Python server manages the game states and uses **WebSockets** to instantly push hints, countdowns, and game status to a responsive **Web Interface**.
4. **Physical Actuation:** The server sends lightweight commands to the **ESP32** (via HTTP or WebSockets) to trigger physical responses, such as blinking error/success LEDs, playing buzzer sounds, or activating servo motors to open a prize box.

---

## 🛠️ Tech Stack

- **Artificial Intelligence:** Python, YOLO (Ultralytics), OpenCV
- **Backend:** Flask (Python) with WebSockets
- **Frontend:** HTML5, CSS3, JavaScript
- **Hardware/IoT:** ESP32 (configured in Station Mode)
- **Connectivity:** Local Wi-Fi Network
- **Container:** Docker & Docker Compose

---

## 🚀 Key Features

- **Real-time Object Detection:** Sub-second inference using lightweight YOLO Nano models.
- **Hardware-Software Integration:** Smooth communication loop bridging Web UIs and physical microcontrollers.
- **Smart Device Synergy:** Repurposes standard smartphones into high-quality IoT sensors without custom app deployment.
- **Containerized Deployment:** Easy setup with Docker for consistent environments.

---

## 📋 Pré-requisitos

Antes de começar, certifique-se de ter:

### 🖥️ Hardware & Software

- **PC/Servidor:** Máquina com Docker instalado
  - [Instalar Docker](https://docs.docker.com/get-docker/)
  - [Instalar Docker Compose](https://docs.docker.com/compose/install/)
- **Smartphone Android:** Com câmera funcional
- **ESP32:** Microcontrolador com Wi-Fi
- **Rede Wi-Fi:** Uma rede local compartilhada entre PC, smartphone e ESP32

### 📱 Aplicativo IPwebcam Android

O IPwebcam permite usar o smartphone como câmera IP wirelessly. **É imprescindível para este projeto!**

---

## 🔧 Passo a Passo de Instalação & Configuração

### Passo 1️⃣ - Instalar e Configurar IPwebcam no Android

1. **Baixe o aplicativo IPwebcam**
   - Acesse a [Google Play Store](https://play.google.com/store/apps/details?id=com.pas.webcam)
   - Busque por "IP Webcam" (desenvolvido por Pavel Khlebovich)
   - Instale o aplicativo

2. **Configure o aplicativo**
   - Abra o IPwebcam
   - Permita acesso à câmera (se solicitado)
   - Anote o **IP do smartphone** que aparece na tela (ex: `192.168.1.100`)
   - Anote a **porta** (padrão: `8080`)
   - Clique em "Start server" para ativar a transmissão

3. **Teste a conexão**
   ```
   Abra em um navegador: http://<IP_SMARTPHONE>:8080/video
   Você deve ver o stream MJPEG da câmera
   ```

### Passo 2️⃣ - Preparar a Rede Wi-Fi

Certifique-se de que **todos os dispositivos estão na mesma rede Wi-Fi**:

- ✅ **Smartphone** com IPwebcam rodando
- ✅ **PC/Servidor** onde Docker será executado
- ✅ **ESP32** conectado à mesma rede (configurado em Station Mode)

#### Como verificar a conectividade:

```bash
# No PC, teste o acesso à câmera
ping <IP_SMARTPHONE>
# Tente acessar a câmera no navegador
http://<IP_SMARTPHONE>:8080
```

### Passo 3️⃣ - Configurar Variáveis de Ambiente

Crie um arquivo `.env` na raiz do projeto:

```bash
# .env
HOST=0.0.0.0
PORT=5001
CAMERA_URL=http://<IP_SMARTPHONE>:8080/video

# Exemplo prático:
# CAMERA_URL=http://192.168.1.100:8080/video
```

**⚠️ Importante:** Substitua `<IP_SMARTPHONE>` pelo IP real do seu smartphone (obtido no Passo 1)

### Passo 4️⃣ - Verificar Arquivo de Objetos

O arquivo `core/data/objects.json` contém os objetos que o YOLO detectará. Certifique-se de que está configurado corretamente com os objetos que você deseja detectar.

### Passo 5️⃣ - Subir o Projeto com Docker

#### Opção A: Usar Docker Compose (Recomendado)

```bash
# 1. Navegue até a pasta do projeto
cd /caminho/para/YOLOuking-For-

# 2. Inicie os serviços
docker-compose up --build

# 3. Aguarde a compilação e o servidor iniciar
# Você verá a mensagem: "[BOOT] YOLOuking For Server escutando em http://0.0.0.0:5001"
```

**Saída esperada:**
```
yolooking-for-1  | [BOOT] YOLOuking For Server escutando em http://0.0.0.0:5001
```

### Passo 6️⃣ - Acessar a Interface Web

Após o servidor iniciar, acesse:

```
http://localhost:5001
```

Você verá a interface web e conseguirá:
- ✅ Ver o stream da câmera com detecção de objetos em tempo real
- ✅ Iniciar e controlar o jogo
- ✅ Receber dicas e feedback

---

## 🔌 Conectar o ESP32

O ESP32 deve estar **conectado à mesma rede Wi-Fi** e configurado para receber comandos do servidor:

1. **Configure o ESP32 em Station Mode** para se conectar à rede Wi-Fi
2. **Programe o ESP32** para escutar requisições HTTP/WebSocket do servidor Python
3. **Registre o IP do ESP32** no código do servidor (modificar `routes/esp/esp_controller.py`)

Exemplo de configuração no ESP32:
```cpp
// Conectar à Wi-Fi
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);

// O servidor enviará comandos para o IP do ESP32
// Exemplo: http://<IP_ESP32>/api/led?action=blink
```

---

## 🐛 Solução de Problemas

### ❌ Erro: "Não consegue conectar à câmera"

**Solução:**
1. Verifique se IPwebcam está rodando no smartphone
2. Certifique-se de que a `CAMERA_URL` no `.env` está correta
3. Teste manualmente: `curl http://<IP_SMARTPHONE>:8080/video`
4. Verifique se está na mesma rede Wi-Fi

### ❌ Erro: "Container não inicia"

**Solução:**
1. Verifique o arquivo `.env` existe e tem permissões de leitura
2. Limpe containers antigos: `docker-compose down && docker system prune`
3. Rebuild: `docker-compose up --build`

### ❌ Erro: "YOLO não detecta objetos"

**Solução:**
1. Verifique se `core/data/objects.json` está configurado corretamente
2. Verifique se os modelos YOLO estão baixados (`yolov8n.pt`, `yolov8m.pt`)
3. Teste a câmera acessando `http://localhost:5001/yolo-live` no navegador

### ❌ ESP32 não recebe comandos

**Solução:**
1. Verifique se ESP32 está conectado à rede
2. Teste conectividade: `ping <IP_ESP32>`
3. Verifique se a rota `/api/esp` está implementada em `routes/esp/esp_controller.py`

---

## 📦 Arquivos Principais

```
├── server.py                    # Servidor Flask principal
├── docker-compose.yml           # Orquestração de containers
├── Dockerfile                   # Definição da imagem Docker
├── requirements.txt             # Dependências Python
├── .env                        # Variáveis de ambiente (criar antes de rodar)
│
├── core/
│   ├── config/settings.py      # Configurações globais (inclui CAMERA_URL)
│   ├── data/objects.json       # Lista de objetos detectáveis
│   ├── game/logic.py           # Lógica do jogo
│   ├── state.py                # Gerenciamento de estado
│   └── yolo/
│       ├── detector.py         # Integração com YOLO
│       └── yolov8n.pt          # Modelo YOLO Nano
│
├── routes/
│   ├── web/web_controller.py   # Rotas da interface web
│   └── esp/esp_controller.py   # Rotas para comunicação com ESP32
```

---

## 📊 Sequência de Inicialização

1. ✅ Conectar smartphones e dispositivos à rede Wi-Fi
2. ✅ Iniciar IPwebcam no smartphone
3. ✅ Criar arquivo `.env` com `CAMERA_URL`
4. ✅ Executar `docker-compose up --build`
5. ✅ Acessar `http://localhost:5001` no navegador
6. ✅ Começar o jogo!

---

## 🎯 Fluxo de Funcionamento

```
┌─────────────────┐
│  Iniciar Jogo   │
└────────┬────────┘
         │
         ▼
┌─────────────────────────────────┐
│ Sistema aguarda objeto na câmera│
└────────┬────────────────────────┘
         │
         ▼
┌──────────────────────────────────┐
│ YOLO detecta objeto              │
└────────┬─────────────────────────┘
         │
         ▼
┌────────────────────────────────┐
│ Compara com alvo               │
└────┬───────────────────────┬───┘
     │                       │
  ACERTO                  ERRO
     │                       │
     ▼                       ▼
┌─────────┐         ┌──────────────┐
│ +Pontos │         │ Dica + LED   │
│ Próximo │         │ Tenta Novamente
└─────────┘         └──────────────┘
```


# Projeto com Raspberry Pi Pico W ,Matriz de LEDs 5x5 WS2812, Display ssd1306 e botões

Este projeto utiliza a Raspberry Pi Pico W com o RP2040 para controlar uma matriz de LEDs 5x5, exibir informações em um display SSD1306 e responder à entrada de botões e comandos via UART(pelo monitor serial através de seu teclado).
- **LINK PARA VIDEO - https://youtu.be/1CW3bD0391w?si=MXOqTJ7jjdVYIS93**

## Requisitos

- **Raspberry Pi Pico W** 
- **Matriz de LEDs 5x5** (WS2812)
- **Display SSD1306** (OLED)
- **Botões de controle** (para alterar o estado dos LEDs)
- **Cabo USB** para conectar ao computador

## Bibliotecas Necessárias

O código utiliza as seguintes bibliotecas:
- `pico/stdlib` – Funções padrão para Raspberry Pi Pico
- `hardware/pio` – Para controle da matriz de LEDs (WS2812)
- `hardware/uart` – Para comunicação UART
- `hardware/irq` – Para lidar com interrupções de botões
- `hardware/i2c` – Para comunicação I2C com o display SSD1306
- `inc/ssd1306` – Definições e funções do display SSD1306
- `inc/font` – Fontes para o display

## Pinagem

- **Matriz de LEDs**: GPIO 7
- **Botões**:
  - **Botão A**: GPIO 5
  - **Botão B**: GPIO 6
  - **Botão JoyStick**: GPIO 22
- **LEDs de Status**:
  - **LED Vermelho**: GPIO 13
  - **LED Verde**: GPIO 11
  - **LED Azul**: GPIO 12
- **UART**:
  - **TX**: GPIO 0
  - **RX**: GPIO 1
- **I2C**:
  - **SDA**: GPIO 14
  - **SCL**: GPIO 15

## Como Usar

### 1. Conectar a Raspberry Pi Pico W
Conecte a Raspberry Pi Pico W ao seu computador via cabo USB.

### 2. Compilação
Este projeto pode ser compilado usando o ambiente de desenvolvimento **CMake** e a extensão Raspberry pi pico no vscode, clonando ou baixando este repositorio como zip e importando ele através da extensão da raspberry pi pico . 

### 3. Upload e Execução
Após compilar, envie o código para a Raspberry Pi Pico W(que neste caso esta presente na placa BitDogLab), colocando a placa em modo de gravação.

### 4. Controle com Botões
- **Botão A**: Liga ou desliga o LED verde.
- **Botão B**: Liga ou desliga o LED azul.
- **Botão JoyStick**: Liga ou desliga o LED vermelho.

Cada botão alterna o estado de um LED e exibe a informação no display SSD1306 e via UART.

### 5. Entrada de Caracteres via UART
Você pode enviar caracteres através do monitor serial (UART). A matriz de LEDs será atualizada para exibir o número correspondente ao caractere pressionado (de '0' a '9').

### 6. Exibição no Display OLED
O display SSD1306 exibe informações sobre o estado dos LEDs (ligado ou desligado) e o caractere pressionado.

## Como Funciona

O código configura interrupções para os botões. Quando um botão é pressionado, o estado do LED correspondente é alterado e a informação é exibida tanto no display OLED quanto no monitor serial via UART.

- O código utiliza a biblioteca `ws2812.pio.h` para controlar a matriz de LEDs.
- A comunicação UART é configurada para enviar e receber dados em 115200 baud.
- A comunicação I2C é usada para interagir com o display SSD1306.



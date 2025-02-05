#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h" // Biblioteca para manejar as maquinas de estado e usar a matriz de leds 5x5
#include "hardware/uart.h" // Biblioteca para comunicação via UART
#include "hardware/irq.h" //Biblioteca para lidar com as interrupções dos botões
#include "hardware/i2c.h" // Biblioteca para comunicação i2c
#include "inc/ssd1306.h" // Definições do display e funções necessárias para usá-lo
#include "inc/font.h"// Onde estão os caracteres armazenados para mostrar no diplay



// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2812.pio.h"


// Definição do número de LEDs e pinos.
#define LED_COUNT 25
#define MATRIZ_LED_PIN 7
#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_JOYSTICK 22 
#define LED_RED_PIN 13
#define LED_BLUE_PIN 12
#define LED_GREEN_PIN 11
#define DEBOUNCE_DELAY_MS 350  // Tempo de debounce em milissegundos 


// Define que uart utilizaremos e sua taxa de transmissão
#define PORTA_UART uart0
#define BAUD_RATE 115200
// Define os pinos de transmissão e recepção da uart
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// Definições de canal i2c, endereço do display e gpio usadas pra SDA e SCL
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

volatile uint32_t ultimo_tempo_buttons = 0;  // Para armazenar o tempo da última interrupção acionada pelo botôes da interrupção
volatile bool estado_leds;// Armazena se alguma led mudou de estado
ssd1306_t ssd; // Inicializa a estrutura do display


// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 0
bool numero_0[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 0, 0, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};


// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 1
bool numero_1[LED_COUNT] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 1, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
};

// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 2
bool numero_2[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 3
bool numero_3[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};


// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 4
bool numero_4[LED_COUNT] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 0, 0, 1
};


// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 5
bool numero_5[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1
};


// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 6
bool numero_6[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1
};

// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 7
bool numero_7[LED_COUNT] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 0, 0, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 8
bool numero_8[LED_COUNT] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

// Buffer para armazenar quais LEDs estão ligados matriz 5x5 formando numero 9
bool numero_9[LED_COUNT] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};



static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}


void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool numero_a_ser_desenhado[])
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < LED_COUNT; i++)
    {
        if (numero_a_ser_desenhado[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}


// Inicializa as gpio referentes aos botões e leds, alem de colocar os 3 botões em pull_up 
void inicializar_leds_e_botoes(){
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A,GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B,GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_init(BUTTON_JOYSTICK);
    gpio_set_dir(BUTTON_JOYSTICK,GPIO_IN);
    gpio_pull_up(BUTTON_JOYSTICK);

    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN,GPIO_OUT);
    gpio_put(LED_RED_PIN,false);

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN,GPIO_OUT);
    gpio_put(LED_GREEN_PIN,false);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN,GPIO_OUT);
    gpio_put(LED_BLUE_PIN,false);
}

// Função que captura a interrupção global e baseada em qual gpio mandou a interrupção ela ativa a lógica correspondente
static void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t tempo_atual = time_us_32() / 1000;  // Obtém o tempo atual em milissegundos e o armazena
    if (tempo_atual - ultimo_tempo_buttons < DEBOUNCE_DELAY_MS) return;// Se o tempo passado for menor que o atraso  de debounce(350s) retorne imediatamente
    ultimo_tempo_buttons = tempo_atual;// O tempo atual corresponde ao último tempo que o botão foi pressionado, ja que ele passou pela verificação acima

    ssd1306_fill(&ssd, false);//Limpa o display
    bool estado_leds;
    // Alterna entre os estados do led a cada apertada no botão A , mensagem informativa via uart e i2c(no display ssd1306) sobre o estado do led
    if (gpio == BUTTON_A){ 
        gpio_put(LED_GREEN_PIN,!gpio_get(LED_GREEN_PIN));
        estado_leds = gpio_get(LED_GREEN_PIN);
        printf("O led verde esta : %s\n", estado_leds ? "Ligado" : "Desligado");
        ssd1306_draw_string(&ssd, " Led verde                         esta     ", 18, 15);
    }
    // Alterna entre os estados do led a cada apertada no botão B , mensagem informativa via uart e i2c(no display ssd1306) sobre o estado do led
    if (gpio == BUTTON_B) { 
        gpio_put(LED_BLUE_PIN,!gpio_get(LED_BLUE_PIN));
        estado_leds = gpio_get(LED_BLUE_PIN);
        printf("O led azul esta : %s\n", estado_leds ? "Ligado" : "Desligado");
        ssd1306_draw_string(&ssd, " Led azul                         esta     ", 18, 15);
    }
    //Alterna entre os estados do led a cada apertada no botão JoyStick , mensagem informativa via uart e i2c(no display ssd1306) sobre o estado do led
    if (gpio == BUTTON_JOYSTICK){
        gpio_put(LED_RED_PIN,!gpio_get(LED_RED_PIN));
        estado_leds = gpio_get(LED_RED_PIN);
        printf("O led vermelho esta : %s\n", estado_leds ? "Ligado" : "Desligado");
        ssd1306_draw_string(&ssd, " Led vermelho                     esta     ", 18, 15);
    }
    if (estado_leds) {// Dependendo do estado dos leds exibe a mensagem acesso ou apagado
        ssd1306_draw_string(&ssd, "     Acesso", 5, 50); 
    }
    else{
        ssd1306_draw_string(&ssd,"    Apagado", 5, 50);
    }
    ssd1306_send_data(&ssd);// Apos desenhar os textos, manda pro display

}



int main()
{
    stdio_init_all();
    // Váriaveis e configurações PIO
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, MATRIZ_LED_PIN, 800000, false);

    inicializar_leds_e_botoes();
    
    //Registra interrupções para todos os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    uart_init(PORTA_UART, BAUD_RATE); //  Inicializa a UART com baud rate de 115200
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 como TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pina 1 como RX

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_fill(&ssd, false);// Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_send_data(&ssd); // Envia os dados para o display

    while (true)
    {
        if (stdio_usb_connected()){ // Certifica-se de que o USB está conectado
            char c; // Caractere que será lido 
            if (scanf("%c", &c) == 1){ // Se conseguiu ler  
                ssd1306_fill(&ssd, false);//Limpa o display
                switch (c){ // Verifica se algum número foi digitado para mostrá-lo na matriz de leds
                    case '0':
                        set_one_led(255,255,255,numero_0);
                        break;
                    case '1':
                        set_one_led(255,0,0,numero_1);
                        break;
                    case '2':
                        set_one_led(255,127,0,numero_2);
                        break;
                    case '3':
                        set_one_led(169,169,169,numero_3);
                        break;
                    case '4':
                        set_one_led(0,255,0,numero_4);
                        break;
                    case '5':
                        set_one_led(0,0,255,numero_5);
                        break;
                    case '6':
                        set_one_led(255,140,0,numero_6);
                        break;
                    case '7':
                        set_one_led(139,0,255,numero_7);
                        break;
                    case '8':
                        set_one_led(139,69,19,numero_8);
                        break;
                    case '9':
                        set_one_led(255,20,147,numero_9);
                        break;
                    default:
                        break;
                    }
                printf("Caractere pressionado: '%c'\n", c); // Informa o caractere pressionado via Monitor serial
                char str[2] = {c, '\0'};  // Cria uma string com o caractere e o terminador nulo
                ssd1306_draw_string(&ssd, " Pressionou:   ", 16, 10); // Coloca a frase pressionou no display
                ssd1306_draw_string(&ssd, str, 60, 30);  // Coloca o caractere pressionado logo abaixo da frase acima
                ssd1306_send_data(&ssd);// Envia os dados para o display
            }
        }
        sleep_ms(20);// Pequeno atraso para não sobrecarregar o microcontrolador
    }

    return 0;
}


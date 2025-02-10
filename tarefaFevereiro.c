// Bibliotecas utilizadas para funcionalidades do sistema
#include "hardware/pio.h"  
#include "hardware/i2c.h" 
#include "pico/stdlib.h" 
#include "ws2818b.pio.h"
#include "ssd1306.h"   
#include <stdlib.h>   
#include <stdio.h>   
#include <math.h>   
#include "font.h"  

// Definições dos pinos para comunicação I2C
#define I2C_PORT i2c1  
#define I2C_SDA 14  
#define I2C_SCL 15     
#define ENDERECO 0x3C 

// Definições dos pinos do LED RGB
#define BLUE 12  
#define GREEN 11 
#define RED 13  

// Função para inicializar os pinos do LED RGB
void led_init() {
  gpio_init(RED);  
  gpio_init(GREEN);
  gpio_init(BLUE); 
  gpio_set_dir(RED, GPIO_OUT);   
  gpio_set_dir(GREEN, GPIO_OUT);
  gpio_set_dir(BLUE, GPIO_OUT); 
}

// Função para controlar o estado dos LEDs RGB
void led_vector(bool rr, bool gg, bool bb) {
  led_init();        
  gpio_put(RED, rr);    
  gpio_put(GREEN, gg);  
  gpio_put(BLUE, bb);  
}

ssd1306_t ssd;  // Declara a variável ssd de forma global
// Inicialização e configurar do I2C e do ssd_d OLED SSD1306 
void ssd_d() {
    i2c_init(I2C_PORT, 400 * 1000); 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);  
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); 
    ssd1306_config(&ssd);    
    ssd1306_send_data(&ssd); 

    ssd1306_fill(&ssd, false); 
    ssd1306_send_data(&ssd); 
}

// Configurações dos botões
#define DEBOUNCE_DELAY 300 
#define BOTAO_A 5    
#define BOTAO_B 6    

volatile uint32_t last_irq_time_A = 0; 
volatile uint32_t last_irq_time_B = 0;

bool estado_LED_A = false; 
bool estado_LED_B = false;

void botao_callback(uint gpio, uint32_t eventos); // Declaração do callback para interrupção dos botões

// Função para inicializar os botões
void button_init(void) {
    gpio_init(BOTAO_A);
    gpio_init(BOTAO_B);

    gpio_set_dir(BOTAO_A, GPIO_IN); 
    gpio_set_dir(BOTAO_B, GPIO_IN);

    gpio_pull_up(BOTAO_A); 
    gpio_pull_up(BOTAO_B);

    // Configura interrupções com callback para os botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, botao_callback);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, botao_callback);
}

// Função para tratar debounce e alternar o estado dos LEDs
void debounce_botao(uint pino, volatile uint32_t *last_irq_time, bool *estado_LED) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time()); 
    if (tempo_atual - *last_irq_time > DEBOUNCE_DELAY) { 
       
        *last_irq_time = tempo_atual;  
        *estado_LED = !(*estado_LED); 

        if (pino == BOTAO_A) { 
            bool cor = true;
            ssd1306_fill(&ssd, !cor); 
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); 
            ssd1306_draw_string(&ssd, "Acionando o", 20, 10);
            ssd1306_draw_string(&ssd, "Led verde", 20, 30); 
            ssd1306_draw_string(&ssd, *estado_LED ? "" : "", 20, 50); 
            ssd1306_send_data(&ssd); 
            printf("acionando led verde %s\n", *estado_LED ? "" : "");
            led_vector(0, *estado_LED, 0); 

        } else if (pino == BOTAO_B) { 
            bool cor = true;
            ssd1306_fill(&ssd, !cor); 
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); 
            ssd1306_draw_string(&ssd, "Acionando o", 20, 10); 
            ssd1306_draw_string(&ssd, "Led azul", 20, 30);
            ssd1306_draw_string(&ssd, *estado_LED ? "" : "", 20, 50); 
            ssd1306_send_data(&ssd); 
            printf("Acionando Led azul %s\n", *estado_LED ? "" : "");
            led_vector(0, 0,*estado_LED); 
}}}

// Callback chamado quando ocorre interrupção em algum botão
void botao_callback(uint gpio, uint32_t eventos) {
    if (gpio == BOTAO_A) { 
        debounce_botao(BOTAO_A, &last_irq_time_A, &estado_LED_A);
    } else if (gpio == BOTAO_B) { 
        debounce_botao(BOTAO_B, &last_irq_time_B, &estado_LED_B);
}}

//CONFIGURAÇÃO DA MATRIZ WS2812
#define PINO_MATRIZ 7 
#define NUM_LEDS 25  
// Controlando o brilho para 30% (valor máximo é 255)
int BRILHO = 5;
// Função para converter as posições (x, y) da matriz para um índice do vetor de LEDs
int getIndex(int x, int y) {
    if (x % 2 == 0) { 
        return 24 - (x * 5 + y); 
    } else { 
        return 24 - (x * 5 + (4 - y)); 
}}

// Definição da estrutura de cor para cada LED
struct pixel_t {
    uint8_t R, G, B; 
};
typedef struct pixel_t npLED_t; 
npLED_t leds[NUM_LEDS]; 

// PIO e led_vector machine para CRTL dos LEDs
PIO np_pio; 
uint sm;  

// Função para atualizar os LEDs da matriz
void buffer() {
    for (uint i = 0; i < NUM_LEDS; ++i) { // Percorre todos os LEDs
        pio_sm_put_blocking(np_pio, sm, leds[i].R); 
        pio_sm_put_blocking(np_pio, sm, leds[i].G); 
        pio_sm_put_blocking(np_pio, sm, leds[i].B); 
    }
    sleep_us(100); // Aguarda 100 microsegundos para estabilizar
}
// Função de CRTL inicial da matriz de LEDs
void CRTL(uint pino) {
    uint offset = pio_add_program(pio0, &ws2818b_program); 
    np_pio = pio0; // Seleciona o PIO 0
    sm = pio_claim_unused_sm(np_pio, true); 
    ws2818b_program_init(np_pio, sm, offset, pino, 800000.f); 

    for (uint i = 0; i < NUM_LEDS; ++i) { 
        leds[i].R = leds[i].G = leds[i].B = 0;
    }
    buffer(); 
}
// Função para configurar a cor de um LED específico
void np_color(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[indice].R = r; 
    leds[indice].G = g; 
    leds[indice].B = b; 
}
// Função para off_matrixr todos os LEDs
void off_matrix() {
    for (uint i = 0; i < NUM_LEDS; ++i) { 
        np_color(i, 0, 0, 0); 
    }
    buffer(); 
}

// DESENHAR OS NÚMEROS NA MATRIZ DE LEDS
void desenho0() {  // Função para desenhar o número 0 na matriz de LEDs
    int mat1[5][5][3] = {  // Matriz tridimensional para representar a cor de cada LED (5x5)
        {{0, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}},   
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},      
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},     
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},    
        {{0, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}}
    };
    // Loop para percorrer cada linha da matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {  
            int posicao = getIndex(linha, cols);
            
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();  
}
void desenho1() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 10}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho2() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 10, BRILHO}, {0, 10, BRILHO}, {0, 10, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 10, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 10, BRILHO}, {0, 10, BRILHO}, {0, 10, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 10, BRILHO}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 10, BRILHO}, {0, 10, BRILHO}, {0, 10, BRILHO}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho3() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0, 0  }},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0,   0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho4() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho5() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho6() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }} buffer();
}
void desenho7() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, BRILHO, BRILHO}, {0, BRILHO, BRILHO}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho8() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }}  buffer();
}
void desenho9() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        } } buffer();
}

char atualizar = 0; // Estado atual do sistema
void comando(char atualizar) {
    switch (atualizar) {
        case '0': desenho0(); break;
        case '1': desenho1(); break;
        case '2': desenho2(); break;
        case '3': desenho3(); break;
        case '4': desenho4(); break;
        case '5': desenho5(); break;
        case '6': desenho6(); break;
        case '7': desenho7(); break;
        case '8': desenho8(); break;
        case '9': desenho9(); break;        
        default:  off_matrix();  led_vector(0,0,0); break;
}}

int main() {
    CRTL(PINO_MATRIZ);
    stdio_init_all();   
    button_init();   
    led_init();     
    ssd_d();        
    bool cor = true; 

      // Exibição inicial no ssd_d OLED
      ssd1306_fill(&ssd, !cor);
      ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);        
      ssd1306_draw_string(&ssd, "Interaja", 8, 10);       
      ssd1306_draw_string(&ssd, "com os botoes", 8, 30); 
      ssd1306_draw_string(&ssd, "!!!", 8, 50); 
      ssd1306_send_data(&ssd);                         

// Loop infinito para manter a aplicação em execução
while (true) {
    cor = !cor;  
    ssd1306_send_data(&ssd);  
    sleep_ms(1000);  

    printf("Interaja com o teclado: ");                         
    fflush(stdout);                                         
    if (scanf(" %c", &atualizar) == 1) {                   
        comando(atualizar);                               
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);  
        ssd1306_draw_string(&ssd, &atualizar, 63, 29);
        ssd1306_send_data(&ssd);                                                            
    }}
    return 0;
}

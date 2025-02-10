🛠️ Projeto: Interfaces de Comunicação Serial com RP2040

🐖 Sumário

🎯 Objetivos

👌 Descrição do Projeto

⚙️ Funcionalidades Implementadas

🛠️ Requisitos do Projeto

💂️ Estrutura do Repositório

🖥️ Como Compilar

🤝 Contribuições

🎦 Demonstração em Vídeo

💡 Considerações Finais

---

🎯 Objetivos

- Entender o funcionamento e a aplicação da comunicação serial em microcontroladores.
- Aplicar conceitos de UART e I2C na prática.
- Trabalhar com LEDs comuns e LEDs endereçáveis WS2812.
- Implementar soluções para botões utilizando interrupções e debounce.
- Desenvolver um projeto que integre hardware e software.

---

👌 Descrição do Projeto

Este projeto foi desenvolvido utilizando a placa BitDogLab, que conta com os seguintes componentes:

- Matriz 5x5 de LEDs WS2812 (GPIO 7)
- LED RGB (GPIOs 11, 12 e 13)
- Botão A (GPIO 5)
- Botão B (GPIO 6)
- Display SSD1306 via I2C (GPIO 14 (SDA) e GPIO 15 (SCL))

---

⚙️ Funcionalidades Implementadas

- **Modificação da Biblioteca font.h:**
  - Inclusão de caracteres minúsculos personalizados.
- **Entrada de Caracteres via Serial Monitor:**
  - Exibição dos caracteres digitados no display SSD1306.
  - Representação de números entre 0 e 9 na matriz 5x5.
- **Interação com Botões:**
  - **Botão A:** Alterna o estado do LED Verde RGB, com registro no display e no Serial Monitor.
  - **Botão B:** Alterna o estado do LED Azul RGB, com registro no display e no Serial Monitor.

---

🛠️ Requisitos do Projeto

- **Interrupções:** Controle dos eventos dos botões.
- **Debounce:** Implementação via software para evitar leituras erradas dos botões.
- **Controle de LEDs:** Utilização da matriz WS2812 e dos LEDs RGB individuais.
- **Uso do Display SSD1306:** Exibição de fontes em diferentes tamanhos e estilos.
- **Comunicação via UART:** Envio de informações para o Serial Monitor.
- **Organização do Código:** Código limpo, bem estruturado e comentado.

---

💂️ Estrutura do Repositório

```
├── tarefa03fev.c      # Código principal do projeto
├── font.h               # Biblioteca com fontes modificadas
├── README.md            # Este arquivo
├── ws2818b.pio.h        # Biblioteca para controle da matriz de LEDs
├── ssd1306.c            # Implementação das funções para o display
├── ssd1306.h            # Cabeçalho para uso da biblioteca do display
└── ...                  # Demais arquivos necessários
```

---

🖥️ Como Compilar

1. Clone o repositório:
   ```sh
   https://github.com/zrcrd/tarefa03Fev.git
   ```
2. Acesse a pasta do projeto:
   ```sh
   cd tarefaU03fev
   ```
3. Compile o projeto no ambiente configurado para RP2040.
4. Carregue o código na placa BitDogLab.

### 🖥️ Método Alternativo:

1. Baixe o repositório como um arquivo ZIP.
2. Extraia o conteúdo para uma pasta de fácil acesso.
3. Utilize a extensão Raspberry Pi Pico no VS Code para importar o projeto.
4. Aguarde a criação da pasta **build**.
5. Clique em "Compile" para compilar o código.
6. Utilize "RUN" com a BitDogLab no modo **boot select** para transferir o programa para a RP2040.
7. Agora, interaja com os botões e o teclado para explorar as funcionalidades do projeto.

---

🧑‍💻 Autor

**José Ricardo de Souza Santos**

---

📝 Descrição

Este projeto foi desenvolvido como parte das atividades do **Cepedi** no curso **EmbarcaTech em Software e Sistemas Embarcados**. O objetivo principal foi explorar a comunicação serial via **UART, I2C e SPI**, além da integração de hardware com o microcontrolador **RP2040**.

---

🤝 Contribuições

Este projeto foi criado por José Ricardo de Souza Santos, mas contribuições são sempre bem-vindas! Para contribuir, siga os passos abaixo:

1. Faça um fork do repositório.
2. Crie uma nova branch:
   ```sh
   git checkout -b minha-feature
   ```
3. Realize suas alterações e faça um commit:
   ```sh
   git commit -m 'Adicionando nova funcionalidade'
   ```
4. Envie suas alterações:
   ```sh
   git push origin minha-feature
   ```
5. Abra um **Pull Request**.

---

🎦 Demonstração em Vídeo

O vídeo de demonstração do projeto está disponível no Drive: https://drive.google.com/file/d/1EBjomx8bZmN1-yTECOaX3Ln_fgl4rjzq/view?usp=drive_link

---

💡 Considerações Finais

Este projeto oferece uma excelente oportunidade para reforçar conhecimentos sobre **comunicação serial (UART, SPI, I2C)**, controle de hardware e desenvolvimento com microcontroladores.

Se tiver dúvidas ou sugestões, sinta-se à vontade para contribuir!

---


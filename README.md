# 💡 Ray Tracing em Fragment Shader  
### Renderização Procedural em OpenGL 4.1  

---

## 🎓 Contexto Acadêmico

Este repositório apresenta o código desenvolvido para o **Trabalho Prático 2 da disciplina Computação Interativa e Visualização**, no âmbito do **Mestrado em Engenharia Informática** da **Universidade da Beira Interior (UBI)**.  

- **Aluno:** Reges Hengen  
- **Número de aluno:** M13521  
- **Ano letivo:** 2025  
- **Tema do trabalho:** *Real-Time Ray Tracing Using Fragment Shaders in OpenGL*  

---

## 🧠 Descrição do Projeto

O projeto implementa um **ray tracer em tempo real** utilizando o **estágio de fragment shader** do pipeline programável do **OpenGL 4.1**.  

O sistema realiza o traçado de raios a partir da câmera, detecta interseções com objetos tridimensionais (esferas) e aplica cálculos de **iluminação difusa, especular, sombras e reflexões** em tempo real.  

---

## 🧰 Tecnologias Utilizadas

- **Linguagem:** C++17  
- **API gráfica:** OpenGL 4.1 Core Profile  
- **Bibliotecas:**  
  - [GLFW](https://www.glfw.org/) – Criação da janela e contexto OpenGL  
  - [GLEW](http://glew.sourceforge.net/) – Carregamento de extensões OpenGL  
  - [GLM](https://github.com/g-truc/glm) – Matemática 3D (vetores e matrizes)  

---

## ⚙️ Dependências (macOS)

Instale as dependências via **Homebrew**:

```bash
brew install glfw glew glm
```

---

## 🧩 Estrutura de Arquivos

```bash
📦 raytracer-frag
├── main.cpp              # Código principal (setup OpenGL, shaders e render loop)
├── shaders/
│   ├── fullscreen.vert   # Vertex Shader (triângulo em tela cheia)
│   └── raytrace.frag     # Fragment Shader (implementa o ray tracing)
└── README.md             # Este arquivo
```

---

## 🚀 Como Compilar e Executar

No terminal, navegue até o diretório do projeto e execute:

```bash
clang++ main.cpp -o raytracer -std=c++17 \
-I/opt/homebrew/opt/glew/include \
-I/opt/homebrew/opt/glfw/include \
-I/opt/homebrew/opt/glm/include \
-L/opt/homebrew/opt/glew/lib \
-L/opt/homebrew/opt/glfw/lib \
-framework OpenGL -lglfw -lglew
```

Em seguida, execute o programa:

```bash
./raytracer
```

---

## 🖼️ Resultado do Experimento

O programa renderiza uma cena composta por três esferas coloridas com iluminação direcional, sombras e reflexões simples.  
A execução ocorre em tempo real, explorando o paralelismo do *fragment shader* para o cálculo independente de cada pixel.  

<p align="center">
  <img width="800" height="628" alt="image" src="https://github.com/user-attachments/assets/9f5c0ea2-f7d3-40af-9055-3bcb1ebc4060" />
  <em>Figura 1 – Resultado da execução do ray tracer em fragment shader.</em>
</p>

---

## 📚 Referência Técnica

Baseado no material de apoio disponível em  
👉 [Ray Tracing in One Weekend Series](https://raytracing.github.io/)  
e nas especificações da API OpenGL:  
👉 Kessenich, J., Baldwin, D., & Rost, R. (2016). *The OpenGL Shading Language* (4.60 Specification).  

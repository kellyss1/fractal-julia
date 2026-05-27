#include <fmt/core.h>
#include <SFML/Graphics.hpp>

#include <complex>
#include "fractal_serial.h"
#include "fractal_simd.h"
#include "fractal_openmp.h"
#include "palette.h"
#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

// dimesion de la imagen
#define WIDTH 1600
#define HEIGHT 900

// parametros de la imagen
int max_iteraciones = 10;

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

std::complex<double> c(-0.7, 0.27015);

// textura
uint32_t *pixel_buffer = nullptr;
uint16_t *texture_buffer = nullptr;

enum runtime_type
{
    SERIAL_1 = 0,
    SERIAL_2,
    SIMD,
    OPENMP,
    PARALLEL_FOR
};

int main()
{

    runtime_type r_type = runtime_type::SERIAL_1;

    // Inicializamos el buffer de pixeles y la textura
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];
    texture_buffer = new uint16_t[WIDTH * HEIGHT];

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia set - SMFL");

#ifdef _WIN32
    HWND hWnd = window.getNativeHandle();
    ShowWindow(hWnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({WIDTH, HEIGHT});
    sf::Sprite sprite(texture);

    sf::Font font("Arial.ttf");
    sf::Text text(font, "Julia Set", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    std::string options = "Options: [1] Serial 1 [2] Serial 2 [3] SIMD [4] OpenMP [5] Parallel For | Up/Down: Change iterations";
    sf::Text textOptions(font, options, 20);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setPosition({10, window.getView().getSize().y - 40});

    // FPS
    int frames = 0;
    int fps = 0;
    sf::Clock clock;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();
                if (!evt)
                {
                    continue;
                }

                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iteraciones += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iteraciones -= 10;
                    if (max_iteraciones < 10)
                        max_iteraciones = 10;
                    break;
                case sf::Keyboard::Scan::Num1:
                    r_type = runtime_type::SERIAL_1;
                    break;
                case sf::Keyboard::Scan::Num2:
                    r_type = runtime_type::SERIAL_2;
                    break;
                case sf::Keyboard::Scan::Num3:
                    r_type = runtime_type::SIMD;
                    break;
                case sf::Keyboard::Scan::Num4:
                    r_type = runtime_type::OPENMP;
                    break;
                case sf::Keyboard::Scan::Num5:
                    r_type = runtime_type::PARALLEL_FOR;
                    break;
                }
                std::memset(pixel_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t)); // limpiar el buffer de pixeles para evitar artefactos al cambiar de modo o iteraciones
            }
        }

        // Crear la textura
        std::string mode = "";
        std::string paletteName = "";
        std::string threadInfo = "";
        if (r_type == runtime_type::SERIAL_1)
        {
            color_ramp = color_ramp; // paleta por defecto (no cambia)
            paletteName = "Default";
            julia_serial_1(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
            mode = "Serial 1";
        }
        else if (r_type == runtime_type::SERIAL_2)
        {
            color_ramp = color_ramp_blue;
            paletteName = "Blue";
            julia_serial_2(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
            mode = "Serial 2";
        }
        else if (r_type == runtime_type::SIMD)
        {
            color_ramp = color_ramp_green;
            paletteName = "Green";
            julia_simd(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
            mode = "SIMD";
        }
        else if (r_type == runtime_type::OPENMP)
        {
            color_ramp = color_ramp_mono;
            paletteName = "Mono";
            julia_openmp_regiones(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
            mode = "OpenMP (regions)";
#ifdef _OPENMP
            threadInfo = fmt::format("Threads: {}", omp_get_max_threads());
#else
            threadInfo = "Threads: 1";
#endif
        }
        else if (r_type == runtime_type::PARALLEL_FOR)
        {
            color_ramp = color_ramp_mono;
            paletteName = "Mono";
            julia_openmp_for(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
            mode = "OpenMP (parallel for)";
#ifdef _OPENMP
            threadInfo = fmt::format("Threads: {}", omp_get_max_threads());
#else
            threadInfo = "Threads: 1";
#endif
        }

        texture.update((const uint8_t *)pixel_buffer); // BUFFER DE TEXTURAS QUE REQUIERE

        // Contar FPS
        frames++;

        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }

        // Actualizar el titulo de la ventana con el FPS
        auto msg = fmt::format("Julia set Iterations: {} FPS: {} Mode: {} Palette: {} {}", max_iteraciones, fps, mode, paletteName, threadInfo);
        text.setString(msg);

        // Clear screen
        window.clear();

        // Draw scene
        window.draw(sprite);
        window.draw(text);
        window.draw(textOptions);
        // Update the window
        window.display();
    }

    delete[] pixel_buffer;
    delete[] texture_buffer;

    return 0;
}
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>

void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::VertexArray* src);
void KeyboardInteractions(float *x_start, float *y_start, float *zoom);

const int nmax = 256;
const float r2max = 10000.0f;
const int scrnWidth = 800;
const int scrnHeight = 600;

int main()
{
    sf::RenderWindow window(sf::VideoMode(scrnWidth, scrnHeight), "SFML");
    float x_start = 0, y_start = 0, zoom = 1;

    sf::Font font; 
    font.loadFromFile("arial.ttf"); //загрузка шрифта 

    sf::Text text;
    text.setFont(font); 
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White); 
    text.setPosition(0.f, 0.f); 

    sf::Clock clock;

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); 
        }

        KeyboardInteractions(&x_start, &y_start, &zoom);

        sf::VertexArray mypoint(sf::Points, scrnHeight * scrnWidth);

        sf::Time start = clock.restart(); //перезапуск времени, возвращает пройденное время

        CountMandelbrot(&x_start, &y_start, &zoom, &mypoint);
        sf::Time finish = clock.restart();
        
        text.setString("FPS: " + std::to_string((int)(1 / finish.asSeconds()))); //сама строка

        window.draw(mypoint);
        window.draw(text);
        window.display(); //отображение кадра
    }

}

void CountMandelbrot(float* x_start, float* y_start, float* zoom, sf::VertexArray* src) 
{ 
    float dx = 0.001f / float(*zoom), 
          dy = 0.001f / float(*zoom);

    for (int iy = 0; iy < scrnHeight; iy++)
    {     
        float x0 = (( float(*x_start) * (*zoom) - (float)(scrnWidth / 2)) * dx);
        float y0 = ((float)iy + float(*y_start) * (*zoom) - (float)(scrnHeight / 2)) * dy;

        for (int ix = 0; ix < scrnWidth; ix++, x0 += dx)
        {
            float x = x0,
                  y = y0;

            int n = 0;

            for (; n < nmax; n++)
            {
                float x2 = x * x,
                      y2 = y * y,
                      xy = x * y;

                float r2 = x2 + y2;

                if (r2 >= r2max) break;

                x = x2 - y2 + x0;
                y = xy + xy + y0;

            }

            sf::Color color;
            if (n < nmax) 
            {
                double I = sqrtf(sqrtf((float)n / (float)nmax)) * 255.f;
                color = sf::Color(I, 128, 255 - I / 2, I);
            }

            else { color = sf::Color(); }

            (*src)[ix + iy * scrnWidth].position = {ix, iy};
            (*src)[ix + iy * scrnWidth].color = color;
        }
    }

}

void KeyboardInteractions(float *x_start, float *y_start, float *zoom)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            *x_start += 40 / float(*zoom);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
            *x_start -= 40 / float(*zoom);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) 
            *y_start -= 40 / float(*zoom);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
            *y_start += 40 / float(*zoom);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
            *zoom *= 1.205;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
            *zoom *= 0.895;
}
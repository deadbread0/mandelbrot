#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#define NOPICTURE

void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::Uint8* src);
void KeyboardInteractions(float *x_start, float *y_start, float *zoom);

const int NMAX = 256;
const float R2MAX = 10000.0f;
const int SCRNWIDTH = 800;
const int SCRNHEIGHT = 600;

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCRNWIDTH, SCRNHEIGHT), "SFML");
    float x_start = 0, y_start = 0, zoom = 1;

    sf::Font font; 
    font.loadFromFile("arial.ttf"); //загрузка шрифта 

    sf::Text text;
    text.setFont(font); 
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White); 
    text.setPosition(0.f, 0.f); 

    sf::Clock clock;

    #ifdef NOPICTURE
    int counter = 0;
    float sum = 0;
    #endif

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); 
        }

        KeyboardInteractions(&x_start, &y_start, &zoom);

        sf::Uint8* pixels = new sf::Uint8[SCRNWIDTH * SCRNHEIGHT * 4]; // * 4 т.к. пиксели имеют четыре компоненты (RGBA)

        // #ifndef NOPICTURE //по логике без этого ifndef должно быть самое точное время, но оно часто 0...
        sf::Time start = clock.restart(); //перезапуск времени, возвращает пройденное время
        // #endif

        CountMandelbrot(&x_start, &y_start, &zoom, pixels);
        sf::Time finish = clock.restart();

        #ifdef NOPICTURE
        if (counter < 1000)
        {
            fprintf(stderr, "time №%d: %f ms\n", counter + 1, (float)finish.asMicroseconds() / 1000);
            sum += (float)finish.asMicroseconds() / 1000;
        }
        else
        {
            window.close();
            fprintf(stderr, "mean time: %f ms\n", sum / (counter + 1));
        }
        counter++;
        #endif

        #ifndef NOPICTURE
        text.setString("FPS: " + std::to_string((int)(1 / finish.asSeconds()))); //сама строка

        sf::Texture texture;
        texture.create(SCRNWIDTH, SCRNHEIGHT);
        sf::Sprite sprite;

        texture.update(pixels);
        sprite.setTexture(texture);

        window.draw(sprite);

        window.display();
        window.draw(text);
        window.display(); //отображение кадра
        #endif
    }

}

void CountMandelbrot(float* x_start, float* y_start, float* zoom,  sf::Uint8* src) 
{ 
    float dx = 0.001f / float(*zoom), 
          dy = 0.001f / float(*zoom);

    for (int iy = 0; iy < SCRNHEIGHT; iy++)
    {     
        float x0 = (( float(*x_start) * (*zoom) - (float)(SCRNWIDTH / 2)) * dx);
        float y0 = ((float)iy + float(*y_start) * (*zoom) - (float)(SCRNHEIGHT / 2)) * dy;

        for (int ix = 0; ix < SCRNWIDTH; ix++, x0 += dx)
        {
            float x = x0,
                  y = y0;

            int n = 0;

            for (; n < NMAX; n++)
            {
                float x2 = x * x,
                      y2 = y * y,
                      xy = x * y;

                float r2 = x2 + y2;

                if (r2 >= R2MAX) break;

                x = x2 - y2 + x0;
                y = xy + xy + y0;

            }

            #ifndef NOPICTURE
            sf::Color color;
            size_t point_pos = (iy * SCRNWIDTH + ix) * 4;

            if (n < NMAX) 
            {
                double I = sqrtf(sqrtf((float)n / (float)NMAX)) * 255.f;
                // color = sf::Color(I, 128, 255 - I / 2, I);
                sf::Uint8 color = (sf::Uint8) I;

                src[point_pos] = color;
                src[point_pos + 1] = 128;
                src[point_pos + 2] = 255 - color / 2;
                src[point_pos + 3] = 255;
            }

            else 
            { 
                src[point_pos] = 0;
                src[point_pos + 1] = 0;
                src[point_pos + 2] = 0;
                src[point_pos + 3] = 255; 
            }
            #endif
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
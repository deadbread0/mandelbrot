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


void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::VertexArray* src) 
{ 
    for (int iy = 0; iy < scrnHeight; iy++) 
    {    
        float dx = 0.001f / float(*zoom), 
              dy = 0.001f / float(*zoom);

        float x0 = (( float(*x_start) * (*zoom) - (float)(scrnWidth / 2)) * dx);
        float y0 = ((float)iy + float(*y_start) * (*zoom) - (float)(scrnHeight / 2)) * dy;
        
        for (int ix = 0; ix < scrnWidth; x0 += dx * 8) 
        {
            float X0[8] = {x0, x0 + dx, x0 + 2 * dx, x0 + 3 * dx, x0 + 4 * dx, x0 + 5 * dx, x0 + 6 * dx, x0 + 7 * dx};
            float Y0[8] = {y0, y0, y0, y0, y0, y0, y0, y0};
            
            float X[8] = {}; for (int i = 0; i < 8; i++) {X[i] = X0[i];}
            float Y[8] = {}; for (int i = 0; i < 8; i++) {Y[i] = Y0[i];}
            int N[8] = {0};

            for (int n = 0; n < nmax; n++) 
            {
                float X2[8] = {}; for (int i = 0; i < 8; i++) {X2[i] = X[i] * X[i];}
                float Y2[8] = {}; for (int i = 0; i < 8; i++) {Y2[i] = Y[i] * Y[i];}
                float XY[8] = {}; for (int i = 0; i < 8; i++) {XY[i] = X[i] * Y[i];}

                float R2[8] = {}; for (int i = 0; i < 8; i++) {R2[i] = X2[i] + Y2[i];}

                int cmp[8] = {};
                for (int i = 0; i < 8; i++) if (R2[i] <= r2max) cmp[i] = 1;
                
                int mask = 0;
                for (int i = 0; i < 8; i++) mask |= (cmp[i] << i);
                if (!mask) break;

                for (int i = 0; i < 8; i++) N[i] = N[i] + cmp[i];

                for (int i = 0; i < 8; i++) X[i] = X2[i] - Y2[i] + X0[i];
                for (int i = 0; i < 8; i++) Y[i] = XY[i] + XY[i] + Y0[i];
            }

            for (int i = 0; i < 8; i++, ix++) 
            {
                sf::Color color;
                if (N[i] < nmax) {
                    double I = sqrtf(sqrtf((float)N[i] / (float)nmax)) * 255.f;
                    color = sf::Color(I, 128, 255 - I / 2, I);
                }

                else { color = sf::Color(); }

                (*src)[ix + iy * scrnWidth].position = {ix, iy};
                (*src)[ix + iy * scrnWidth].color = color;
            }
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
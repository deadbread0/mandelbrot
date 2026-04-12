#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#pragma GCC push_options
#pragma GCC target("avx2")
#pragma GCC target("avx")
#include <immintrin.h>
#include <bits/basic_string.h>

void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::VertexArray* src);
void KeyboardInteractions(float *x_start, float *y_start, float *zoom);
                              
const int nmax = 256;
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
    __m256 r2max = _mm256_set1_ps(10000.0f);
    __m256 arr01234567 = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f); 

    float dx = 0.001f / float(*zoom), 
          dy = 0.001f / float(*zoom);

    for (int iy = 0; iy < scrnHeight; iy++) 
    {    
       __m256 DX = _mm256_set1_ps(dx); DX = _mm256_mul_ps(DX, arr01234567);

        float x0 = (( float(*x_start) * (*zoom) - (float)(scrnWidth / 2)) * dx);
        float y0 = ((float)iy + float(*y_start) * (*zoom) - (float)(scrnHeight / 2)) * dy;
        
        for (int ix = 0; ix < scrnWidth; x0 += dx * 8) 
        {
            __m256 X0 = _mm256_set1_ps(x0); X0 = _mm256_add_ps(X0, DX);
            __m256 Y0 = _mm256_set1_ps(y0);
                
            __m256 X = X0;
            __m256 Y = Y0;
            __m256i N = _mm256_setzero_si256();
            
            for (int n = 0; n < nmax; n++) 
            {
                __m256 X2 = _mm256_mul_ps(X, X);
                __m256 Y2 = _mm256_mul_ps(Y, Y);
                __m256 XY = _mm256_mul_ps(X, Y);

                __m256 R2 = _mm256_add_ps(X2, Y2);

                __m256 cmp = _mm256_mul_ps(_mm256_cmp_ps(R2, r2max, _CMP_LE_OQ), _mm256_set1_ps(1.f));

                int mask = _mm256_movemask_ps(cmp);
                if (!mask) break;

                N = _mm256_sub_epi32(N, _mm256_castps_si256(cmp));

                X = _mm256_sub_ps(X2, Y2); X = _mm256_add_ps(X, X0);
                Y = _mm256_add_ps(XY, XY); Y = _mm256_add_ps(Y, Y0);
            }

            for (int i = 0; i < 8; i++, ix++) 
            {
                sf::Color color;

                int iterationsArray[8] = {};
                _mm256_storeu_si256((__m256i*)iterationsArray, N);
                if (iterationsArray[i] < nmax) 
                {
                    double I = sqrtf(sqrtf((float)iterationsArray[i] / (float)nmax)) * 255.f;
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

#pragma GCC pop_options
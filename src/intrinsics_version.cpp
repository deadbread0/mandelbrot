#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#pragma GCC push_options
#pragma GCC target("avx2")
#pragma GCC target("avx")
#include <immintrin.h>
#include <bits/basic_string.h>
// #define NOPICTURE

void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::Uint8* src);
void KeyboardInteractions(float *x_start, float *y_start, float *zoom);
                              
const int NMAX = 256;
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

        sf::Time start = clock.restart(); //перезапуск времени, возвращает пройденное время

        CountMandelbrot(&x_start, &y_start, &zoom, pixels);
        sf::Time finish = clock.restart();

        #ifdef NOPICTURE
        if (counter < 500)
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


void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::Uint8* src) 
{ 
    __m256 r2max = _mm256_set1_ps(10000.0f);
    __m256 arr01234567 = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f); 

    float dx = 0.001f / float(*zoom), 
          dy = 0.001f / float(*zoom);

    for (volatile int iy = 0; iy < SCRNHEIGHT; iy++) 
    {    
       __m256 DX = _mm256_set1_ps(dx); DX = _mm256_mul_ps(DX, arr01234567);

        float x0 = (( float(*x_start) * (*zoom) - (float)(SCRNWIDTH / 2)) * dx);
        float y0 = ((float)iy + float(*y_start) * (*zoom) - (float)(SCRNHEIGHT / 2)) * dy;
        
        for (volatile int ix = 0; ix < SCRNWIDTH; x0 += dx * 8) 
        {
            __m256 X0 = _mm256_set1_ps(x0); X0 = _mm256_add_ps(X0, DX);
            __m256 Y0 = _mm256_set1_ps(y0);
                
            __m256 X = X0;
            __m256 Y = Y0;
            __m256i N = _mm256_setzero_si256();
            
            for (volatile int n = 0; n < NMAX; n++) 
            {
                __m256 X2 = _mm256_mul_ps(X, X);
                __m256 Y2 = _mm256_mul_ps(Y, Y);
                __m256 XY = _mm256_mul_ps(X, Y);

                __m256 R2 = _mm256_add_ps(X2, Y2);

                __m256 cmp = _mm256_cmp_ps(R2, r2max, _CMP_LE_OQ);

                int mask = _mm256_movemask_ps(cmp);
                if (!mask) break;

                N = _mm256_sub_epi32(N, _mm256_castps_si256(cmp));

                X = _mm256_sub_ps(X2, Y2); X = _mm256_add_ps(X, X0);
                Y = _mm256_add_ps(XY, XY); Y = _mm256_add_ps(Y, Y0);
            }

            #ifdef NOPICTURE
            ix += 8;
            #endif
            
            #ifndef NOPICTURE
            sf::Color color;
            
            int iterationsArray[8] = {};
            _mm256_storeu_si256((__m256i*)iterationsArray, N);

            for (int i = 0; i < 8; i++, ix++) 
            {
                size_t point_pos = (iy * SCRNWIDTH + ix) * 4;
                if (iterationsArray[i] < NMAX) 
                {
                    double I = sqrtf(sqrtf((float)iterationsArray[i] / (float)NMAX)) * 255.f;
                    sf::Uint8 color = (sf::Uint8) I;

                    src[point_pos] = color;
                    src[point_pos + 1] = 128;
                    src[point_pos + 2] = 255 - color / 2;
                    src[point_pos + 3] = color;
                }

                else 
                { 
                    src[point_pos] = 0;
                    src[point_pos + 1] = 0;
                    src[point_pos + 2] = 0;
                    src[point_pos + 3] = 255; 
                }
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

#pragma GCC pop_options
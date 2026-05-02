#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#define NOPICTURE

void CountMandelbrot (float* x_start, float* y_start, float* zoom, sf::Uint8* src);
void KeyboardInteractions(float *x_start, float *y_start, float *zoom);

void mm_cpy_ps(float arr0[8], float arr1[8]); /*arr0 присваиваются значения arr1*/ 
void mm_set_ps(float arr[8], float elem0, float elem1, float elem2, float elem3, 
                              float elem4, float elem5, float elem6, float elem7);
void mm_cpy_ps(float arr0[8], float arr1[8]); /*arr0 присваиваются значения arr1*/ 
void mm_add_ps(float arr0[8], float arr1[8], float arr2[8]); /*arr0 присваиваются значения arr1 + arr2*/ 
void mm_mul_ps(float arr0[8], float arr1[8], float arr2[8]); /*arr0 присваиваются значения arr1 + arr2*/ 
void mm_sub_ps(float arr0[8], float arr1[8], float arr2[8]); /*arr0 присваиваются значения arr1 - arr2*/ 
int mm_movemask_ps(int cmp[8]);
                              
const int NMAX = 256;
const float R2MAX = 10000.f;
const int SCRNWIDTH = 800;
const int SCRNHEIGHT = 600;
const float arr01234567[] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};

void mm_set_ps1(float arr[8], float elem) {for (int i = 0; i < 8; i++) arr[i] = elem;}

void mm_set_ps(float arr[8], float elem0, float elem1, float elem2, float elem3, 
                              float elem4, float elem5, float elem6, float elem7) 
    {arr[0] = elem0; arr[1] = elem1; arr[2] = elem2; arr[3] = elem3; 
     arr[4] = elem4; arr[5] = elem5; arr[6] = elem6; arr[7] = elem7;}

void mm_cpy_ps(float arr0[8], float arr1[8]) /*arr0 присваиваются значения arr1*/ 
{for (int i = 0; i < 8; i++) arr0[i] = arr1[i];}

void mm_add_ps(float arr0[8], float arr1[8], float arr2[8]) /*arr0 присваиваются значения arr1 + arr2*/ 
{for (int i = 0; i < 8; i++) arr0[i] = arr1[i] + arr2[i];}

void mm_mul_ps(float arr0[8], float arr1[8], float arr2[8]) /*arr0 присваиваются значения arr1 * arr2*/ 
{for (int i = 0; i < 8; i++) arr0[i] = arr1[i] * arr2[i];}

void mm_sub_ps(float arr0[8], float arr1[8], float arr2[8]) /*arr0 присваиваются значения arr1 - arr2*/ 
{for (int i = 0; i < 8; i++) arr0[i] = arr1[i] - arr2[i];}

int mm_movemask_ps(int cmp[8])
{int mask = 0; for (int i = 0; i < 8; i++) mask |= (cmp[i] << i); return mask;}

void mm_cmp_ps(float arr_for_cmp[8], int cmp[8])
{for (int i = 0; i < 8; i++) if (arr_for_cmp[i] <= R2MAX) cmp[i] = 1;}


int main()
{
    sf::RenderWindow window(sf::VideoMode(SCRNWIDTH, SCRNHEIGHT), "SFML");
    float x_start = 0, y_start = 0, zoom = 1;

    sf::Font font;
    font.loadFromFile("arial.ttf");

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
    float dx = 0.001f / float(*zoom), 
          dy = 0.001f / float(*zoom);

    for (volatile int iy = 0; iy < SCRNHEIGHT; iy++) 
    {    
        float DX[8] = {}; mm_set_ps1(DX, dx); mm_mul_ps(DX, DX, (float*)arr01234567);

        float x0 = (( float(*x_start) * (*zoom) - (float)(SCRNWIDTH / 2)) * dx);
        float y0 = ((float)iy + float(*y_start) * (*zoom) - (float)(SCRNHEIGHT / 2)) * dy;
        
        for (volatile int ix = 0; ix < SCRNWIDTH; x0 += dx * 8) 
        {
            float X0[8] = {}; mm_set_ps1(X0, x0); mm_add_ps(X0, X0, DX);
            float Y0[8] = {}; mm_set_ps1(Y0, y0);
            
            float X[8] = {}; mm_cpy_ps(X, X0);
            float Y[8] = {}; mm_cpy_ps(Y, Y0);
            int N[8] = {0};

            for (volatile int n = 0; n < NMAX; n++) {
                float X2[8] = {}; mm_mul_ps(X2, X, X);
                float Y2[8] = {}; mm_mul_ps(Y2, Y, Y);
                float XY[8] = {}; mm_mul_ps(XY, X, Y);

                float R2[8] = {}; mm_add_ps(R2, X2, Y2);

                int cmp[8] = {}; mm_cmp_ps(R2, cmp);
                
                int mask = mm_movemask_ps(cmp);
                if (!mask) break;

                mm_add_ps((float*)N, (float*)N, (float*)cmp);

                mm_sub_ps(X, X2, Y2); mm_add_ps(X, X, X0);
                mm_add_ps(Y, XY, XY); mm_add_ps(Y, Y, Y0);
            }

            #ifdef NOPICTURE
            ix += 8;
            #endif
            #ifndef NOPICTURE
            
            sf::Color color;
            for (int i = 0; i < 8; i++, ix++) 
            {
                size_t point_pos = (iy * SCRNWIDTH + ix) * 4; 

                if (N[i] < NMAX) 
                {
                    double I = sqrtf(sqrtf((float)N[i] / (float)NMAX)) * 255.f;
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
                    src[point_pos + 3] = 0; 
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
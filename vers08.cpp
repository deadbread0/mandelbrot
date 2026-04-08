// #define TX_USE_SFML
#include "TX\TXLib.h" 
#include <stdio.h>
#include <windows.h>
#include "simple_version.h"

int main()
{
    HWND wtf = txCreateWindow (800, 600);
    // HDC dc = txDC();
    // POINT size = txGetExtent();
    double size_x = 800, size_y = 600;

    RGBQUAD* buf = NULL;                                      // Do NOT actually create the array!
    HDC dc = txCreateDIBSection(size_x, size_y, &buf);
    COLORREF buff [600][800] = {};                                      // Do NOT actually create the array!
    
    double x_start = 0, y_start = 0, zoom = 1;
    
    // DrawMandelbrot(&dc, &buf, &x_start, &y_start);
    
     while (!GetAsyncKeyState (VK_ESCAPE))
                  {
                    DrawMandelbrot(&dc, buff, &x_start, &y_start, &zoom); // fake draw
                    ReallyDraw(&dc, buff);
                    txBitBlt (txDC(), 0, 0, size_x, size_y, dc, 0, 0);
                    if (txGetAsyncKeyState (VK_RIGHT)) x_start -= 15;
                    if (txGetAsyncKeyState (VK_LEFT)) x_start += 15;
                    if (txGetAsyncKeyState (VK_UP)) y_start += 15;
                    if (txGetAsyncKeyState (VK_DOWN)) y_start -= 15;
                    if (txGetAsyncKeyState (VK_OEM_PLUS)) zoom *= 1.005;
                    if (txGetAsyncKeyState (VK_OEM_MINUS)) zoom *= 0.995;
                    txSleep (0);
                  }

    txDeleteDC (dc);
    ReleaseDC (wtf, dc);

    return 0;
}

void DrawMandelbrot(HDC *dc, COLORREF buf[600][800], double *x_start, double *y_start, double *zoom)
{

    float xc = ( 400.f + ( float(*x_start) - 300.f) * 0.01f / float(*zoom) );
    float yc = ( float(*y_start) - 300.f ) * 0.01f / float(*zoom) + 300.f; 

    for (int iy = 0; iy < 600; iy++)
    {

        float dx = 0.001f, //если поставить оч мало, то сегфолта не будет
              dy = 0.001f / float(*zoom);
              
        float x0 = (( float(*x_start) - 300.f) * dx);
        float y0 = ((float)iy + float(*y_start) - 300.f) * dy;

        for (int ix = 0; ix < 800; x0 += 8 * dx)
        {
            float X0[8] = {x0, x0 + dx, x0 + dx * 2, x0 + dx * 3, x0 + dx * 4, x0 + dx * 5, x0 + dx * 6, x0 + dx * 7}; //сегфолт чзх?? если поставить везде x0, то все ок, просто картинка с низким разрешением 
            float Y0[8] = {y0, y0, y0, y0, y0, y0, y0, y0};

            float X[8] = {}; for (int i = 0; i < 8; i++) X[i] = X0[i];
            float Y[8] = {}; for (int i = 0; i < 8; i++) Y[i] = Y0[i];

            // x = (x - xc) / float(*zoom) + xc;
            // y = (y - yc) / float(*zoom) + yc;

            int N[8] = {0, 0, 0, 0, 0, 0, 0, 0};

            for (int n = 0; n < nmax; n++)
            {
                float x2[8] = {}; for (int i = 0; i < 8; i++) x2[i] = X[i] * X[i]; 
                float y2[8] = {}; for (int i = 0; i < 8; i++) y2[i] = Y[i] * Y[i];
                float xy[8] = {}; for (int i = 0; i < 8; i++) xy[i] = X[i] * Y[i];

                float r2[8] = {}; for (int i = 0; i < 8; i++) r2[i] = x2[i] + y2[i];

                int cmp[8] = {};
                for (int i = 0; i < 8; i++) if (r2[i] <= r2max) cmp[i] = 1; //продолжаем сравнение

                int mask = 0;
                for (int i = 0; i < 8; i++) mask |= (cmp[i] << i); // |= — это побитовое присваивание ИЛИ
                if (!mask) break;

                for (int i = 0; i < 8; i++) N[i] = N[i] + cmp[i];
                //что ж делать что ж делать...
                for (int i = 0; i < 8; i++) X[i] = x2[i] - y2[i] + X0[i]; //происходит нечто паранормальное, так возникает сегфолт только при таком раскладе (поочереди все перебрала ничего по отдельности не лажает и сумма даже печатается)
                for (int i = 0; i < 8; i++) Y[i] = xy[i] + xy[i] + Y0[i];
            }

            for (int i = 0; i < 8; i++, ix++)
            {
                double q = 50;
                COLORREF color = (N[i] < nmax)? COLORREF RGB(128 - q, q, 255 - q) : COLORREF RGB(0, 0, 0);
                buf[iy][ix] = color;
            }
        }
    }

    printf ("FPS %f\t\t\r", txGetFPS());
}

// void PrintSetLoopUnroll (float* x_start, float* y_start, sf::VertexArray* src) { //это вроде как рабочий код на sfml, но мне ж такое не подходит блин
//     float xc = ((float)scrnWidth  + *x_start - 300.f) / 2;
//     float yc = ((float)scrnHeight + *y_start - 300.f) / 2;

//     for (int iy = 0; iy < scrnHeight; iy++) {    
//         float x0 = (            *x_start - 300.f) * dx;
//         float y0 = ((float)iy + *y_start - 300.f) * dy;
        
//         for (int ix = 0; ix < scrnWidth; x0 += dx*4) {
//             float X0[4] = {x0, x0 + dx, x0 + 2*dx, x0 + 3*dx};
//             float Y0[4] = {y0, y0,      y0,        y0};
            
//             float X[4] = {}; for (int i = 0; i < 4; i++) {X[i] = X0[i];}
//             float Y[4] = {}; for (int i = 0; i < 4; i++) {Y[i] = Y0[i];}
//             int N[4] = {0};

//             for (int n = 0; n < nMax; n++) {
//                 float X2[4] = {}; for (int i = 0; i < 4; i++) {X2[i] = X[i] * X[i];}
//                 float Y2[4] = {}; for (int i = 0; i < 4; i++) {Y2[i] = Y[i] * Y[i];}
//                 float XY[4] = {}; for (int i = 0; i < 4; i++) {XY[i] = X[i] * Y[i];}

//                 float R2[4] = {}; for (int i = 0; i < 4; i++) {R2[i] = X2[i] * Y2[i];}

//                 int cmp[4] = {};
//                 for (int i = 0; i < 4; i++) if (R2[i] <= r2Max) cmp[i] = 1;
                
//                 int mask = 0;
//                 for (int i = 0; i < 4; i++) mask |= (cmp[i] << i);
//                 if (!mask) break;

//                 for (int i = 0; i < 4; i++) N[i] = N[i] + cmp[i];

//                 for (int i = 0; i < 4; i++) X[i] = X2[i] - Y2[i] + X0[i];
//                 for (int i = 0; i < 4; i++) Y[i] = XY[i] + XY[i] + Y0[i];
//             }

//             for (int i = 0; i < 4; i++, ix++) {
//                 sf::Color color;
//                 if (N[i] < nMax) {
//                     double I = sqrtf(sqrtf((float)N[i] / (float)nMax)) * 255.f;
//                     color = sf::Color(I, 128, 255 - I, 255);
//                 }

//                 else { color = sf::Color(); }

//                 (*src)[ix + iy * 800].position = {ix, iy};
//                 (*src)[ix + iy * 800].color = color;
//             }
//         }
//     }
// }

void ReallyDraw(HDC *dc, COLORREF buf[600][800])
{
    for (int iy = 0; iy < 600; iy++)
    {
        for (int ix = 0; ix < 800; ix++)
        {
            txSetPixel(ix, iy, buf[iy][ix], *dc);
        }
    }
}
